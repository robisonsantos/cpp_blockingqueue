#include "blocking_queue.h"

blocking_queue<T>::blocking_queue() {}

size_t blocking_queue<T>::size() 
{ 
	return internal_queue.size(); 
}

bool blocking_queue<T>::empty()
{
	return internal_queue.empty();
}

void blocking_queue<T>::push(T const & data)
{
	internal_queue.push(data);
}

T blocking_queue<T>::pop(unsigned int timeout=0)
{
	T data = internal_queue.front();
	internal_queue.pop()
	return data;
}

~blocking_queue<T>::blocking_queue() {}
