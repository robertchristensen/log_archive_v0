#ifndef LOGDISTRIBUTERANALYZER_CHARACTERSIMILARITY_H
#define LOGDISTRIBUTERANALYZER_CHARACTERSIMILARITY_H

#include <vector>
#include <list>

#include "../include/LogDistributerAnalyzer.h"
#include "../include/characterCount.h"


class LogDistributerAnalyzer_characterSimilarity : public LogDistributerAnalyzer
{
    public:
        LogDistributerAnalyzer_characterSimilarity(int buckets, int depth = 10);
        virtual ~LogDistributerAnalyzer_characterSimilarity();

        virtual int getBucket(const std::string&);
        virtual int getBucket(const char*);

        virtual void* preBuild(const char*)
        {   return NULL;     }
    protected:
    private:
    void StatsInColumn(const std::list< characterCount* >* input,
                                const characterCount* compare,
                                float &average,
                                float &best);

    static const int ms_defaultThreshold = 25;


    int m_buckets;
    int m_historyDepth;
    int m_usedBuckets;

    std::vector< std::list<characterCount*>* >* mp_history;
};

#endif // LOGDISTRIBUTERANALYZER_CHARACTERSIMILARITY_H
