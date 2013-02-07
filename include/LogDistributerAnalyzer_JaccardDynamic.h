#ifndef LOGDISTRIBUTERANALYZER_JACCARDDYNAMIC_H
#define LOGDISTRIBUTERANALYZER_JACCARDDYNAMIC_H

#include <vector>
#include <string>
#include <list>
#include <set>

#include "../include/LogDistributerAnalyzer.h"


class LogDistributerAnalyzer_JaccardDynamic : public LogDistributerAnalyzer
{
    public:
        LogDistributerAnalyzer_JaccardDynamic(int buckets, int depth = 0);
        virtual ~LogDistributerAnalyzer_JaccardDynamic();

        virtual int getBucket(const std::string&);
        virtual int getBucket(const char*);
    protected:

    private:
        void makeParsedSet(const std::string&, std::set<std::string>*) const;
        static float averageJaccardInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare);
        static float bestJaccardInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare);

        static void JaccardStatsInColumn(const std::list< std::set<std::string>* >* input, const std::set<std::string>* compare, float &average, float &best);

        //int64_t m_stringsDistributed;
        int m_bucketcount;
        int m_historyDepth;

        int m_leastUsedBucketUsed;
        int m_lastTimeBucketUsed;
        int m_maxleastBucketUsed;
        float m_avgleadBucketUsed;

        std::vector< std::list< std::set<std::string>* >* >* mp_history;
        //std::vector<int64_t> *mp_last_time_used; // tracks the last time a particular bucket was used.

        static char* seperators;
        const static int history_depth_default = 10;
        const static float threshold = 0.6;
};

#endif // LOGDISTRIBUTERANALYZER_JACCARDDYNAMIC_H
