#ifndef LOGDISTRIBUTERANALYZER_JACCARDSIMILARITYCOMBINED_H
#define LOGDISTRIBUTERANALYZER_JACCARDSIMILARITYCOMBINED_H

#include <vector>
#include <string>
#include <list>
#include <set>

#include "../include/LogDistributerAnalyzer.h"

class LogDistributerAnalyzer_JaccardSimilarityCombined : public LogDistributerAnalyzer
{
    public:
        LogDistributerAnalyzer_JaccardSimilarityCombined(int buckets, int depth = 1);
        virtual ~LogDistributerAnalyzer_JaccardSimilarityCombined();

        virtual int getBucket(const std::string&);
        virtual int getBucket(const char*);
    protected:
    private:
        static float averageJaccardInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare);
        static float bestJaccardInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare);
        static void JaccardStatsInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare, float &average, float &best);

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
        constexpr static float default_threshold = 0.5;
        constexpr static float upper_threshold = 0.9;
};

#endif // LOGDISTRIBUTERANALYZER_JACCARDSIMILARITYCOMBINED_H
