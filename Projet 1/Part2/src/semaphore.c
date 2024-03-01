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
long counter = 0;
int one = 1;


typedef struct semaphore{ 
	int val;
    test_and_test_and_set sem;
}semaphore;
semaphore *mutex;

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

void create_semaphore(semaphore *semaphore, int val){
	semaphore->val = val;
	semaphore->sem = 0;
}

void wait(semaphore *semaphore){ 
	if (semaphore->val <=0){
		while(semaphore->val <= 0);
	}
	testandtestandset_lock(&semaphore->sem);
	semaphore->val = semaphore->val - 1;
	testandtestandset_unlock(&semaphore->sem);
}

void post(semaphore *semaphore){ 
	testandtestandset_lock(&semaphore->sem);
	semaphore->val = semaphore->val + 1;
	testandtestandset_unlock(&semaphore->sem);
}


int increment(int i){
	return i+1;
}

void *func(void *param){
	while(counter<6400){	
		printf("moien %d \n", mutex->val);
		post(mutex);
		printf("alop %d \n", mutex->val);
		wait(mutex);
		printf("oops\n");
		counter = increment(counter);
		post(mutex);
		wait(mutex);
	}	
	return(NULL);
}

int main(int argc, char** argv){
	int nbr_threads = atoi(argv[1]);
	printf("%d\n", nbr_threads);
	pthread_t tid[nbr_threads];
	mutex =(semaphore *) malloc(sizeof(semaphore));
	printf("duck\n");
	create_semaphore(mutex, 0);
	printf("moien\n");
	for(int i = 0; i<nbr_threads; i++){
		pthread_create(&tid[i],NULL,&func,NULL);	
	}
	for(int i = 0; i<nbr_threads; i++){
		pthread_join(tid[i],NULL);
	}
	printf("%ld\n", counter);
	free(mutex);
	return 0;
}
