#include "utils.h"
using namespace std;

static const char *usage = "usage: uclient serveraddr:port [c|u|n]\n"
"  c for connected udp socket\n"
"  u for unconnected which is default\n"
"  n is a number; send this many messages to server; uses unconnected";

void work(int serverfd, const sockaddr_in &serverAddr)
{
  const int bufLen = 50;
  char buf[bufLen] = {};
  sockaddr_in sourceAddr = {};
  socklen_t sourceAddrLen = sizeof(sourceAddr);

  while (fgets(buf, bufLen, stdin)) {
    int sendLen = strlen(buf);
    int ret = sendto(serverfd, buf, sendLen, 0,
        (const sockaddr *)&serverAddr, sizeof(serverAddr));
    passert("sending error", ret != -1);

    ret = recvfrom(serverfd, buf, bufLen, 0,
        (sockaddr *)&sourceAddr, &sourceAddrLen);
    passert("recv error", ret != -1);
    cout << "server: ";
    printAddress(sourceAddr, cout);
    printf(" sent [%s]\n", buf);
  }
}

void workc(int serverfd, const sockaddr_in &serverAddr)
{
  int ret = connect(serverfd, (const sockaddr *) &serverAddr, sizeof(serverAddr));
  passert("error connecting to server", ret != -1);

  const int bufLen = 50;
  char buf[bufLen] = {};
  sockaddr_in sourceAddr = {};

  while (fgets(buf, bufLen, stdin)) {
    int sendLen = strlen(buf);
    ret = write(serverfd, buf, sendLen);
    passert("write error", ret != -1);

    ret = read(serverfd, buf, bufLen);
    passert("read error", ret != -1);
    cout << "server: ";
    printAddress(sourceAddr, cout);
    printf(" sent [%s]\n", buf);
  }
}

void workn(int serverfd, const sockaddr_in &serverAddr, int iters)
{
  const int bufLen = 50;
  char buf[bufLen] = {};

  for (int i = 0; i < iters; i++) {
    int wrote = snprintf(buf, bufLen, "%d", i);
    int ret = sendto(serverfd, buf, wrote, 0,
        (const sockaddr *)&serverAddr, sizeof(serverAddr));
    passert("sending error", ret != -1);
  }
  printf("sent %d messages\n", iters);
}

int main(int argc, const char *argv[])
{
  if (argc < 2) {
    cout << usage << endl;
    return 1;
  }

  int iters = 0;
  char type = 'u';
  if (argc > 2) {
    if (argv[2][0] == 'c') type = 'c';
    else if (argv[2][0] == 'u') type = 'u';
    else {
      iters = atoi(argv[2]);
      if (iters > 0) type = 'n';
    }
  }

  sockaddr_in serverAddr = {};
  getAddress(argv[1], serverAddr);
  cout << "connecting to using " << type << " ";
  printAddress(serverAddr, cout);
  cout << endl;

  int serverfd = socket(AF_INET, SOCK_DGRAM, 0);
  passert("creating server socket", serverfd != -1);

  if (type == 'u') {
    work(serverfd, serverAddr);
  }
  else if (type == 'c') {
    workc(serverfd, serverAddr);
  }
  else if (type == 'n') {
    workn(serverfd, serverAddr, iters);
  }

  return 0;
}
