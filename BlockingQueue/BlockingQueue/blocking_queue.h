#ifndef __BLOCKING_QUEUE_H__
#define __BLOCKING_QUEUE_H__

#include <queue>
using namespace std;

#include "pthread.h"
#include <ctime>

#ifdef WIN32
#include "wintime.h"
#else 
#include <ctime>
#endif

template<typename T>
class blocking_queue
{
private:
	queue<T> *internal_queue;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
		
public:
	blocking_queue() : internal_queue(new queue<T>()) 
	{
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&cond, NULL);
	}
		
	/*
	 * Returns the current size of this queue
	 */
	size_t size()
	{
		pthread_mutex_lock(&mutex);
		size_t ret = internal_queue->size();
		pthread_mutex_unlock(&mutex);

		return ret;
	}

	/*
	 * Checks if this queue is empty
	 */
	bool empty() 
	{
		pthread_mutex_lock(&mutex);
		bool ret = internal_queue->empty();
		pthread_mutex_unlock(&mutex);

		return ret;
	}

	/*
	 * Add a new data to queue
	 */
	void push(T const & data)
	{
		pthread_mutex_lock(&mutex);
		internal_queue->push(data);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}

	/*
	 * Blocks until we have something in the queue 
	 * to be read.
	 */
	T pop()
	{
		pthread_mutex_lock(&mutex);
		while(internal_queue->empty())
			pthread_cond_wait(&cond, &mutex);

		T data = internal_queue->front();
		internal_queue->pop();
		pthread_mutex_unlock(&mutex);
		return data;
	}

	/*
	 * This funcitons will block for timeout ms
	 * or until the queue has samething to be read
	 */
	T pop(unsigned long timeout)
	{
		// Set up the timer based on the timeout
		struct timespec ts;
		struct timeval tv;

		// Convert time interval to timespec
		gettimeofday(&tv, NULL);
		ts.tv_sec  = tv.tv_sec;
        ts.tv_nsec = tv.tv_usec * 1000;

		// Add the timeout
		int sec_to_wait = timeout / 1000;
		int nsec_to_wait = (timeout % 1000) * 1000;
		ts.tv_nsec += nsec_to_wait;
        ts.tv_sec  += sec_to_wait;

		// Wait for timeout or the actual signal 
		pthread_mutex_lock(&mutex);
		while(internal_queue->empty())
			if(pthread_cond_timedwait(&cond, &mutex, &ts) == ETIMEDOUT)
				break;

		// Check to see if the queue is still empty
		// If not, pop a value, otherwhise return null
		// FIXME: returning NULL might not be a good idea!
		T data = NULL;

		if(!internal_queue->empty()) {
			data = internal_queue->front();
			internal_queue->pop();
		}
		pthread_mutex_unlock(&mutex);

		return data;
	}

	/*
	 * Destructor 
	 */
	~blocking_queue()
	{
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
		delete internal_queue;
	}
};

#endif