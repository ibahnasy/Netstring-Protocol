CXX = gcc
CFLAGS = -pedantic -Wall -Wextra -Werror -ggdb -O0

all: server client

server: netstring
	$(CXX) $(CFLAGS) -o server server.c netstring.o

client: netstring
	$(CXX) $(CFLAGS) -o client client.c netstring.o

netstring:
	$(CXX) $(CFLAGS) -c netstring.c

test: netstring
	$(CXX) $(CFLAGS) -o test_netstring test_netstring.c netstring.o
	- ./test_netstring

clean:
	- rm *.o
	- rm server client test_netstring
