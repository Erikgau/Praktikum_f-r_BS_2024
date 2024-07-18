
#ifndef MEILENSTEIN3FRFR_SUB_H
#define MEILENSTEIN3FRFR_SUB_H

#include "keyValStore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <ctype.h>
#include <stdbool.h>
#include "begandsub.h"

char* handle_command(Storage *map, const char* command, int sem_group_id, bool* inTransaction, int msg_q_id, int * msg_q_ids, SubscriptionArray * sub_list);

char** splitByWhiteSpace(const char *longArray, int* numSubarrays);

void handle_client(int client_socket, Storage *map, int sem_group_id, bool* inTransaction, int msg_q_id, int * msg_q_ids);



#endif //MEILENSTEIN3FRFR_SUB_H
