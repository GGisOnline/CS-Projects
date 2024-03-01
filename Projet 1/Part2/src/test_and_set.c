#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // todo rm
#include <stdbool.h>

long counter = 0;
typedef long long test_and_set;
test_and_set mutex;
int one = 1;

void testandset_lock(test_and_set *lock){
	asm volatile( "acces: movl %0, %%eax;"
		"xchgl %%eax, (%1);"
		"testl %%eax, %%eax;"
        "jnz acces"
		: "=&r"(one)
		: "r" (lock)
	);
}

void testandset_unlock(test_and_set *lock){
	*lock = 0;
}

void *enter(void* param){
    //unsigned long id = pthread_self();
    int thread_nbr = (int) param;
    int threadcount = 6400/thread_nbr;
    int local_counter = 0;
	while(counter<6400 && local_counter <= threadcount){
		testandset_lock(&mutex);
        //printf("lock \n", counter);
		counter++;		
        //printf("%ld \n", counter);
		while(rand() > RAND_MAX/10000);
        //printf("unlock \n", counter);
		testandset_unlock(&mutex);
        
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
