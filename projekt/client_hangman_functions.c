#include "client_hangman_functions.h"

/* Send clue to the server */
void send_clue(char* clue, int sock_fd)
{
    char test[MAXLINE];
    if (write(sock_fd, clue, strlen(clue)) < 0)
    {
        fprintf(stderr,"write error : %s\n", strerror(errno));
        return;
    }
}