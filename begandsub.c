#include "begandsub.h"

SubscriptionArray * SubArray() {
    // Erstellen des Shared Memory
    int shmid = shmget(IPC_PRIVATE, sizeof(SubscriptionArray), IPC_CREAT|0600);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    SubscriptionArray * sub_list = (SubscriptionArray *)shmat(shmid, 0, 0);
    if (sub_list == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    return sub_list;
}

void addSubscription(SubscriptionArray * array, const char * subscription) {
    if (array->count < MAX_SUBSCRIPTIONS) {
        strcpy(array->subscriptions[array->count], subscription);
        array->count++;
    } else {
        printf("Max subscriptions reached. Cannot add more.\n");
    }
}

int isSubscribed(SubscriptionArray * sub_list, const char * subscription) {
    for (int i = 0; i < sub_list->count; ++i) {
        if (strcmp(sub_list->subscriptions[i], subscription) == 0) {
            return 1; // Subscription found
        }
    }
    return 0; // Subscription not found
}

int semaphore_create_group(int count){
    int sem_group_id = semget (IPC_PRIVATE, count, IPC_CREAT|0644);
    return sem_group_id;
}

int semaphore_set_value(int sem_group_id, int sem_id, int value){
    struct sembuf values;
    values.sem_num = sem_id;
    values.sem_op = value;
    values.sem_flg = 0;
    int result = semop(sem_group_id, &values, 1);
    return result;
}

int semaphore_off(int sem_group_id, int sem_id){
    struct sembuf values;
    values.sem_num = sem_id;
    values.sem_flg = SEM_UNDO;
    values.sem_op = -1;
    int result = semop(sem_group_id, &values, 1);
    return result;
}

int semaphore_on(int sem_group_id, int sem_id){
    struct sembuf values;
    values.sem_num = sem_id;
    values.sem_flg = SEM_UNDO;
    values.sem_op = 1;
    int result = semop(sem_group_id, &values, 1);
    return result;
}
