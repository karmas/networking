#include "utils.h"

#include <iostream>
#include <cstring>
#include <cassert>
using namespace std;

void request(int fd)
{
  const int bufCap = 20;
  char buf[bufCap] = {};
  while (fgets(buf, bufCap, stdin)) {
    int wrote = strlen(buf);
    if (write(fd, buf, wrote) < 0) {
      perror("unable to write");
    }

    bzero(buf, bufCap);
    int got = 0;
    int gotTotal = 0;
    while ((got = read(fd, buf, wrote))) {
      if (got > 0) {
        gotTotal += got;
      }
      else if (got == -1) {
        if (errno == EINTR) {
          cout << "recover from interruption" << endl;
        }
        else {
          cout << "read error" << endl;
          break;
        }
      }
      if (gotTotal == wrote) break;
    }
    if (gotTotal == 0) {
      cout << "error: server died" << endl;
      return;
    }
    cout << "from server: [";
    cout << buf << "]" << endl;
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
