#include "keyValStore.h"
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

int shm_id;
struct KeyValue* keyValueStore;
int* numKeyValues;

void initialize_shared_memory() {
    shm_id = shmget(SHM_KEY, sizeof(struct KeyValue) * 100 + sizeof(int), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    void* shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("shmat failed");
        exit(1);
    }

    keyValueStore = (struct KeyValue*)shm_ptr;
    numKeyValues = (int*)(shm_ptr + sizeof(struct KeyValue) * 100);

    if (*numKeyValues == -1) {
        *numKeyValues = 0; // Initialisieren, falls das Segment neu erstellt wurde
    }
}

void detach_shared_memory() {
    if (shmdt(keyValueStore) == -1) {
        perror("shmdt failed");
    }
}

void cleanup_shared_memory() {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
    }
}

int put(char* key, char* value) {
    for (int i = 0; i < *numKeyValues; i++) {
        if (strcmp(key, keyValueStore[i].key) == 0) {
            strcpy(keyValueStore[i].value, value);
            return 0;
        }
    }

    if (*numKeyValues < 100) {
        strcpy(keyValueStore[*numKeyValues].key, key);
        strcpy(keyValueStore[*numKeyValues].value, value);
        (*numKeyValues)++;
        return 0;
    } else {
        printf(">PUT: name:%s value:array_full\n", key);
        return -1;
    }
}

int get(char* key, char* res) {
    for (int i = 0; i < *numKeyValues; i++) {
        if (strcmp(key, keyValueStore[i].key) == 0) {
            snprintf(res, BUFFER_SIZE, ">GET: name:%s value:%s", keyValueStore[i].key, keyValueStore[i].value);
            return 0;
        }
    }

    strcpy(res, ">GET:");
    strcat(res, key);
    strcat(res, ":key_nonexistent\n");
    return -1;
}

int del(char* key) {
    for (int i = 0; i < *numKeyValues; i++) {
        if (strcmp(key, keyValueStore[i].key) == 0) {
            for (int j = i; j < *numKeyValues - 1; j++) {
                strcpy(keyValueStore[j].key, keyValueStore[j + 1].key);
                strcpy(keyValueStore[j].value, keyValueStore[j + 1].value);
            }
            (*numKeyValues)--;
            return 0;
        }
    }
    return -1;
}
