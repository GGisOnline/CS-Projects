#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

sem_t rsem;
sem_t wsem;
pthread_mutex_t mutex_readcount;
pthread_mutex_t mutex_writecount;
pthread_mutex_t mutextotalread;
pthread_mutex_t mutextotalwrite;
pthread_mutex_t z;
//pthread_mutex_t mutex_total_read;
//pthread_mutex_t mutex_total_write;
int readcount = 0;
int writecount = 0;
int total_read = 0;
int total_write = 0;
int max_read = 2560;
int max_write = 640;

void reader(void *param){
    while(true){

        pthread_mutex_lock(&mutextotalread);
        total_read = total_read+1;
        if(total_read>=max_read){
            sem_post(&rsem);
            pthread_mutex_unlock(&mutextotalread);      
            return NULL;
        }   
        pthread_mutex_unlock(&mutextotalread);

        pthread_mutex_lock(&z);
        while(rand() > RAND_MAX/10000);
        sem_wait(&rsem);        
        pthread_mutex_lock(&mutex_readcount);        
        //exclusion mutuelle, readcount
        readcount = readcount + 1;
        if(readcount == 1){
            //first reader
            sem_wait(&wsem);            
        }
        pthread_mutex_unlock(&mutex_readcount);
        sem_post(&rsem);
        pthread_mutex_unlock(&z);

        
        pthread_mutex_lock(&mutex_readcount);
        //exclusion mutuelle, readcount
        readcount--;
        if (readcount == 0){
            //départ du dernier writer
            sem_post(&wsem);
        }
        pthread_mutex_unlock(&mutex_readcount);
    }
}

void writer(void *param){
    while(true){ 
        pthread_mutex_lock(&mutextotalwrite);
        pthread_mutex_lock(&mutex_writecount);
        //section critique - writecount
        total_write++;
        writecount = writecount +1;
        pthread_mutex_unlock(&mutex_writecount);
        if(writecount == 1){
            //first writer
            sem_wait(&rsem);
        }
        if(total_write>=max_write){
            sem_post(&rsem);
            pthread_mutex_unlock(&mutextotalwrite);
            return NULL;
        }
        pthread_mutex_unlock(&mutextotalwrite);
        sem_wait(&wsem);
        while(rand() > RAND_MAX/10000); //SIMULATE WRITE DATABASE
        sem_post(&wsem);
        pthread_mutex_lock(&mutex_writecount);
        //section critique - writecount
        writecount = writecount -1;
        if(writecount==0){  
            //départ du dernier writer
            sem_post(&rsem);
        }
        pthread_mutex_unlock(&mutex_writecount);
    }
}


int main(int argc, char *argv[]){
    
    long Nb_reader =strtol(argv[1], NULL, 10);
    long Nb_writer =strtol(argv[2], NULL, 10);

    pthread_mutex_init(&mutex_readcount, NULL);
    pthread_mutex_init(&mutex_writecount, NULL);
    pthread_mutex_init(&z, NULL);
    pthread_mutex_init(&mutextotalread, NULL);
    pthread_mutex_init(&mutextotalwrite, NULL);

    sem_init(&wsem,0,1);
    sem_init(&rsem,0,1);

    pthread_t thread_reader[Nb_reader];
    pthread_t thread_writer[Nb_writer];

    for (int i = 0; i < Nb_reader; i++){
        pthread_create(&thread_reader[i], NULL, reader, NULL);
    }

    for (int i = 0; i < Nb_writer; i++){
        pthread_create(&thread_writer[i], NULL, writer, NULL);
    }

    for (int i = 0; i < Nb_reader; i++){
        pthread_join(thread_reader[i], NULL);
    }

    for (int i = 0; i < Nb_writer; i++){
        pthread_join(thread_writer[i], NULL);
    }

    sem_destroy(&rsem);
    sem_destroy(&wsem);

    pthread_mutex_destroy(&mutex_writecount);
    pthread_mutex_destroy(&mutex_readcount);
    pthread_mutex_destroy(&z);
    pthread_mutex_destroy(&mutextotalwrite);
    pthread_mutex_destroy(&mutextotalread);

    return EXIT_SUCCESS;
}