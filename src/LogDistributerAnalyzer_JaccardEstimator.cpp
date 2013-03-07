/*
This implementation uses the kmv esitimater from Wangchao Le.  The implementation uses several strange programing
techniques.  The expectation is that the implementation is not very effient and may need to be reimplemented if
we want to prove the speed of compression using this partitioner is comparable to what is being done currently
for centralized compression.

However, the current goal is to verify that using the estimator we can get comparable results compared
to calculating the Jaccard value exactly.  If it works well, this class and the kmv estimator may have to
be compleatly rewritten.

NEW:::::
I have rewritten the kmv estimator so it would be more optimzied for this particular application.  I have
removed copying variables significantly.  Under the direction of Fiefie, I will be using qgrams to create
the set, which means the sets will have significntly more elements
*/
#include <vector>
#include <string>
#include <limits>
#include <iostream>

#include "../include/LogDistributerAnalyzer_JaccardEstimator.h"
//#include "../include/kmv.h"
#include "../include/kmv_est.h"

using namespace std;

char* LogDistributerAnalyzer_JaccardEstimator::seperators = " \t\n:.\\/@[]();\"\'&=-,#";

LogDistributerAnalyzer_JaccardEstimator::LogDistributerAnalyzer_JaccardEstimator(int buckets, int depth, int kmvSize, int qgramSize)
: m_bucketCount(buckets),
  m_slidingWindowSize(depth),
  m_kmvSize(kmvSize),
  m_qgramsize(qgramSize),
  m_bucketsInUse(0),
  m_threshold(0.5)
{
    mp_history = new vector< vector<kmv_est*>* >(m_bucketCount);
    for(int i=0; i < m_bucketCount; i++)
        mp_history->at(i) = new vector<kmv_est*>();
    mp_historyIndex = new int[m_bucketCount];
    for(int i=0; i < m_bucketCount; i++)
        mp_historyIndex[0] = 0;
}

LogDistributerAnalyzer_JaccardEstimator::~LogDistributerAnalyzer_JaccardEstimator()
{
    delete [] mp_historyIndex;

    for(int i = 0 ; i < mp_history->size() ; i++)
    {
        for(int t = 0; t < (mp_history->at(i))->size() ; t++)
        {
            delete mp_history->at(i)->at(t);
            mp_history->at(i)->at(t) = NULL;
        }

        //delete mp_history[i];
    }
}

int LogDistributerAnalyzer_JaccardEstimator::getBucket(const std::string& str)
{
    return getBucket(str.c_str());
}

int LogDistributerAnalyzer_JaccardEstimator::getBucket(const char* str)
{
    int bestBucket = -1;
    float threshold = m_threshold;

    if(m_bucketsInUse >= m_bucketCount)
        threshold = 0;

    // make new kmv estimator
    kmv_est* newKMV = new kmv_est(str, m_kmvSize, m_qgramsize);

    // search the main list to find the best location to place the new log entry
    float average;
    float max;
    for( int i = 0; i < mp_history->size() && i < m_bucketsInUse; i++)
    {
        StatsInColumn(mp_history->at(i), newKMV, average, max);
        if(average >= threshold)
        {
            bestBucket = i;
            threshold = average;
        }
    }

    // if a bucket can not be found that is bellow the threshold, start using a new bucket
    if(bestBucket == -1)
    {
        bestBucket = m_bucketsInUse++;
    }

    // cleanup and inert the new record into the appropreate location.

    // since we are using vectors, we have to make sure we are replacing only when the vector group is full
    if(mp_history->at( bestBucket )->size() == m_slidingWindowSize)
    {
        delete mp_history->at(bestBucket)->at(mp_historyIndex[bestBucket]);
        mp_history->at(bestBucket)->at(mp_historyIndex[bestBucket]) = newKMV;
    }
    else
    {
        mp_history->at(bestBucket)->push_back(newKMV);
    }
    // updates the next location to insert in this bucket
    mp_historyIndex[bestBucket] = (mp_historyIndex[bestBucket] + 1) % m_slidingWindowSize;

    // remove the last element in the history if it exists, to keep the number of records maintanable

    //cout << "*" << bestBucket << "*" << threshold << "*" << endl;
    return bestBucket;
}

void LogDistributerAnalyzer_JaccardEstimator::StatsInColumn(const std::vector<kmv_est*>* input,
                   kmv_est* compare,
                   float &average,
                   float &best)
{
    int count = 0;
    average   = 0.0;
    best      = 0.0;

    // this is what we do so we can compare individual elements in the column
    vector<kmv_est*>::const_iterator it = input->begin();
    for( ; it < input->end(); it++, count++)
    {
        float tmp = compare->jaccard_est(*it);
        average += tmp;
        best = max(best, tmp);
    }

    average = count == 0 ? 0.0 : average / count;
}

