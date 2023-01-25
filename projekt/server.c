#include <stdio.h>
#include <unistd.h>	
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <stdlib.h>	
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "common_defs.h"
#include "server_hangman_functions.h"

#define LISTENQ                 (2)

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

int main(int argc, char* argv[])
{
    int listen_fd;
    struct sockaddr_in6 serv_addr;
    struct Player players[MAX_NUMBER_OF_PLAYERS];
    char read_buff[MAXLINE];
    char write_buff[MAXLINE];
    char clue_buff[CLUE_MAX_SIZE] = {};
    char guessed_clue_buff[CLUE_MAX_SIZE] = {};
    pid_t childpid;

    int client_ctr = 0;

    signal(SIGCHLD, sig_chld);

    /* Creating listening socket */
    if ((listen_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr,"socket error : %s\n", strerror(errno));
        return 1;
    }

    /* Set SO_REUSEADDR sockopt */
    int optval = 1;               
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
        fprintf(stderr,"SO_REUSEADDR setsockopt error : %s\n", strerror(errno));
    }

    /* filling SA structure */
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_addr = in6addr_any;
    serv_addr.sin6_port = htons(HANGMAN_PORT);

    /* Bind */
    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr,"bind error : %s\n", strerror(errno));
        return 1;
    }

    /* Listen */
    if (listen(listen_fd, LISTENQ) < 0)
    {
        fprintf(stderr,"listen error : %s\n", strerror(errno));
        return 1;
    }

    /* Client Handling */
    while (1)
    {
        socklen_t clilen = sizeof(players[client_ctr].sock_addr);
        if ((players[client_ctr].sock_fd = accept(listen_fd, (struct sockaddr *)&players[client_ctr].sock_addr, &clilen)) < 0)
        {
			if (errno == EINTR)
				continue;		/* back to while() */
			else
				perror("accept error");
				exit(1);
		}
        else
        {
            players[client_ctr].player_id = client_ctr;
            players[client_ctr].player_role = client_ctr == 0 ? MASTER : GUESSER;
            players[client_ctr].left_strikes = MAX_STRIKES_NUMBER;
        }
            if (client_ctr == 1)
            {
                if ((childpid = fork()) == 0)
                {
                    close(listen_fd);

                    /* Send role to clients */
                    print_role(&players[MASTER_PLAYER]);
                    print_role(&players[GUESSER_PLAYER]);

                    /* Get sent clue from player MASTER */
                    get_clue(&players[MASTER_PLAYER], clue_buff);

                    init_guessed_buff(clue_buff, guessed_clue_buff);

                    snprintf(write_buff, MAXLINE, "The Master has chosen a clue. You may now proceed to guessing letters\n\n");

                    if (write(players[GUESSER_PLAYER].sock_fd, write_buff, strlen(write_buff)) <0)
                    {
                        fprintf(stderr,"write error : %s\n", strerror(errno));
                        return 1;
                    }

                    while (1)
                    {
                        bool game_is_on = get_guess(&players[GUESSER_PLAYER], clue_buff, guessed_clue_buff);
                        if (game_is_on)
                        {
                            continue;
                        }
                        else
                        {
                            if (players[GUESSER_PLAYER].left_strikes == 0)
                            {
                                snprintf(write_buff, MAXLINE, "Game over! Guesser has lost");
                                puts(write_buff);
                            }
                            else
                            {
                                snprintf(write_buff, MAXLINE, "Game over! Guesser has won");
                                puts(write_buff);

                            }
                            if (write(players[MASTER_PLAYER].sock_fd, write_buff, strlen(write_buff)) < 0)
                            {
                                fprintf(stderr,"write error : %s\n", strerror(errno));
                                return 1;                                
                            }
                            break;
                        }
                    }
                }
                break;
            }

        client_ctr++;
    }
    exit(0);
    return 0;
}