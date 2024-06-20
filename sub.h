#include "keyValStore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void handle_client(int clientSocket);

void sigchld_handler(int s);

void error(const char *msg);

#ifndef MEILENSTEIN_1_SUB_H
#define MEILENSTEIN_1_SUB_H

#endif //MEILENSTEIN_1_SUB_H
