#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <pthread.h>
#include <deque>
#include <semaphore.h>

class ThreadSafeQueue
{
    public:
        ThreadSafeQueue();
        virtual ~ThreadSafeQueue();

        bool empty( ) const;
        int size( ) const;
        void* pop_front( );
        //void* back( );
        void push( void* x);
        //void pop( );
    protected:
    private:
        std::deque<void*> *mp_backend;
        pthread_mutex_t m_accessMutex;
        sem_t m_available;
};

#endif // THREADSAFEQUEUE_H
