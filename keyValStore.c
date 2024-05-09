//
// Created by Erik on 09.05.2024.
//
#include "keyValStore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct KeyValue keyValueStore[100];
int numKeyValues = 0;

int put(char* key, char* value) {
    for (int i = 0; i < numKeyValues; i++) {
        if (strcmp(key, keyValueStore[i].key) == 0) {
            // Schlüssel bereits vorhanden, Wert überschreiben
            strcpy(keyValueStore[i].value, value);
            return 0;
        }
    }

    // Schlüssel nicht vorhanden, neues Schlüssel-Wert-Paar hinzufügen
    if (numKeyValues < 100) {
        strcpy(keyValueStore[numKeyValues].key, key);
        strcpy(keyValueStore[numKeyValues].value, value);
        numKeyValues++;
        return 0;
    }
    else {
        // Fehlermeldung zurückgeben, wenn das Array voll ist
        printf("PUT: name:%s value:array_full\n", key);
        return -1;
    }
}

int get(char* key, char* res) {
    for (int i = 0; i < numKeyValues; i++) {
        if (strcmp(key, keyValueStore[i].key) == 0) {
            // Schlüssel gefunden, Wert zurückgeben
            snprintf(res, BUFFER_SIZE, ">GET: name:%s value:%s", keyValueStore[i].key, keyValueStore[i].value);
            return 0;
        }
    }

    // Schlüssel nicht vorhanden, Fehler zurückgeben
    strcpy(res, ">GET:");
    strcat(res, key);
    strcat(res, ":key_nonexistent\n");
    return -1;
}

int del(char* key) {
    for (int i = 0; i < numKeyValues; i++) {
        if (strcmp(key, keyValueStore[i].key) == 0) {
            // Schlüssel gefunden, Schlüssel-Wert-Paar löschen
            for (int j = i; j < numKeyValues - 1; j++) {
                strcpy(keyValueStore[j].key, keyValueStore[j + 1].key);
                strcpy(keyValueStore[j].value, keyValueStore[j + 1].value);
            }
            numKeyValues--;
            return 0;
        }
    }

    // Schlüssel nicht vorhanden, Fehler zurückgeben
    return -1;
}