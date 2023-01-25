#pragma once

#include <stdio.h>
#include <unistd.h>	
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <stdlib.h>	
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "common_defs.h"

/* Send set roles to both clients */
/* Client connected first is the MASTER and second is GUESSER */
void print_role(struct Player* player);

/* Receive clue sent by MASTER - alias for simple read */
void get_clue(struct Player* player, char* clue_buff);

/* Check if client's guess is correct */
bool char_in_clue(char guess, char* clue_buff, char* guessed_clue_buff);

/* guessed_clue_buff is a buffer containing concealed clue */
/* Modify the buffer to have '_' as many as the real legth of the clue is (does not count NULLs) */
void init_guessed_buff(char* clue_buff, char* guessed_clue_buff);

/* Check if whole clue is guessed */
bool check_if_clue_guessed(char* guessed_clue_buff);

/* Handle player's guess: receive message, send feedback */
/* Return false if game is over */
bool get_guess(struct Player* player, char* clue_buff, char* guessed_clue_buff);

/* Fill the buffer with hangman pattern corresponding to current strike */
void draw_hangman(int strike, char* hangman_buff);
