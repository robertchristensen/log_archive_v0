#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <pthread.h>
#include <deque>
#include <semaphore.h>
/*
This Thread safe queue implementation is heavily based on the threaded
 buffer from the book "Computer Systems A Programmer's Perspective" by
 Bryant and O'Hallaron.  The queue is created with a specific capacity.
 If more then that number of elements are inserted, in order to insert
 another element the inserting thread must wait.
*/
class ThreadSafeQueue
{
    public:
        ThreadSafeQueue(int initial_capacity);
        virtual ~ThreadSafeQueue();

        // returns true if the queue is empty
        // this call does not use any locking.
        bool empty( );
        // return the number of elements in the queue
        int size( );
        // take the element from the front of the queue
        void* pop_front( );
        // put an element at the end of the queue
        void push( void* x);
    protected:
    private:
        std::deque<void*> *mp_backend;
        pthread_mutex_t m_accessMutex;
        sem_t m_available_slots;  // number of elements that can be inserted before being full
        sem_t m_available_items;  // number of elements that can be removed before being empty
};

#endif // THREADSAFEQUEUE_H
