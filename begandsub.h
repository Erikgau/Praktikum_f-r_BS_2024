
#ifndef MEILENSTEIN3FRFR_SUBSCRIP_H
#define MEILENSTEIN3FRFR_SUBSCRIP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MAX_SUBSCRIPTIONS 100
#define MAX_SUBSCRIPTION_LENGTH 50

typedef struct {
    char subscriptions[MAX_SUBSCRIPTIONS][MAX_SUBSCRIPTION_LENGTH];
    int count;
} SubscriptionArray;

SubscriptionArray * SubArray();

void addSubscription(SubscriptionArray * array, const char * subscription);

int isSubscribed(SubscriptionArray * array, const char * subscription);

int semaphore_create_group(int count);

int semaphore_set_value(int sem_group_id, int sem_id, int value);

int semaphore_off(int sem_group_id, int sem_id);

int semaphore_on(int sem_group_id, int sem_id);



#endif //MEILENSTEIN3FRFR_SUBSCRIP_H
