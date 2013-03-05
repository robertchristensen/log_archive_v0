#ifndef LOGDISTRIBUTERANALYZER_JACCARDSIMILARITY_QGRAMEXACT_H
#define LOGDISTRIBUTERANALYZER_JACCARDSIMILARITY_QGRAMEXACT_H

#include "../include/LogDistributerAnalyzer.h"

#include <set>
#include <vector>
#include <list>
#include <string>
#include <inttypes.h>

class LogDistributerAnalyzer_JaccardSimilarity_qgramExact : public LogDistributerAnalyzer
{
        public:
        LogDistributerAnalyzer_JaccardSimilarity_qgramExact(int buckets, int depth = 1, int qgramLength = 3);
        virtual ~LogDistributerAnalyzer_JaccardSimilarity_qgramExact();

        virtual int getBucket(const std::string&);
        virtual int getBucket(const char*);


    protected:

    private:
        void makeParsedSet(const std::string&, std::set<std::string>*);

        static float averageJaccardInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare);
        static float bestJaccardInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare);
        static void JaccardStatsInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare, float &average, float &best);

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
        int m_qgramLength;

        std::vector< std::list< std::set<std::string>* >* >* mp_history;
        //std::vector<int64_t> *mp_last_time_used; // tracks the last time a particular bucket was used.

        static char* seperators;
        const static int history_depth_default = 10;
        constexpr static float default_threshold = 0.5;
        constexpr static float upper_threshold = 0.9;
};

#endif // LOGDISTRIBUTERANALYZER_JACCARDSIMILARITY_QGRAMEXACT_H
