// Copyright 2017 Pok On Cheng
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/***************************
  * Pok On Cheng (pocheng) *
  * 74157306               *
  * CompSci 131 Lab 1 A    *
  ***************************/
  
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <pthread.h>
#include <time.h>


/*Global variables */
int num_threads; 
int count;
pthread_mutex_t *mutexes;

/* For representing the status of each philosopher */
typedef enum{
	none,   // No forks
	one,    // One fork
	two     // Both forks to consume
} utensil;

/* Representation of a philosopher */
typedef struct phil_data{
	int phil_num;
	int course;
	utensil forks; 
}phil_data;

int *numList;
phil_data *philosophers;

/* ****************Change function below ***************** */
void *eat_meal(void *ptr){
/* 3 course meal: Each need to acquire both forks 3 times.
 *  First try for fork in front.
 * Then for the one on the right, if not fetched, put the first one back.
 * If both acquired, eat one course.
 */
    phil_data *phil_ptr = ptr;
	int num = phil_ptr->phil_num;
	int hasBothForks;
    int i;
	for (i = 0; i < 3; i++) {
		hasBothForks = 0;
		while(!hasBothForks) {
			if (num > 0) {
				// check the front fork
				if (philosophers[num-1].forks != 2) {
					pthread_mutex_lock(&mutexes[num]);
					philosophers[num].forks = 1;
				}
			}
			else {
				if (philosophers[num_threads-1].forks != 2) {
					pthread_mutex_lock(&mutexes[num]);
					philosophers[num].forks = 1;
				}
			}
			// check the right forks
			if (philosophers[(num+1)%num_threads].forks == 0) {
				pthread_mutex_lock(&mutexes[(num+1)%num_threads]);
				philosophers[num].forks = 2;
				hasBothForks = 1;
			}
			else {
				pthread_mutex_unlock(&mutexes[num]);
				philosophers[num].forks = 0;
			}
		}
		// start eating
		#ifdef _WIN32
		Sleep(1);
		#else
		usleep(1);
		#endif
		philosophers[num].course += 1;
		printf("Philosopher %d has just eaten the course %d meal.\n", num, philosophers[num].course);
		// release both forks
		pthread_mutex_unlock(&mutexes[num]);
		pthread_mutex_unlock(&mutexes[(num+1)%num_threads]);
		philosophers[num].forks = 0;
		
    }
	return 0;
}

/* ****************Add the support for pthreads in function below ***************** */
int main( int argc, char **argv ){
	int num_philosophers, error;
	if (argc < 2) {
          fprintf(stderr, "Format: %s <Number of philosophers>\n", argv[0]);
          return 0;
     }
    num_philosophers = num_threads = atoi(argv[1]);
	pthread_t threads[num_threads];
	philosophers = malloc(sizeof(phil_data)*num_philosophers);
	mutexes = malloc(sizeof(pthread_mutex_t)*num_philosophers); //Each mutex element represent a fork
	
	/* Initialize structs */
	int i;
	for( i = 0; i < num_philosophers; i++ ){
		philosophers[i].phil_num = i;
		philosophers[i].course   = 0;
		philosophers[i].forks    = none;
	}
	
/* Each thread will represent a philosopher */

/* Initialize Mutex, Create threads, Join threads and Destroy mutex */

	error = 0;
	int isInitialized = 0;
	
    /* Initialize Mutex */
	if (error == 0) {
		int i;
		for (i = 0; i < num_threads; i++) {
			if ((error = pthread_mutex_init(&mutexes[i], NULL)) != 0) {
				printf("Failing to initialize mutex!\n");
			}
		}
		isInitialized = 1;
	}
	
	/* Create threads */
	if (error == 0) {
		int i;
		for (i = 0; i < num_threads; i++) {
			if ((error = pthread_create(&threads[i], NULL, (void *)eat_meal, (void *)(&philosophers[i]))) !=0) {
				printf("Failing to create thread!\n");
			}
		}
	}

    /* Join threads */
	if (error == 0) {
		int i;
		for (i = 0; i < num_threads; i++) {
			if ((error = pthread_join(threads[i], NULL)) != 0) {
			printf("Failing to join threads!\n");
			}
		}
	}
	
	/* Destroy mutex */
	if (isInitialized == 1) {
		error = pthread_mutex_destroy(mutexes);
	}
	
	/* Free Malloc from heap */
	free(philosophers);
	free(mutexes);
	return 0;
}
