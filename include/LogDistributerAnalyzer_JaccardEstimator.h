#ifndef LOGDISTRIBUTERANALYZER_JACCARDESTIMATOR_H
#define LOGDISTRIBUTERANALYZER_JACCARDESTIMATOR_H

#include <vector>
#include <string>
#include <thread>

#include "../include/LogDistributerAnalyzer.h"
//#include "../include/kmv.h"
#include "../include/kmv_est.h"
#include "../include/ThreadSafeQueue.h"


class LogDistributerAnalyzer_JaccardEstimator : public LogDistributerAnalyzer
{
    public:
        // buckets is the max number of buckets to keep track of
        // depth is the number of estimators for each bucket that is tracked.
        // kmvSize is the number of value the estimator will use to estimate similarity
        LogDistributerAnalyzer_JaccardEstimator(int buckets, int depth = 1, int kmvSize = 10, int qgramsize = 6);
        virtual ~LogDistributerAnalyzer_JaccardEstimator();

        virtual int getBucket(const std::string&);
        virtual int getBucket(const char*);

        virtual void* preBuild(const char*);
        virtual int ThreadedgetBucket(LogRecord* rec);

        static void ThreadStart(LogDistributerAnalyzer_JaccardEstimator*);
        void ThreadLoop( );
    protected:
    private:
        static float averageInColumn(const std::vector<kmv_est*>* input, const kmv_est* compare);
        static void StatsInColumn(const std::vector<kmv_est*>* input, const kmv_est* compare, float &average, float &best);

        int m_bucketCount;       // number of buckets to keep track of
        int m_slidingWindowSize; // how many values should be kept for each bucket
        int m_kmvSize;           // how many value should the kmv extimator
        int m_qgramsize;         // how large each qgram is when creating the estimator
        int m_threadGroupSize;   // number of columns a thread will process per request
        int m_threadStartCount;  // the number of threads to create in the thread pool.

        std::vector<std::thread*>* m_workers; // the worker threads used for async processing

        ThreadSafeQueue* m_dispatch;  // the queue used to give tasks to the workers
        ThreadSafeQueue* m_collector; // the queue used to collect information from tasks.

        int m_bucketsInUse;
        float m_threshold;

        std::vector< std::vector<kmv_est*>* >* mp_history;
        int* mp_historyIndex;

        static char* seperators;

        struct dispatch_data
        {
            kmv_est *to_compare;
            int start_column;
            int end_column;

        };
        struct collect_data
        {
            float best_average;
            int best_column;
        };
};

#endif // LOGDISTRIBUTERANALYZER_JACCARDESTIMATOR_H
