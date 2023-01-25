#include "server_hangman_functions.h"

/* Send set roles to both clients */
/* Client connected first is the MASTER and second is GUESSER */
void print_role(struct Player* player)
{
    char write_buff[MAXLINE];
    if (player->player_role == MASTER)
    {
        snprintf(write_buff, sizeof(write_buff), "%d - Your role is: MASTER\n", player->player_role);
    }
    else if (player->player_role == GUESSER)
    {
        snprintf(write_buff, sizeof(write_buff), "%d - Your role is: GUESSER\n", player->player_role);
    }

    if (write(player->sock_fd, write_buff, strlen(write_buff)) < 0)
    {
        fprintf(stderr,"write error : %s\n", strerror(errno));
        return;
    }
    
}

/* Receive clue sent by MASTER - alias for simple read */
void get_clue(struct Player* player, char* clue_buff)
{
    
    if (read(player->sock_fd, clue_buff, CLUE_MAX_SIZE) < 0)
    {
        fprintf(stderr,"read error : %s\n", strerror(errno));
        return;
    }

    puts(clue_buff);
}

/* Check if client's guess is correct */
bool char_in_clue(char guess, char* clue_buff, char* guessed_clue_buff)
{
    bool val = false;

    for (int i = 0; i < CLUE_MAX_SIZE; i++)
    {
        if (clue_buff[i] == guess)
        {
            guessed_clue_buff[i] = guess;
            val = true;
        }
    }

    return val;
}

/* guessed_clue_buff is a buffer containing concealed clue */
/* Modify the buffer to have '_' as many as the real legth of the clue is (do not count NULLs) */
void init_guessed_buff(char* clue_buff, char* guessed_clue_buff)
{
    int clue_len = 0;
    for (int i = 0; i < CLUE_MAX_SIZE; i++)
    {
        if (clue_buff[i] != 0 && clue_buff[i] !='\n')
        {
            guessed_clue_buff[i] = '_';
        }
        else
        {
            guessed_clue_buff[i] = 0;
        }
    }
}

/* Check if whole clue is guessed */
bool check_if_clue_guessed(char* guessed_clue_buff)
{
    puts(guessed_clue_buff);
    for (int i = 0; i < CLUE_MAX_SIZE; i++)
    {
        if (guessed_clue_buff[i] == '_')
        {
            return false;
        }
    }
    return true;
}

bool get_guess(struct Player* player, char* clue_buff, char* guessed_clue_buff)
{
    char guess;
    char guess_buff[MAXLINE];
    char write_buff[MAXLINE];
    char message[200];
    char hangman_buff[HANGMAN_BUFF_SIZE];
    bzero(write_buff, MAXLINE);

    if (read(player->sock_fd, guess_buff, MAXLINE) < 0)
    {
        fprintf(stderr,"read error : %s\n", strerror(errno));
        return false;
    }

    guess = guess_buff[0];
    
    if (char_in_clue(guess, clue_buff, guessed_clue_buff))
    {
        snprintf(message, sizeof(message), "You have guessed correctly!\nThere are %d strikes left.\n", player->left_strikes);
        draw_hangman(player->left_strikes, hangman_buff);
        printf("Player has guessed correctly!\n");
        if (check_if_clue_guessed(guessed_clue_buff))
        {
            bzero(write_buff, MAXLINE);
            snprintf(message, sizeof(message), "Congratulations\n");
            strcat(write_buff, message);
            strcat(write_buff, guessed_clue_buff);
            strcat(write_buff, hangman_buff);

            if (write(player->sock_fd, write_buff, strlen(write_buff)) < 0)
            {
                fprintf(stderr,"write error : %s\n", strerror(errno));
                return false;
            }
            return false;
        }
        strcat(write_buff, message);
        strcat(write_buff, guessed_clue_buff);
        strcat(write_buff, hangman_buff);
        if (write(player->sock_fd, write_buff, strlen(write_buff)) < 0)
        {
            fprintf(stderr,"write error : %s\n", strerror(errno));
            return false;
        }
        fflush(stdout);

    }
    else
    {
        snprintf(message, sizeof(message), "Your guess is wrong!\nThere are %d strikes left.\n", --player->left_strikes);
        draw_hangman(player->left_strikes, hangman_buff);
        printf("Player has not guessed correctly!\n");
        if (player->left_strikes == 0)
        {
            bzero(write_buff, MAXLINE);
            snprintf(message, sizeof(message), "Game over\n");
            strcat(write_buff, message);
            strcat(write_buff, guessed_clue_buff);
            strcat(write_buff, hangman_buff);
            if (write(player->sock_fd, write_buff, strlen(write_buff)) < 0)
            {
                fprintf(stderr,"write error : %s\n", strerror(errno));
                return false;
            }
            return false;
        }
        strcat(write_buff, message);
        strcat(write_buff, guessed_clue_buff);
        strcat(write_buff, hangman_buff);

        if (write(player->sock_fd, write_buff, strlen(write_buff)) < 0)
        {
            fprintf(stderr,"write error : %s\n", strerror(errno));
            return false;
        }
        fflush(stdout);

    }
    return true;

}

void draw_hangman(int strike, char* hangman_buff)
{
    switch (strike)
    {
    case 5:
        snprintf(hangman_buff, HANGMAN_BUFF_SIZE, "\n  ____\n  |  |\n  |\n  |\n  |\n-----\n");
        break;
    
    case 4:
        snprintf(hangman_buff, HANGMAN_BUFF_SIZE, "\n  ____\n  |  |\n  |  O\n  |\n  |\n-----\n");
        break;

    case 3:
        snprintf(hangman_buff, HANGMAN_BUFF_SIZE, "\n  ____\n  |  |\n  |  O\n  | -|\n  |\n-----\n");
        break;
    case 2:
        snprintf(hangman_buff, HANGMAN_BUFF_SIZE, "\n  ____\n  |  |\n  |  O\n  | -|-\n  |\n-----\n");
        break;

    case 1:
        snprintf(hangman_buff, HANGMAN_BUFF_SIZE, "\n  ____\n  |  |\n  |  O\n  | -|-\n  | /\n-----\n");
        break;

    case 0:
        snprintf(hangman_buff, HANGMAN_BUFF_SIZE, "\n  ____\n  |  |\n  |  O\n  | -|-\n  | / \\ \n-----\n");
        break;

    default:
        break;
    }
}
