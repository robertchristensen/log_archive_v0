#ifndef LOGDISTRIBUTERANALYZER_ROUNDROBIN_H
#define LOGDISTRIBUTERANALYZER_ROUNDROBIN_H

//#include <../include/LogDistributerAnalyzer.h>
#include "../include/LogDistributerAnalyzer.h"

class LogDistributerAnalyzer_RoundRobin : public LogDistributerAnalyzer
{
    public:
        LogDistributerAnalyzer_RoundRobin(int number_of_buckets);
        virtual ~LogDistributerAnalyzer_RoundRobin();

        virtual int getBucket(const std::string&);
        virtual int getBucket(const char*);

        int getNextIndex();
    protected:
    private:
        int m_buckets;
        int m_index;
};

#endif // LOGDISTRIBUTERANALYZER_ROUNDROBIN_H
