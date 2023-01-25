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
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "common_defs.h"

void print_role(struct Player* player);

void get_clue(struct Player* player, char* clue_buff);

bool char_in_clue(char guess, char* clue_buff, char* guessed_clue_buff);

void init_guessed_buff(char* clue_buff, char* guessed_clue_buff);

bool check_if_clue_guessed(char* guessed_clue_buff);

bool get_guess(struct Player* player, char* clue_buff, char* guessed_clue_buff);

void draw_hangman(int strike, char* hangman_buff);
