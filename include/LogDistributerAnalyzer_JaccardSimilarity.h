#ifndef LOGDISTRIBUTERANALYZER_JACCARDSIMILARITY_H
#define LOGDISTRIBUTERANALYZER_JACCARDSIMILARITY_H

#include "../include/LogDistributerAnalyzer.h"
#include "../include/ThreadSafeQueue.h"

#include <set>
#include <vector>
#include <list>
#include <string>
#include <inttypes.h>

#include <pthread.h>
#include <semaphore.h>

#define MAX_THREAD_COUNT (8)


class LogDistributerAnalyzer_JaccardSimilarity : public LogDistributerAnalyzer
{
    public:
        LogDistributerAnalyzer_JaccardSimilarity(int buckets, int depth = 1);
        virtual ~LogDistributerAnalyzer_JaccardSimilarity();

        virtual int getBucket(const std::string&);
        virtual int getBucket(const char*);
    protected:
        struct ThreadArguments
        {
            const std::list< std::set<std::string>* >* p_column;
            const std::set<std::string>* p_compareSet;
            float* write_location;
            sem_t* complete_indicator;
            bool kill_thread; // this is true if the thread should die before processing the input
        };

        struct ThreadInit
        {
            ThreadSafeQueue *mp_queue;
        };

    private:
        void makeParsedSet(const std::string&, std::set<std::string>*) const;
        static float averageJaccardInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare);
        static float bestJaccardInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare);
        static void JaccardStatsInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare, float &average, float &best);

        static void *ThreadRunner( void * arg );
        int ThreadedBestBucket( const std::set<std::string>* input_set, float minimum_value );

        pthread_t threads[MAX_THREAD_COUNT];
        ThreadSafeQueue *mp_threadQueue;
        ThreadInit t_arg;
        //sem_t thread_work_queued;


        int64_t m_stringsDistributed;
        int m_bucketcount;
        int m_historyDepth;

        int m_leastUsedBucketUsed;
        int m_lastTimeBucketUsed;
        int m_maxleastBucketUsed;
        float m_avgleadBucketUsed;
        float m_threshold;
        float m_upperThreshold;

        std::vector< std::list< std::set<std::string>* >* >* mp_history;
        //std::vector<int64_t> *mp_last_time_used; // tracks the last time a particular bucket was used.

        static char* seperators;
        const static int history_depth_default = 10;
        const static float default_threshold = 0.5;
        const static float upper_threshold = 0.9;
};

#endif // LOGDISTRIBUTERANALYZER_JACCARDSIMILARITY_H
