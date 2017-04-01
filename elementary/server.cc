#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

static const char *usageString = "usage: app server_ip server_port";

void print(const sockaddr_in &sin)
{
  in_addr temp;
  temp.s_addr = sin.sin_addr.s_addr;
  const char *ia = inet_ntoa(temp);
  unsigned short port = ntohs(sin.sin_port);
  cout << ia << ":" << port;
}

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
  //saddr.sin_addr.s_addr = INADDR_ANY;
  saddr.sin_addr.s_addr = serverAddr.s_addr;
  if (bind(sd, (const sockaddr *)&saddr, sizeof(saddr)) < 0) {
    perror("bind fail");
    return 1;
  }

  const int maxPendingConns = 10;
  if (listen(sd, maxPendingConns) < 0) {
    perror("listen fail");
    return 1;
  }

  while (true) {
    sockaddr_in clientAddr;
    bzero(&clientAddr, sizeof(clientAddr));
    socklen_t clen = sizeof(clientAddr);
    int cd = accept(sd, (sockaddr *)&clientAddr, &clen);
    if (cd < 0) {
      perror("accept fail");
      return 1;
    }

    cout << "client connected from ";
    print(clientAddr);
    cout << endl;

    time_t now = time(0);
    const char *s = ctime(&now);
    const int bufLen = 100;
    char buf[bufLen] = {};
    strncpy(buf, s, bufLen);
    if (write(cd, buf, bufLen) < 0) {
      perror("error writing:");
    }

    close(cd);
  }

  return 0;
}
