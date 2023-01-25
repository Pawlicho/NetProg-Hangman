#include <stdio.h>
#include <unistd.h>	
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <stdlib.h>	
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "common_defs.h"
#include "client_hangman_functions.h"

#define START_OF_WIN_MESSAGE    ('C')
#define START_OF_LOSE_MESSAGE    ('G')

int main(int argc, char* argv[])
{

    int sock_fd;
    struct sockaddr_in6 servaddr;
    char buff[MAXLINE];
    enum role player_role;
    char clue[CLUE_MAX_SIZE];

    /* Handle program missusage */
    if (argc != 2)
    {
		fprintf(stderr, "usage: %s <IPv6_address> : \n", argv[0]);
		return 1;
	}

    /* Create stream socket */
    if ( ( sock_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0 )
    {
        fprintf(stderr,"socket error : %s\n", strerror(errno));
    }

    bzero(&servaddr, sizeof(servaddr));
    /* Fill SA structure */
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(HANGMAN_PORT);

    /* Convert IP address format and fill into SA */
    if (inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr) <= 0)
    {
        fprintf(stderr,"inet_pton error for %s : \n", argv[1]);
        return 1;
    }

    /* Connect to the server */
    if (connect(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        fprintf(stderr,"connect error : %s \n", strerror(errno));
		return 1;
    }

    /* Get player's role from server */
    if (read(sock_fd, buff, MAXLINE) < 0)
    {
        fprintf(stderr,"read error : %s\n", strerror(errno));
        return 1;
    }

	puts(buff);
    player_role = buff[0] == '0' ? MASTER : GUESSER;
    assert(player_role == 0 || player_role == 1);

    /* Choose client action based on the rol*/
    if (player_role == MASTER)
    {
        /* Enter clue and sent over to server */
        puts("Choose clue for another player to guess it: ");
        fgets(clue, CLUE_MAX_SIZE, stdin);
        send_clue(clue, sock_fd);

        /* Wait and read the game-over message */
        if (read(sock_fd, buff, MAXLINE) < 0)
        {
            fprintf(stderr,"read error : %s\n", strerror(errno));
            return 1;
        }    
        puts(buff);
    }
    else if (player_role == GUESSER)
    {
        /* Get message informing that player can start sending guesses */
        if (read(sock_fd, buff, MAXLINE) < 0)
        {
            fprintf(stderr,"read error : %s\n", strerror(errno));
            return 1;
        }

        puts(buff);

        /* Send char and receive information about correctness of the guess alternately */
        while (1)
        {
            /* Enter char and send to server */
            fgets(buff, MAXLINE, stdin);
            if (write(sock_fd, buff, strlen(buff)) < 0)
            {
                fprintf(stderr,"write error : %s\n", strerror(errno));
                return 1;
            }        

            bzero(buff, MAXLINE);
            /* Receive confirmation from server */
            if (read(sock_fd, buff, MAXLINE) < 0)
            {
                fprintf(stderr,"read error : %s\n", strerror(errno));
                return 1;
            }
            puts(buff);

            /* Game over - no need for further message exchange */
            if (buff[0] == START_OF_WIN_MESSAGE)
            {
                break;
            }
            if (buff[0] == START_OF_LOSE_MESSAGE)
            {
                break;
            }

        }
    }

	exit(0);
    return 0;
}