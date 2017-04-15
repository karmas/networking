#include "utils.h"
#include <sys/wait.h>
using namespace std;

void handleClient(int connfd)
{
  int ret = 0;
  const int bufCap = 10;
  char buf[bufCap] = {};
  while ((ret = read(connfd, buf, bufCap))) {
    if (ret > 0) {
      if (write(connfd, buf, ret) < 0) {
        perror("unable to write");
      }
    }
    else if (ret == -1) {
      if (errno != EINTR) {
        cout << "client read error" << endl;
        break;
      }
      else {
        cout << "recover from interruption" << endl;
      }
    }
  }
}

void sig_chld(int signo)
{
  pid_t pid = 0;
  int stat = 0;
  while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
    // not good to call IO in signal handler
    cout << "child pid = " << pid;
    cout << ", status = " << stat << endl;
  }
}

int main(int argc, const char *argv[])
{
  signal(SIGCHLD, sig_chld);
  assert("server ip in host:port format missing" && argc > 1);

  sockaddr_in serverAddress = {};
  assert("can't get server address" && getAddress(argv[1], serverAddress));

  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  passert("error creating listening socket", listenfd != -1);
  int ret = bind(listenfd, (const sockaddr *) &serverAddress, sizeof(serverAddress));
  passert("error binding", ret != -1);
  ret = listen(listenfd, 5);
  passert("error in listen()", ret != -1);

  sockaddr_in clientAddress = {};
  socklen_t clientAddressLen = sizeof(clientAddress);
  while (true) {
    int connfd = accept(listenfd, (sockaddr *) &clientAddress, &clientAddressLen);
    if (connfd == -1) {
      if (errno == EINTR) {
        cout << "handle interrupt" << endl;
        continue;
      }
      else
        break;
    }
    cout << "client connected: ";
    printAddress(clientAddress, cout);
    cout << endl;
    if (fork() == 0) {
      close(listenfd);
      handleClient(connfd);
      exit(0);
    }
    close(connfd);
  }

  return 0;
}
