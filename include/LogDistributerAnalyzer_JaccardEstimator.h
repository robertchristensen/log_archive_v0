#ifndef LOGDISTRIBUTERANALYZER_JACCARDESTIMATOR_H
#define LOGDISTRIBUTERANALYZER_JACCARDESTIMATOR_H

#include <vector>
#include <string>

#include "../include/LogDistributerAnalyzer.h"
#include "../include/kmv.h"


class LogDistributerAnalyzer_JaccardEstimator : public LogDistributerAnalyzer
{
    public:
        // buckets is the max number of buckets to keep track of
        // depth is the number of estimators for each bucket that is tracked.
        // kmvSize is the number of value the estimator will use to estimate similarity
        LogDistributerAnalyzer_JaccardEstimator(int buckets, int depth = 1, int kmvSize = 5);
        virtual ~LogDistributerAnalyzer_JaccardEstimator();

        virtual int getBucket(const std::string&);
        virtual int getBucket(const char*);
    protected:
    private:
        static void makeKMVfromString(const std::string str, kmv& val);
        static void StatsInColumn(const std::vector<kmv*>* input, kmv* compare, float &average, float &best);

        int m_bucketCount;       // number of buckets to keep track of
        int m_slidingWindowSize; // how many values should be kept for each bucket
        int m_kmvSize;           // how many value should the kmv extimator

        int m_bucketsInUse;
        float m_threshold;

        std::vector< std::vector<kmv*>* >* mp_history;
        int* mp_historyIndex;

        static char* seperators;
};

#endif // LOGDISTRIBUTERANALYZER_JACCARDESTIMATOR_H
