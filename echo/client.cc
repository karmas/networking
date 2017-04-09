#include "utils.h"

#include <iostream>
#include <cstring>
#include <cassert>
using namespace std;

void request(int fd)
{
  const int bufCap = 20;
  char buf[bufCap] = {};
  fd_set fds;
  const int maxfds = fd + 1;
  int ret = 0;
  bool stdineof = false;

  while (true) {
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    if (!stdineof)
      FD_SET(fileno(stdin), &fds);
    ret = select(maxfds, &fds, 0, 0, 0);
    if (ret == -1) {
      if (errno == EINTR) {
        puts("select recover from interrupt");
        continue;
      }
      else {
        perror("error in select");
        break;
      }
    }

    // stdin
    if (!stdineof && FD_ISSET(fileno(stdin), &fds)) {
      char *s = fgets(buf, bufCap, stdin);
      if (!s) {
        FD_CLR(fileno(stdin), &fds);
        stdineof = true;
        shutdown(fd, SHUT_WR);
      }
      else {
        write(fd, buf, strlen(buf));
      }
    }
    if (FD_ISSET(fd, &fds)) {
      int got = read(fd, buf, bufCap);
      if (got == 0) {
        if (stdineof) {
          puts("server got shutdown");
        }
        else {
          puts("server died");
        }
        return;
      }
      else if (got == -1) {
        perror("error reading from socket");
        return;
      }
      buf[got] = 0;
      printf("from server [%s]\n", buf);
    }
  }
}

int main(int argc, const char *argv[])
{
  string usage = "usage: " + string(argv[0]) + " serverip:port [numconns]";
  usage += "\n  numconns is for testing multiple connections";

  if (argc < 2) {
    cout << usage << endl;
    return 1;
  }

  int numConns = 1;
  if (argc > 2) {
    numConns = atoi(argv[2]);
    if (numConns == 0) numConns = 1;
    cout << "will make " << numConns << " connections" << endl;
  }

  const char *serverSocket = argv[1];
  sockaddr_in serverAddress;
  bzero(&serverAddress, sizeof(serverAddress));
  assert("error getting address" && getAddress(serverSocket, serverAddress));

  cout << "connecting to: ";
  printAddress(serverAddress, cout);
  cout << endl;

  int serverfds[numConns];
  bzero(serverfds, sizeof(serverfds));
  for (int i = 0; i < numConns; ++i) {
    serverfds[i] = socket(AF_INET, SOCK_STREAM, 0);
    passert("error creating socket", serverfds[i] == -1);
    int ret = connect(serverfds[i], (const sockaddr *)&serverAddress,
        sizeof(serverAddress));
    passert("error connecting to server", ret == -1);
  }

  // only first connection interacts
  request(serverfds[0]);

  return 0;
}
