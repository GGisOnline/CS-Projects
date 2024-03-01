#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sched.h>
#include <stdint.h>

typedef long long test_and_test_and_set;
test_and_test_and_set mutex;
long counter = 0;
int one = 1;

void testandtestandset_lock(test_and_test_and_set *lock){
	asm volatile( 
		"lock: testl $0, (%1);"
		"jnz lock;"
		"acces: movl %0, %%eax;"
		"xchgl %%eax, (%1);"
		"testl %%eax, %%eax;"
        
		"jnz acces;"			
		: "=&r"(one)
		: "r" (lock)
		: "%eax"
	);
}

void testandtestandset_unlock(test_and_test_and_set *lock){ 
	*lock = 0;
}

void *enter(void *param){
	unsigned long id = pthread_self();
    int thread_nbr = (int) param;
    int threadcount = 6400/thread_nbr;
    int local_counter = 0;
	while(counter<6400 && local_counter <= threadcount){
		testandtestandset_lock(&mutex);
        //printf("lock\n",id);
		counter++;		
        //printf("thread nuber%ld is counting on %i\n", id,counter);
		while(rand() > RAND_MAX/10000);
		//printf("unlock\n",id);
		testandtestandset_unlock(&mutex);        
	}	
	return NULL;
}

int main(int argc, char** argv){

    int err;
    long NbrCore = strtol(argv[1], NULL, 10);

    pthread_t thread_T[NbrCore];
    
    mutex = 0;

    for(int i = 0; i < NbrCore; ++i){
        err = pthread_create(&(thread_T[i]), NULL, enter,(void*) NbrCore);
    }

        for(int i = 0; i < NbrCore; ++i){
        err = pthread_join(thread_T[i], NULL);
    }

	return EXIT_SUCCESS;
}