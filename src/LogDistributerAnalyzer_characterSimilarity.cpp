#include "../include/LogDistributerAnalyzer_characterSimilarity.h"
#include "../include/characterCount.h"

#include <vector>
#include <list>
#include <limits>

using namespace std;

LogDistributerAnalyzer_characterSimilarity::LogDistributerAnalyzer_characterSimilarity(int buckets, int depth)
:   m_buckets(buckets)
,   m_historyDepth(depth)
,   m_usedBuckets(0)
{
    mp_history = new vector< list< characterCount* >* >(m_buckets);

    vector< list< characterCount* >* >::iterator it;
    for(it = mp_history->begin(); it != mp_history->end(); ++it)
    {
        *it = new list< characterCount* >();
    }
}

LogDistributerAnalyzer_characterSimilarity::~LogDistributerAnalyzer_characterSimilarity()
{
    vector< list< characterCount* >* >::iterator it_v;
    for(it_v = mp_history->begin(); it_v != mp_history->end(); ++it_v)
    {
        list< characterCount* >::iterator it_l;
        for(it_l = (*it_v)->begin(); it_l != (*it_v)->end(); ++it_l)
        {
            delete *it_l;
        }
        delete *it_v;
    }
    delete mp_history;
}

int LogDistributerAnalyzer_characterSimilarity::getBucket(const std::string&str)
{
    return this->getBucket(str.c_str());
}

int LogDistributerAnalyzer_characterSimilarity::getBucket(const char* str)
{
    int bestBucket = -1;
    float threshold = ms_defaultThreshold;

    if(m_usedBuckets >= m_buckets)
        threshold = std::numeric_limits<int>::max();

    // create the couting set
    characterCount* to_insert = new characterCount(str);

    float average;
    float max;
    // iterate existing buckets, looking for a value that is below the threshold
    for( int i = 0; i < mp_history->size() && i < m_usedBuckets; i++)
    {
        StatsInColumn(mp_history->at(i), to_insert, average, max);
        if(average < threshold)
        {
            bestBucket = i;
            threshold = average;
        }
    }

    // if a bucket can not be found that is bellow the threshold, start using a new bucket
    if(bestBucket == -1)
    {
        bestBucket = m_usedBuckets++;
    }

    // cleanup and insert the new record into the appropreate location.
    mp_history->at( bestBucket )->push_front( to_insert );

    // remove the last element in the history if it exists, to keep the number of records maintanable
    if(mp_history->at( bestBucket )->size() > m_historyDepth)
    {
        delete mp_history->at( bestBucket )->back();
        mp_history->at( bestBucket )->pop_back();
    }

    return bestBucket;
}

void LogDistributerAnalyzer_characterSimilarity::StatsInColumn(const std::list< characterCount* >* input,
                                                                const characterCount* compare,
                                                                float &average,
                                                                float &best)
{
    int count = 0;
    average = 0.0;
    best    = 0.0;

    list< characterCount* >::const_iterator it = input->begin();
    for(; it != input->end(); it++)
    {
        int tmp = (*it)->linearDifference(*compare);//Jaccard(*(*(it)), *compare);
        average += tmp;
        best = best > tmp ? best : tmp;
        count++;
    }

    average = count == 0 ? 0.0 : average / count;
}
