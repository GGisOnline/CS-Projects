#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


void error(int err, char *msg) {
  fprintf(stderr,"%s a retourné %d message d'erreur : %s\n",msg,err,strerror(errno));
  exit(EXIT_FAILURE);
}

void eat(int phil)
{
	//emulating without any delay
}

void think(int phil)
{
	//emulating without any delay
}

struct Thread_Info{
  int id;
  int core;
  pthread_mutex_t *fork;
};


void* philosopher (void* arg){

    struct Thread_Info *Info = arg;
    int id= Info->id;
    int core = Info->core;
    pthread_mutex_t *fork = Info->fork;

    int philosopher = id;
    int right_philosopher = (philosopher + 1) % core;
    int count = 0;

    while(true) {
    // philosophe pense
    think(id);

    if(count ==100000){
      return (NULL);
    }
    if(philosopher<right_philosopher) {
      pthread_mutex_lock(&fork[philosopher]);
      pthread_mutex_lock(&fork[right_philosopher]);
    }
    else {
      pthread_mutex_lock(&fork[right_philosopher]);
      pthread_mutex_lock(&fork[philosopher]);
    }
    //printf("   Count : %d   ", count);
    eat(id);
    
    count++;
    
    pthread_mutex_unlock(&fork[philosopher]);
    pthread_mutex_unlock(&fork[right_philosopher]);
  }
  return (NULL);
}


int main ( int argc, char *argv[])
{
    int err;
    long NbrCore = strtol(argv[1], NULL, 10);     //prend l'input du terminal
    srand(getpid());
    
    struct Thread_Info *info;
    pthread_mutex_t fork[NbrCore];
    pthread_t phil[NbrCore];
    int i;
    int which;
    int id[NbrCore];
    
    if (NbrCore == 1) NbrCore++;
    for (i = 0; i < NbrCore; i++){
        id[i]=i;
    }
    
    for (i = 0; i < NbrCore; i++) {
        err=pthread_mutex_init( &fork[i], NULL);
        if(err!=0)
        error(err,"pthread_mutex_init");
        }
    for (i = 0; i < NbrCore; i++) {
        info = malloc(sizeof(struct Thread_Info));
        info->id = i;
        info->fork = fork;
        info-> core = NbrCore;
        err=pthread_create(&phil[i], NULL, philosopher, info);
        if(err!=0)
        error(err,"pthread_create");
        }
    for (i = 0; i < NbrCore; i++) {
        pthread_join(phil[i], NULL);
        if(err!=0)
        error(err,"pthread_join");
        }
    
    for (i = 0; i < NbrCore; i++) {
        pthread_mutex_destroy(&fork[i]);
        if(err!=0)
        error(err,"pthread_mutex_destroy");
        }
        
    
    free(info);
    
    return (EXIT_SUCCESS);
}