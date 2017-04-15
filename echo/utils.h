#ifndef utils_h
#define utils_h

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <unistd.h>
#include <signal.h>

void passert(const char *msg, bool cond)
{
  if (cond) return;
  perror(msg);
  exit(1);
}

// @param s dotted decimal address and port e.g. 127.0.0.1:10
// @param addr fill it with ip address
// @return true if success else false
bool getAddress(const char *s, sockaddr_in &addr)
{
  const char *sep = strchr(s, ':');
  if (!sep) return false;

  int hostCap = sep - s + 1;
  char host[hostCap];
  bzero(host, hostCap);
  strncpy(host, s, hostCap - 1);

  in_addr hostBin;
  bzero(&hostBin, sizeof(hostBin));
  if (!inet_aton(host, &hostBin)) {
    return false;
  }
  unsigned short port = atoi(sep + 1);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = hostBin;
  return true;
}

void printAddress(sockaddr_in &addr, std::ostream &out)
{
  const char *host = inet_ntoa(addr.sin_addr);
  unsigned short port = ntohs(addr.sin_port);
  out << host << ":" << port;
}

typedef void Sigfunc(int);

// return old signal handler
Sigfunc *signal(int signo, Sigfunc *func)
{
  struct sigaction sa = {};
  sa.sa_handler = func;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
#ifdef SA_RESTART
  // don't restart alarm since it is usually used to timeout system calls
  if (signo != SIGALRM) {
    //sa.sa_flags |= SA_RESTART;
  }
#endif
  struct sigaction oldsa = {};
  if (sigaction(signo, &sa, &oldsa) == -1) {
    return SIG_ERR;
  }
  return oldsa.sa_handler;
}

#endif
