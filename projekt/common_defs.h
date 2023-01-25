#pragma once

#define HANGMAN_PORT            (7)
#define MAXLINE                 (1024)
#define MAX_NUMBER_OF_PLAYERS   (2)
#define CLUE_MAX_SIZE           (25)
#define MASTER_PLAYER           (0)
#define GUESSER_PLAYER          (1)
#define MAX_STRIKES_NUMBER      (5)
#define HANGMAN_BUFF_SIZE       (200)

enum role
{
    MASTER,
    GUESSER
};

struct Player
{
    int player_id;
    int sock_fd;
    struct sockaddr_in6 sock_addr;
    enum role player_role;
    int left_strikes;
};