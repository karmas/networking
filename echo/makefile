CC = g++
CPPFLAGS = -Wall -g -std=c++11

apps = server client mserver

all: $(apps)

server.o client.o mserver.o: utils.h

server: server.o

client: client.o

mserver: mserver.o

clean:
	rm -f $(apps) *.o
