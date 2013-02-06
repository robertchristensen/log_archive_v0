#ifndef LOGDISTRIBUTERANALYZER_JACCARDSIMILARITY_H
#define LOGDISTRIBUTERANALYZER_JACCARDSIMILARITY_H

#include "../include/LogDistributerAnalyzer.h"

#include <set>
#include <vector>
#include <list>
#include <string>
#include <inttypes.h>

#include <pthread.h>


class LogDistributerAnalyzer_JaccardSimilarity : public LogDistributerAnalyzer
{
    public:
        LogDistributerAnalyzer_JaccardSimilarity(int buckets);
        virtual ~LogDistributerAnalyzer_JaccardSimilarity();

        virtual int getBucket(const std::string&);
        virtual int getBucket(const char*);
    protected:
        struct ThreadArguments
        {
            LogDistributerAnalyzer_JaccardSimilarity *launcher;
            const std::list< std::set<std::string>* > *column_data;
            const std::set<std::string> *toCompare;
            float *result;
        };

    private:
        void makeParsedSet(const std::string&, std::set<std::string>*) const;
        static float averageJaccardInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare);
        static float bestJaccardInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare);

        static void JaccardStatsInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare, float &average, float &best);

        int64_t m_stringsDistributed;
        int m_bucketcount;

        int m_leastUsedBucketUsed;
        int m_lastTimeBucketUsed;
        int m_maxleastBucketUsed;
        float m_avgleadBucketUsed;

        std::vector< std::list< std::set<std::string>* >* >* mp_history;
        //std::vector<int64_t> *mp_last_time_used; // tracks the last time a particular bucket was used.

        static char* seperators;
        const static int history_depth = 10;
        const static float threshold = 0.5;

};

#endif // LOGDISTRIBUTERANALYZER_JACCARDSIMILARITY_H
