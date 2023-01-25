cc=gcc

all:server_hangman_functions.o client_hangman_functions.o server client 

server_hangman_functions.o: server_hangman_functions.c
		$(cc) -c server_hangman_functions.c

client_hangman_functions.o: client_hangman_functions.c
		$(cc) -c client_hangman_functions.c

server: server.c
		$(cc) -o server server.c server_hangman_functions.o

client: client.c
		$(cc) -o client client.c client_hangman_functions.o

clean:
		rm server client