CC = g++
CPPFLAGS = -Wall -g -std=c++11

apps = server client mserver userver uclient

all: $(apps)

server.o client.o mserver.o userver.o uclient.o: utils.h

server: server.o

client: client.o

mserver: mserver.o

userver: userver.o

uclient: uclient.o

clean:
	rm -f $(apps) *.o
