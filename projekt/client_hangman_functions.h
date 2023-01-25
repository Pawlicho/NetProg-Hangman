#pragma once

#include <stdio.h>
#include <unistd.h>	
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <stdlib.h>	
#include <string.h>
#include <errno.h>

#include "common_defs.h"

/* Send got from client message to the server */
void send_clue(char* clue, int sock_fd);