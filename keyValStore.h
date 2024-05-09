//
// Created by Erik on 09.05.2024.
//

#ifndef MEILENSTEIN_1_KEYVALSTORE_H
#define MEILENSTEIN_1_KEYVALSTORE_H

#define BUFFER_SIZE 1024

struct KeyValue {
    char key[256];
    char value[256];
};

extern struct KeyValue keyValueStore[100];
extern int numKeyValues;

int put(char* key, char* value);
int get(char* key, char* res);
int del(char* key);

#endif //MEILENSTEIN_1_KEYVALSTORE_H
