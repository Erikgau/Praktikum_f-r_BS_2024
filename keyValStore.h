#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "begandsub.h"

#ifndef MEILENSTEIN3FRFR_KEYVALSTORE_H
#define MEILENSTEIN3FRFR_KEYVALSTORE_H

#define MAP_SIZE 100
#define KEY_SIZE 128
#define VALUE_SIZE 128
#define MSG_PUT_TYPE 11
#define MSG_SIZE 128

struct KeyValuePair {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
};

typedef struct {
    struct KeyValuePair table[MAP_SIZE];
} Storage;

Storage * SharedMemoryMap();

void put_key(Storage* map, const char* key, const char* value);

const char* get_key(Storage* map, const char* key);

void del_key(Storage* map, const char* key);

// Struktur für die Nachricht
struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int messageQueueCreate();

int messageSendToAllPUT(int own_msg_q_id, int * msg_q_ids, char*key, char*value);

char* receiveMessageContent(int msg_q_id, SubscriptionArray * sub_list);

void splitMessage(const char *message, char **key, char **value);

#endif //MEILENSTEIN3FRFR_KEYVALSTORE_H
