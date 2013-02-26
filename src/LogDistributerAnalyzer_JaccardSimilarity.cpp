#include "../include/LogDistributerAnalyzer_JaccardSimilarity.h"
#include "../include/SetTools.h"

#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <set>
#include <list>
#include <inttypes.h>
#include <limits.h>
#include <climits>
#include <iostream>
#include <cstdio>

using namespace std;

char* LogDistributerAnalyzer_JaccardSimilarity::seperators = " \t\n:.\\/@[]();\"\'&=-,#";

LogDistributerAnalyzer_JaccardSimilarity::LogDistributerAnalyzer_JaccardSimilarity(int buckets, int depth)
:  m_stringsDistributed(0)
,  m_bucketcount(buckets)
,  m_historyDepth(depth)
,  m_leastUsedBucketUsed(0)
,  m_lastTimeBucketUsed(0)
,  m_maxleastBucketUsed(0)
,  m_avgleadBucketUsed(0.0)
,  m_threshold(default_threshold)
{
    if(m_historyDepth <= 0)
        m_historyDepth = history_depth_default;

    mp_history = new vector< list< set<string>* >* >(m_bucketcount);

    vector< list< set<string>* >* >::iterator it;
    for(it = mp_history->begin(); it != mp_history->end(); ++it)
    {
        *it = new list< set<string>* >();
    }

    // initialize thread stuff
    //mp_threadQueue = new ThreadSafeQueue( );
    //t_arg.mp_queue = mp_threadQueue;

    //for(int i=0; i<MAX_THREAD_COUNT; i++)
    //{
    //    int rc = pthread_create(&threads[i], NULL, ThreadRunner, (void *) &t_arg);//pthread_t threads[MAX_THREAD_COUNT];
    //    if (rc){
    //        cerr << "ERROR: unable to create a mapper thread" << endl;
    //        exit(-1);
    //    }
    //}
}

LogDistributerAnalyzer_JaccardSimilarity::~LogDistributerAnalyzer_JaccardSimilarity()
{
    // issue to kill all the threads
    // note: as of right now, there is a memory leak.  It is assumed that this is issued
    //   near the end of the program lifetime.  The memroy leak is also very small, but at least
    //   the threads have died.
    //ThreadArguments *tmp = new ThreadArguments;
    //tmp->kill_thread = true;
    //for(int i=0; i<MAX_THREAD_COUNT; i++)
    //    mp_threadQueue->push(&tmp);

    sleep(0);

    vector< list< set<string>* >* >::iterator it_v;
    for(it_v = mp_history->begin(); it_v != mp_history->end(); ++it_v)
    {
        list< set<string>* >::iterator it_l;
        for(it_l = (*it_v)->begin(); it_l != (*it_v)->end(); ++it_l)
        {
            delete *it_l;
        }
        delete *it_v;
    }
    delete mp_history;
}

int LogDistributerAnalyzer_JaccardSimilarity::getBucket(const std::string& input)
{
    int best_index = -1;
    int index = 0;
    list< set<string>* >* best_location = NULL;
    float best_value = m_threshold; // we only want to consiter the bucket if it is better then the min threshold

    // if all the buckets have already been used, it would be best to place the entry
    // in whatever bucket it would work best in.
    if( m_leastUsedBucketUsed >= m_bucketcount )
        best_value = 0.0;

    set<string>* set_input = new set<string>();
    makeParsedSet(input, set_input);

//    best_index = this->ThreadedBestBucket( set_input, best_value );

    vector< list< set<string>* >* >::iterator it;
    for( it = mp_history->begin(), index = 0; index < m_leastUsedBucketUsed && it != mp_history->end(); it++, index++ )
    {
        float average;
        float best_in_column;
        JaccardStatsInColumn( *it, set_input, average, best_in_column);
        if( average > best_value )
        {
            best_index = index;
            best_value = average;
            best_location = *it;
        }
    }

    // this if statment is used if not all the buckets have been filled or if
    // no similarity is detected.  So similartiy detected should rarely happen.
    if( best_index == -1 || best_location == NULL ){
          best_index = m_leastUsedBucketUsed % mp_history->size();

          ++m_leastUsedBucketUsed;
          //m_threshold *=  0.95;
    }

    best_location = mp_history->at( best_index );
    best_location->push_front( set_input );


    // remove the last element in the history if it exists, to keep the number of records maintanable
    while(best_location->size() > m_historyDepth)
    {
        delete best_location->back();
        best_location->pop_back();
    }

    return best_index;
}

int LogDistributerAnalyzer_JaccardSimilarity::getBucket(const char* str)
{
    return this->getBucket(string(str));
}

/*
returns the average jaccard distance of a column.  The Jaccard distance is calculated for each value
in the set compared to the input
*/
float LogDistributerAnalyzer_JaccardSimilarity::averageJaccardInColumn(const list< set<string>* >* input, const set<string>* compare)
{
    float average;
    float best;

    JaccardStatsInColumn(input, compare, average, best);
    return average;
}

float LogDistributerAnalyzer_JaccardSimilarity::bestJaccardInColumn(const list< set<string>* >* input, const set<string>* compare)
{
    float average;
    float best;

    JaccardStatsInColumn(input, compare, average, best);
    return best;
}

void LogDistributerAnalyzer_JaccardSimilarity::JaccardStatsInColumn(const std::list< std::set<std::string>* >* input,
                                                                    const std::set<std::string>* compare,
                                                                    float &average,
                                                                    float &best)
{
    int count = 0;
    average = 0.0;
    best    = 0.0;

    list< set<string>* >::const_iterator it = input->begin();
    for(; it != input->end(); it++)
    {
        float tmp = Jaccard(*(*(it)), *compare);
        average += tmp;
        best = best > tmp ? best : tmp;
        count++;
    }

    average = count == 0 ? 0.0 : average / count;
}

void LogDistributerAnalyzer_JaccardSimilarity::makeParsedSet(const std::string& input, set<std::string>* output) const
{
    output->clear();

    unsigned startidx = 0;
    unsigned endidx = 0;

    startidx = 0;
    endidx   = input.find_first_of(seperators, startidx);
    while(endidx < input.size())
    {
        // no use inserting a string with zero length
        if(endidx >= startidx + 1)
        {
            output->insert(input.substr(startidx, endidx - startidx ));
        }

        startidx = endidx+1;
        endidx = input.find_first_of(seperators, startidx+1);
    }
}

void *LogDistributerAnalyzer_JaccardSimilarity::ThreadRunner( void * arg )
{
    ThreadInit* t_arg = (ThreadInit*) arg;
    ThreadArguments* input;

    while( true )
    {
        input = (ThreadArguments*)t_arg->mp_queue->pop_front( );

        // if this thread is supposed to die, it should imediatly die
        if(input->kill_thread)
            break;

        // otherwise, perform the required computation:
        *(input->write_location) = averageJaccardInColumn(input->p_column, input->p_compareSet);

        // after the thread is done with the work, it marks the indicator that is is done by releasing the semiphore.
        sem_post( input->complete_indicator );

        // when this is done, it will cycle back around to get more work.
    }

    pthread_exit((void*) 0);
}

int LogDistributerAnalyzer_JaccardSimilarity::ThreadedBestBucket( const std::set<std::string>* input_set, float best_value )
{
    int data_sets = min((int)mp_history->size(), m_leastUsedBucketUsed);
    //float best_value = 0.0;
    int   best_index = -1;


    // setup semiphore
    //sem_t work_indicator;
    //sem_init(&work_indicator, 0, )
    sem_t* semiph = new sem_t[data_sets];
    for(int i=0; i<data_sets; i++)
        sem_init(&semiph[i], 0, 0);

    // setup thread arguments
    float *retVals = new float[data_sets];
    ThreadArguments *args = new ThreadArguments[data_sets];
    for(int i=0; i<data_sets; i++)
    {
        args[i].p_column = mp_history->at(i);
        args[i].p_compareSet = input_set;
        args[i].write_location = &retVals[i];
        args[i].complete_indicator = &semiph[i];
        args[i].kill_thread = false;

        // push the thread arguments to the queue
        mp_threadQueue->push(&args[i]);
    }

    // wait for semiphore
    for(int i=0; i<data_sets; i++)
    {
        sem_wait(&semiph[i]);

        if( retVals[i] > best_value )
        {
            best_index = i;
            best_value = retVals[i];
        }
    }

    // clean up data
    delete []semiph;
    delete []retVals;
    delete []args;

    return best_index;
}

