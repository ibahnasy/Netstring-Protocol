CXX = gcc
CFLAGS = -pedantic -Wall -Wextra -Werror -ggdb -O0

all: server client

server: netstring
	$(CXX) $(CFLAGS) -o server server.c netstring.o

client: netstring
	$(CXX) $(CFLAGS) -o client client.c netstring.o

netstring:
	$(CXX) $(CFLAGS) -c netstring.c

clean:
	- rm *.o
	- rm server client
