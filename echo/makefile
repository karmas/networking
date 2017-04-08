CC = g++
CPPFLAGS = -Wall -g -std=c++11

apps = server client

all: server client

server.o client.o: utils.h

server: server.o

client: client.o

clean:
	rm -f $(apps) *.o
