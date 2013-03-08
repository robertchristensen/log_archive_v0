#ifndef LOGDISTRIBUTER_H
#define LOGDISTRIBUTER_H

#include <inttypes.h>
#include <vector>
#include <thread>
#include "../include/LogArchiver.h"
#include "../include/LogDistributerAnalyzer.h"
#include "../include/ThreadSafeQueue.h"

class LogDistributer
{
public:
    enum DistributerType { ROUND_ROBIN, EDIT_DISTANCE, JACCARD, CHAR, JAC_EST, JACCARD_COMBINE, JACCARD_QGRAM };


    LogDistributer(int num_archivers, DistributerType distrib = ROUND_ROBIN);
    virtual ~LogDistributer();

    // insert a new record (blocking operation)
    int insert(char *, int size);

    // insert a record into the archiver in the background
    int Background_insert(char *, int size);

    // close the files and archivers
    int close();

    // starts the thread that does step 1 processing
    // which includes creating the data structure used by the log distributer analyzer
    static void StartThread_step1( LogDistributer* );
    void ThreadLoop_step1( );

    static int JACCARD_HISTORY;
    static int Q_GRAM_LENGTH;
    static int KMV_MAX_K;
protected:
private:
    // the distributer type this log distributer is using
    DistributerType m_thisDistributerType;
    std::vector<LogArchiver*> *mp_archivers;
    LogDistributerAnalyzer *mp_analyzer;

    // this is the queue of messages that have not been processed.  These
    // have been copied from the source
    ThreadSafeQueue* mp_rawQueue;
    std::thread* mp_stepOneThread;

    //
    //ThreadSafeQueue* mp_

    LogArchiver* mp_index;

    int m_index;

    bool m_closed;

    static const int DEFAULT_INSERT_QUEUE_SIZE = 1000;
};

#endif // LOGDISTRIBUTER_H
