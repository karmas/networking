#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

using namespace std;

static const char *usageString = "usage: app server_ip server_port";

int main(int argc, const char *argv[])
{
  if (argc < 3) {
    cout << usageString << endl;
    return 1;
  }

  in_addr serverAddr;
  bzero(&serverAddr, sizeof(serverAddr));
  if (!inet_aton(argv[1], &serverAddr)) {
    cout << "could not convert ip address" << endl;
    return 1;
  }
  short serverPort = atoi(argv[2]);

  int sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) {
    perror("creating socket");
    return 1;
  }

  sockaddr_in saddr;
  bzero(&saddr, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(serverPort);
  saddr.sin_addr.s_addr = htons(serverAddr.s_addr);
  if (connect(sd, (const sockaddr *)&saddr, sizeof(saddr)) < 0) {
    perror("connect fail");
    return 1;
  }

  cout << "connecting" << endl;

  return 0;
}
