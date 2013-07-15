#include "../include/LogDistributer.h"
#include "../include/LogArchiver.h"
#include "../include/LogDistributerAnalyzer.h"
#include "../include/LogDistributerAnalyzer_RoundRobin.h"
#include "../include/LogDistributerAnalyzer_EditDistance.h"
#include "../include/LogDistributerAnalyzer_JaccardSimilarity.h"
#include "../include/LogDistributerAnalyzer_characterSimilarity.h"
#include "../include/LogDistributerAnalyzer_JaccardEstimator.h"
#include "../include/LogDistributerAnalyzer_JaccardSimilarityCombined.h"
#include "../include/LogDistributerAnalyzer_JaccardSimilarity_qgramExact.h"

#include <cstring>
#include <vector>
#include <thread>
#include <iostream>

using namespace std;

int LogDistributer::JACCARD_HISTORY = 5;
int LogDistributer::Q_GRAM_LENGTH   = 6;
int LogDistributer::KMV_MAX_K       = 60;

LogDistributer::LogDistributer(int num_archivers, DistributerType distrib)
: m_thisDistributerType(distrib),
  m_index(0),
  m_closed(0)
{
    switch(distrib)
    {
         case ROUND_ROBIN:
            mp_analyzer = new LogDistributerAnalyzer_RoundRobin(num_archivers);
            break;
        case EDIT_DISTANCE:
            mp_analyzer = new LogDistributerAnalyzer_EditDistance(num_archivers);
            break;
        case JACCARD:
            mp_analyzer = new LogDistributerAnalyzer_JaccardSimilarity(num_archivers, JACCARD_HISTORY);
            break;
        case CHAR:
            mp_analyzer = new LogDistributerAnalyzer_characterSimilarity(num_archivers, JACCARD_HISTORY);
            break;
        case JAC_EST:
            mp_analyzer = new LogDistributerAnalyzer_JaccardEstimator(num_archivers, JACCARD_HISTORY, KMV_MAX_K, Q_GRAM_LENGTH);
            break;
        case JACCARD_COMBINE:
            mp_analyzer = new LogDistributerAnalyzer_JaccardSimilarityCombined(num_archivers, JACCARD_HISTORY);
            break;
        case JACCARD_QGRAM:
            mp_analyzer = new LogDistributerAnalyzer_JaccardSimilarity_qgramExact(num_archivers, JACCARD_HISTORY, 6);
            break;
        default:
            mp_analyzer = new LogDistributerAnalyzer_RoundRobin(num_archivers);
    }
    mp_archivers = new vector<LogArchiver*>( );
    mp_index = new LogArchiver("index");

    mp_rawQueue = new ThreadSafeQueue(DEFAULT_INSERT_QUEUE_SIZE);
    mp_stepOneThread = new thread(StartThread_step1, this);

    mp_toProcess = new ThreadSafeQueue(DEFAULT_INSERT_QUEUE_SIZE);
    mp_stepTwoThread = new thread(StartThread_step2, this);
}

LogDistributer::~LogDistributer()
{
    if(m_closed != true)
        this->close();

    vector<LogArchiver*>::iterator it = mp_archivers->begin();
    for( ; it != mp_archivers->end() ; it++ )
        delete *it;

    delete mp_archivers;
    delete mp_analyzer;
    delete mp_index;
    delete mp_rawQueue;
    delete mp_stepOneThread;
    delete mp_toProcess;
    delete mp_stepTwoThread;
}

int LogDistributer::close()
{
    if(m_closed == true)
        return 0;

    mp_rawQueue->push( NULL );
    mp_toProcess->push( NULL );

    mp_stepOneThread->join();
    mp_stepTwoThread->join();

    m_closed = true;
    vector<LogArchiver*>::iterator it = mp_archivers->begin();
    for( ; it != mp_archivers->end() ; it++)
        (*it)->close();

    return 0;
}

void LogDistributer::StartThread_step1( LogDistributer* i )
{
    i->ThreadLoop_step1( );
    return;
}

void LogDistributer::ThreadLoop_step1( )
{
    void* from_queue;
    //int bucket;
    //char cbucket;
    LogRecord* data;
    while((from_queue = this->mp_rawQueue->pop_front( )) != NULL)
    {
        data = (LogRecord*) from_queue;
        void* xdata = mp_analyzer->preBuild(data->m_str);
        data->extra = xdata;
        mp_toProcess->push(data);
    }
    return;
}

void LogDistributer::StartThread_step2( LogDistributer* i )
{
    i->ThreadLoop_step2( );
    return;
}

void LogDistributer::ThreadLoop_step2( )
{
    void* from_queue;
    int bucket;
    LogRecord* data;
    char cbucket;

    while((from_queue = this->mp_toProcess->pop_front( )) != NULL)
    {
        data = (LogRecord*) from_queue;
        //bucket = mp_analyzer->ThreadedgetBucket(data->extra);
        bucket = mp_analyzer->ThreadedgetBucket(data);

        cbucket = (unsigned char) bucket;

        if(bucket == mp_archivers->size())
        {
            mp_archivers->push_back(new LogArchiver(bucket));
        }



        data->extra = NULL;
        mp_archivers->at(bucket)->BackgroundInsertRecord(data);

        mp_index->BackgroundInsertRecord( &cbucket, 1 );
    }
}

int LogDistributer::insert(char *str, int size)
{
    // find string similarity before repacking
    int bucket = mp_analyzer->getBucket(str);
    char b = (char) bucket;

    // this will happen when we need to add a new bucket.
    if(bucket == mp_archivers->size())
    {
        mp_archivers->push_back(new LogArchiver(bucket));
    }

    // insert the record into the appropreate bucket
    int retVal = mp_archivers->at(bucket)->InsertRecord(str, size);

    // insert the new record into the index file
    mp_index->InsertRecord( &b, 1 );
    //mp_index->BackgroundInsertRecord( &b, 1 );

    return retVal;
}

int LogDistributer::Background_insert(char *str, int size)
{
    mp_rawQueue->push( new LogRecord(str, size));
    return 0;
}
