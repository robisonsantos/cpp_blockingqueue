#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include "blocking_queue.h"

blocking_queue<int> myqueue = blocking_queue<int>(-1);

void *add(void *t)
{
	int i = (intptr_t) t;
	int limit = i + 10;
	for(; i < limit; i++) {
		if(i % 2 == 0) sleep(2);

		myqueue.push(i);
	}

	pthread_exit(NULL);
	return NULL;
}

void *get(void *t)
{
	for(int i = 0; i < 20; i++)
		cout << "-- " << myqueue.pop(1000) << endl;
		//cout << "-- " << myqueue.pop() << endl;

	pthread_exit(NULL);
	return NULL;
}

int main()
{
	int t1 = 1, t2 = 25;
	pthread_t threads[3];
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0], &attr, add, (void *) (intptr_t)t1);
    pthread_create(&threads[1], &attr, add, (void *) (intptr_t)t2);
    pthread_create(&threads[2], &attr, get, NULL);

	 /* Wait for all threads to complete */
	for (int i=0; i<3; i++) {
		pthread_join(threads[i], NULL);
	}

	cout << "...FINISHED..." << endl;
	pthread_attr_destroy(&attr);
	pthread_exit(NULL);
	return 0;
}
