#include "../include/ThreadSafeQueue.h"

#include <pthread.h>
#include <deque>

using namespace std;

ThreadSafeQueue::ThreadSafeQueue()
{
    mp_backend = new deque<void*>( );
    pthread_mutex_init(&m_accessMutex, NULL);
    sem_init(&m_available, 0, 0);
}

ThreadSafeQueue::~ThreadSafeQueue()
{
    pthread_mutex_destroy(&m_accessMutex);
    delete mp_backend;
}


bool ThreadSafeQueue::empty( ) const
{
//    pthread_mutex_lock( &m_accessMutex );
    bool toRet = mp_backend->empty( );
//    pthread_mutex_unlock( &m_accessMutex );

    return toRet;
}

int ThreadSafeQueue::size( ) const
{
//    pthread_mutex_lock( &m_accessMutex );
    int toRet = mp_backend->size( );
//    pthread_mutex_unlock( &m_accessMutex );

    return toRet;
}

void* ThreadSafeQueue::pop_front( )
{
    sem_wait( &m_available );
    pthread_mutex_lock( &m_accessMutex );
    void* toRet = mp_backend->front( );
    mp_backend->pop_front( );
    pthread_mutex_unlock( &m_accessMutex );

    return toRet;
}

//void* ThreadSafeQueue::back( )
//{
//    pthread_mutex_lock( &m_accessMutex );
//    void* toRet = mp_backend->back( );
//    pthread_mutex_unlock( &m_accessMutex );

//    return toRet;
//}

void ThreadSafeQueue::push( void* x)
{
    pthread_mutex_lock( &m_accessMutex );
    mp_backend->push_back( x );
    sem_post( &m_available );
    pthread_mutex_unlock( &m_accessMutex );
}

//void ThreadSafeQueue::pop( )
//{
//    sem_wait( )
//    pthread_mutex_lock( &m_accessMutex );
//    mp_backend->pop_front( );
//    pthread_mutex_unlock( &m_accessMutex );
//}
