gcc -pedantic -Wall -Wextra -ggdb -Werror netstring.c client.c -o bin/client
gcc -pedantic -Wall -Wextra -ggdb -Werror netstring.c server.c -o bin/server

* To run server: ./server
* To run client: ./client 1100
