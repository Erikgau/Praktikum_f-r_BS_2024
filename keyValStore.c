#include <sys/shm.h>
#include <stdlib.h>
#include "keyValStore.h"

#define MAX_CLIENTS 5

void put_key(Storage* map, const char* key, const char* value) {
    // Check if the key already exists and update the value if found
    for (int i = 0; i < MAP_SIZE; i++) {
        if (strcmp(map->table[i].key, key) == 0) {
            // Key already exists, update the value
            int valueSize = sizeof(map->table[i].value) - 1;
            strncpy(map->table[i].value, value, valueSize);
            map->table[i].value[valueSize] = '\0';
            return;
        }
    }

    // Key not found, search for an empty slot
    for (int i = 0; i < MAP_SIZE; i++) {
        if (map->table[i].key[0] == '\0') {
            // Found an empty slot, insert the new entry
            int valueSize = sizeof(map->table[i].value) - 1;
            int keySize = sizeof(map->table[i].key) - 1;

            strncpy(map->table[i].key, key, keySize);
            map->table[i].key[keySize] = '\0';

            strncpy(map->table[i].value, value, valueSize);
            map->table[i].value[valueSize] = '\0';
            return;
        }
    }

    // Array is fully occupied
    printf("The storage is full, no further values can be saved.\n");
}

const char* get_key(Storage* map, const char* key) {
    // Check if the key exists
    for (int i = 0; i < MAP_SIZE; i++) {
        if (strcmp(map->table[i].key, key) == 0) {
            // Key found, return the value
            return map->table[i].value;
        }
    }
    // Key not found
    return NULL;
}

void del_key(Storage* map, const char* key) {
    // Check if the key exists
    for (int i = 0; i < MAP_SIZE; i++) {
        if (strcmp(map->table[i].key, key) == 0) {
            // Key found, delete the entry
            map->table[i].key[0] = '\0';
            map->table[i].value[0] = '\0';
            return;
        }
    }
}

Storage * SharedMemoryMap(){
    // Erstellen des Shared Memory
    int shmid = shmget(IPC_PRIVATE, sizeof(Storage), IPC_CREAT | 0600);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    // Anhängen des Shared Memory an den Prozess
    Storage * shar_mem_map = (Storage *)shmat(shmid, 0, 0);
    if (shar_mem_map == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    return shar_mem_map;
}

int messageQueueCreate(){
    return msgget(IPC_PRIVATE, IPC_CREAT | 0644);
}

int messageSendToAllPUT(int own_msg_q_id, int * msg_q_ids, char*key, char*value){

    // Nachricht vorbereiten
    struct msg_buffer message;
    message.msg_type = MSG_PUT_TYPE;
    snprintf(message.msg_text, MSG_SIZE, "%s:%s\n\r", key, value);

    // Nachricht senden
    int i = 0;
    while(i < MAX_CLIENTS && msg_q_ids[i]!=0){
        if(msg_q_ids[i] == own_msg_q_id){
            i++;
            continue;
        }
        printf("ID: %i", msg_q_ids[i]);
        int rc = msgsnd(msg_q_ids[i], &message, sizeof(message.msg_text), 0);
        if(rc < 0){
            perror("Fehler beim versenden einer PUT nachricht");
            exit(1);
        }
        printf("Send: %s", message.msg_text);
        i++;
    }

    return 0;
}

char* receiveMessageContent(int msg_q_id, SubscriptionArray * sub_list) {
    // Nachricht empfangen
    struct msg_buffer message;
    int rc = msgrcv(msg_q_id, &message, sizeof(message.msg_text), MSG_PUT_TYPE, IPC_NOWAIT);
    if (rc < 0) {
        return "";
    }

    // Speicher für den Nachrichteninhalt dynamisch zuweisen und kopieren
    char *content = (char *)malloc(strlen(message.msg_text) + 1);
    if (content == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    strcpy(content, message.msg_text);

    //Nachricht in Bestandteile splitten
    char *key, *value;
    splitMessage(content, &key, &value);

    if(isSubscribed(sub_list, key) == 0){
        return "";
    };

    // Rückgabemitteilung zusammenbauen
    char *return_message = (char *)malloc(strlen(key) + strlen(value) + 9); // 9 for "UPDATE "
    if (return_message == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    sprintf(return_message, ">SUB MESSAGE: %s Value %s", key, value);

    return return_message;
}

void splitMessage(const char *message, char **key, char **value) {
    // Find the position of the colon
    char *colon_pos = strchr(message, ':');
    if (colon_pos == NULL) {
        *key = NULL;
        *value = NULL;
        return;
    }

    // Calculate the length of the key and value
    int key_length = colon_pos - message;
    int value_length = strlen(colon_pos + 1);

    // Allocate memory for key and value
    *key = (char *)malloc((key_length + 1) * sizeof(char));
    *value = (char *)malloc((value_length + 1) * sizeof(char));

    if (*key == NULL || *value == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Copy key and value from the message
    strncpy(*key, message, key_length);
    (*key)[key_length] = '\0';
    strcpy(*value, colon_pos + 1);
}
