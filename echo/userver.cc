#include "utils.h"
using namespace std;

static const char *usage = "usage: userver serveraddr:port";

void work(int serverfd, const sockaddr_in &serverAddr)
{
  const int bufLen = 50;
  char buf[bufLen] = {};
  sockaddr_in sourceAddr = {};
  socklen_t sourceAddrLen = sizeof(sourceAddr);

  while (true) {
    int ret = recvfrom(serverfd, buf, bufLen, 0,
        (sockaddr *)&sourceAddr, &sourceAddrLen);
    passert("recv error", ret != -1);
    buf[ret] = 0;
    printf("'%s' from ", buf);
    printAddress(sourceAddr, cout);
    cout << endl;
    ret = sendto(serverfd, buf, ret, 0,
        (sockaddr *)&sourceAddr, sourceAddrLen);
    passert("sending problem", ret != -1);
  }
}

int main(int argc, const char *argv[])
{
  if (argc < 2) {
    cout << usage << endl;
    return 1;
  }

  sockaddr_in serverAddr = {};
  getAddress(argv[1], serverAddr);
  cout << "server at: ";
  printAddress(serverAddr, cout);
  cout << endl;

  int serverfd = socket(AF_INET, SOCK_DGRAM, 0);
  passert("creating server socket", serverfd != -1);
  int ret = bind(serverfd, (sockaddr *)&serverAddr, sizeof(serverAddr));
  passert("binding server socket", ret != -1);

  work(serverfd, serverAddr);

  return 0;
}
