#ifndef LOGDISTRIBUTERANALYZER_EDITDISTANCE_H
#define LOGDISTRIBUTERANALYZER_EDITDISTANCE_H

#include <vector>
#include <string>
#include <inttypes.h>

#include "../include/LogDistributerAnalyzer.h"

class LogDistributerAnalyzer_EditDistance : public LogDistributerAnalyzer
{
    public:
        LogDistributerAnalyzer_EditDistance(int buckets);
        virtual ~LogDistributerAnalyzer_EditDistance();

        virtual int getBucket(const std::string&);
        virtual int getBucket(const char*);
    protected:
    private:
        static int getEditDistance(const char*, const char*);
        static int getEditDistance(const std::string&, const std::string&);

        static constexpr double CONSISTANCY_THRESHOLD = 0.3334;

        int m_buckets;
        int64_t m_stringsDistributed;
        std::vector<std::string> *mp_last_bucket;
        std::vector<int64_t> *mp_last_time_used; // tracks the last time a particular bucket was used.
};

#endif // LOGDISTRIBUTERANALYZER_EDITDISTANCE_H
