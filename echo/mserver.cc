#include "utils.h"
using namespace std;

void useSelect(int listenfd)
{
  sockaddr_in clientAddr = {};
  socklen_t calen = sizeof(clientAddr);

  const int maxClients = 50;
  sockaddr_in clientAddrs[maxClients];
  bzero(clientAddrs, sizeof(clientAddrs));
  int clientfds[maxClients] = {};
  int num_clientfds = 0;
  const int bufCap = 50;
  char buf[bufCap] = {};

  fd_set fset, afset;
  FD_ZERO(&afset);
  FD_SET(listenfd, &afset);
  int maxfd = listenfd;

  while (true) {
    fset = afset;
    int ret = select(maxfd + 1, &fset, 0, 0, 0);

    if (FD_ISSET(listenfd, &fset)) {
      int connfd = accept(listenfd, (sockaddr *) &clientAddr, &calen);
      passert("error accept", connfd == -1);
      cout << "client connected: ";
      printAddress(clientAddr, cout);
      cout << endl;
      passert("too many clients", num_clientfds == maxClients);

      int ci = 0;
      while (clientfds[ci]) ++ci;
      clientfds[ci] = connfd;
      clientAddrs[ci] = clientAddr;
      num_clientfds++;
      if (connfd > maxfd) maxfd = connfd;
      FD_SET(connfd, &afset);
      // no client packets
      if (ret == 1) continue;
    }

    int deadclients = 0;
    for (int i = 0; i < num_clientfds; i++) {
      int connfd = clientfds[i];
      if (connfd && FD_ISSET(connfd, &fset)) {
        ret = read(connfd, buf, bufCap);
        passert("read error", ret == -1);
        if (ret > 0) {
          write(connfd, buf, ret);
        }
        else if (ret == 0) {
          close(connfd);
          FD_CLR(connfd, &afset);
          clientfds[i] = 0;
          cout << "client disconnected: ";
          printAddress(clientAddrs[i], cout);
          cout << endl;
        }
      }
    }
    num_clientfds -= deadclients;
  }
}

int main(int argc, const char *argv[])
{
  string usage = "usage: ";
  usage += argv[0];
  usage += " addr:port [select|poll]";
  if (argc < 2) {
    cout << usage << endl;
    return 1;
  }

  char multiplexer = 's';
  if (argc > 2) {
    if (!strcmp(argv[2], "poll")) {
      multiplexer = 'p';
    }
  }
  cout << "using multiplexer: " << multiplexer << endl;

  sockaddr_in serverAddr = {};
  int ret = getAddress(argv[1], serverAddr);
  assert("can't get server address" && ret);
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  passert("error socket", listenfd == -1);
  ret = bind(listenfd, (const sockaddr *)&serverAddr, sizeof(serverAddr));
  passert("error bind", ret == -1);
  ret = listen(listenfd, 5);
  passert("error listen", ret == -1);

  if (multiplexer == 's') {
    useSelect(listenfd);
  }
  else if (multiplexer == 'p') {
  }
}
