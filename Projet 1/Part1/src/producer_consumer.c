#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 8
int BUFFER[BUFFER_SIZE];

pthread_mutex_t mutex;
sem_t empty;
sem_t full;

int to_produce = 0;
int counter_consumer = 0;

void printBuffer(){
    printf("BUFFER = \t");
    for(int i = 0; i < BUFFER_SIZE; ++i){
        printf("%d\t", BUFFER[i]);
    }
    printf("\n");
}

void error(int err, char *msg) {
    fprintf(stderr,"%s a retourné %d, message d'erreur : %s\n",msg,err,strerror(errno));
    exit(EXIT_FAILURE);
}

int produce(int item){                  // Simulate CPU
    item = rand()%10;
    for (int i=0; i<10000; i++);
    return item;
}

int remove_item(){
    int item = -1;
    for(int i = 0; i<BUFFER_SIZE;i++){
        if(BUFFER[i]!=0){
            item= BUFFER[i];
            BUFFER[i] = 0;
            break;
            }
        }
    return item;
}

int insert_item(){
    int item = -1;
    for (int i=0; i < BUFFER_SIZE;i++){
        if(BUFFER[i] == 0){
            item = i+1;
            BUFFER[i] = item;
            break;
        }
    }    
    return item;
}

void* consumer(long *args){ 
    int item;
    while(true){
        sem_wait(&full); // attente d'une place remplie
        pthread_mutex_lock(&mutex);
        if (counter_consumer <= 8192){
            produce(item);
            remove_item();
            //printBuffer();
            counter_consumer++;
        }
        else{         
            pthread_mutex_unlock(&mutex);            
            for (int i =0; i<args;i++){
                sem_post(&empty);
            }
            return NULL;                        
        }      
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }    
    return NULL;
}

void* producer(long* args){
    int item;
    while(true){
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        if (to_produce <= 8192){
            item = produce(item);
            insert_item();
            to_produce++;
            //printBuffer();
        }
        else{
            pthread_mutex_unlock(&mutex);
            for (int i =0; i<args;i++){
                sem_post(&full);
            }
            return NULL;
        }    
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    return NULL;
}

int main(int argc, char *argv[]){

    int err_semaphore;
    int err_mutex;
    int err_consumer;
    int err_producer;

    long Nb_Producer = strtol(argv[1], NULL, 10);
    long Nb_Consumer = strtol(argv[2], NULL, 10);

    // SEMAPHORE INIT
    err_semaphore = sem_init(&empty, 0, BUFFER_SIZE);
    if(err_semaphore!=0) error(err_semaphore,"sem_init");

    err_semaphore = sem_init(&full, 0, 0);
    if(err_semaphore!=0) error(err_semaphore,"sem_init");

    //MUTEX INIT
    err_mutex = pthread_mutex_init(&mutex, NULL);
    if(err_mutex!=0) error(err_mutex,"sem_init");

    //THREADS CREATION
    pthread_t threads_producer[Nb_Producer];
    pthread_t threads_consumer[Nb_Consumer];
    
    for (int i = 0; i < Nb_Consumer; i++){
        threads_consumer[i] = i;
        err_consumer = pthread_create(&(threads_consumer[i]), NULL, consumer, Nb_Consumer); 
        if(err_consumer!=0) error(err_consumer,"pthread_create_consumer");
    }
    
    for (int i = 0; i < Nb_Producer; i++){
        threads_producer[i] = i;
        err_producer = pthread_create(&(threads_producer[i]), NULL, producer, Nb_Producer); 
        if(err_producer!=0) error(err_producer,"pthread_create_producer");
    }

    //THREADS JOIN
    for(int i=0; i<Nb_Consumer; i++){
        err_consumer = pthread_join(threads_consumer[i],NULL);
        if(err_consumer != 0) error(err_consumer, "thread_join_consumer");
    }

    for(int i=0; i<Nb_Producer; i++){
        err_producer = pthread_join(threads_producer[i],NULL);
        if(err_producer != 0) error(err_producer, "thread_join_producer");
    }

    //DESTROY MUTEX AND SEMAPHORE

    err_consumer = sem_destroy(&empty);
    if(err_consumer != 0) error(err_consumer, "sem_destroy_empty");
    err_producer = sem_destroy(&full);
    if(err_producer != 0) error(err_producer, "sem_destroy_full");

    err_mutex = pthread_mutex_destroy(&mutex);
    if(err_mutex != 0) error(err_mutex, "destroy_mutex");

    return EXIT_SUCCESS;
}