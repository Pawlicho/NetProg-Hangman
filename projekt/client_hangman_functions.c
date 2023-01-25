#include "client_hangman_functions.h"

/* Send clue to the server */
void send_clue(char* clue, int sock_fd)
{
    int write_size;
    char test[MAXLINE];
    if ((write_size = write(sock_fd, clue, strlen(clue))) < 0)
    {
        fprintf(stderr,"write error : %s\n", strerror(errno));
        return;
    }
}