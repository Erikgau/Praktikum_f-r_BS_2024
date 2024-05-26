#ifndef MEILENSTEIN_1_KEYVALSTORE_H
#define MEILENSTEIN_1_KEYVALSTORE_H

#define BUFFER_SIZE 1024
#define SHM_KEY 0x1234 // Shared memory key

struct KeyValue {
    char key[256];
    char value[256];
};

extern int shm_id;
extern struct KeyValue* keyValueStore;
extern int* numKeyValues;

int put(char* key, char* value);
int get(char* key, char* res);
int del(char* key);

void initialize_shared_memory();
void detach_shared_memory();
void cleanup_shared_memory();

#endif //MEILENSTEIN_1_KEYVALSTORE_H
