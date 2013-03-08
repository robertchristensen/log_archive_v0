#include "../include/ThreadSafeQueue.h"

#include <pthread.h>
#include <deque>

using namespace std;

ThreadSafeQueue::ThreadSafeQueue(int initial_capacity)
{
    mp_backend = new deque<void*>( );
    pthread_mutex_init(&m_accessMutex, NULL);
    sem_init(&m_available_slots, 0, initial_capacity);
    sem_init(&m_available_items, 0, 0);
}

ThreadSafeQueue::~ThreadSafeQueue()
{
    pthread_mutex_destroy(&m_accessMutex);
    delete mp_backend;
}

bool ThreadSafeQueue::empty( )
{
    pthread_mutex_lock( &m_accessMutex );
    bool toRet = mp_backend->empty( );
    pthread_mutex_unlock( &m_accessMutex );

    return toRet;
}

int ThreadSafeQueue::size( )
{
    pthread_mutex_lock( &m_accessMutex );
    int toRet = mp_backend->size( );
    pthread_mutex_unlock( &m_accessMutex );

    return toRet;
}

void* ThreadSafeQueue::pop_front( )
{
    void* to_ret;

    sem_wait(&m_available_items);    // wait for an available item
    pthread_mutex_lock( &m_accessMutex ); // lock the data buffer

    to_ret = mp_backend->front( );
    mp_backend->pop_front( );

    pthread_mutex_unlock( &m_accessMutex );
    sem_post( &m_available_slots );       // post that a new slow is now available

    return to_ret;
}

void ThreadSafeQueue::push( void* x)
{
    sem_wait( &m_available_slots );  // make sure there is room to insert the new element in the buffer
    pthread_mutex_lock( &m_accessMutex ); // lock the data buffer

    mp_backend->push_back( x );

    pthread_mutex_unlock( &m_accessMutex );
    sem_post( &m_available_items );  // post that a new item is available

}
