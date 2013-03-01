/*
This implementation uses the kmv esitimater from Wangchao Le.  The implementation uses several strange programing
techniques.  The expectation is that the implementation is not very effient and may need to be reimplemented if
we want to prove the speed of compression using this partitioner is comparable to what is being done currently
for centralized compression.

However, the current goal is to verify that using the estimator we can get comparable results compared
to calculating the Jaccard value exactly.  If it works well, this class and the kmv estimator may have to
be compleatly rewritten.
*/
#include <vector>
#include <string>
#include <limits>
#include <iostream>

#include "../include/LogDistributerAnalyzer_JaccardEstimator.h"
#include "../include/kmv.h"

using namespace std;

char* LogDistributerAnalyzer_JaccardEstimator::seperators = " \t\n:.\\/@[]();\"\'&=-,#";

LogDistributerAnalyzer_JaccardEstimator::LogDistributerAnalyzer_JaccardEstimator(int buckets, int depth, int kmvSize)
: m_bucketCount(buckets),
  m_slidingWindowSize(depth),
  m_kmvSize(kmvSize),
  m_bucketsInUse(0),
  m_threshold(0.5)
{
    mp_history = new vector< vector<kmv*>* >(m_bucketCount);
    for(int i=0; i < m_bucketCount; i++)
        mp_history->at(i) = new vector<kmv*>();
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
    int bestBucket = -1;
    float threshold = m_threshold;

    if(m_bucketsInUse >= m_bucketCount)
        threshold = 0;

    // make new kmv estimator
    kmv* newKMV = new kmv(m_kmvSize);
    makeKMVfromString(str, *newKMV);

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

int LogDistributerAnalyzer_JaccardEstimator::getBucket(const char* str)
{
    string val(str);
    int value = getBucket(val);
    return value;
}

void LogDistributerAnalyzer_JaccardEstimator::makeKMVfromString(const std::string str, kmv& val)
{
    vector<string> set;

    unsigned startidx = 0;
    unsigned endidx = 0;

    startidx = 0;
    endidx   = str.find_first_of(seperators, startidx);

    while(endidx < str.size())
    {
        // no use inserting a string with zero length
        if(endidx >= startidx + 1)
        {
            set.push_back(str.substr(startidx, endidx - startidx));
        }

        // find the next indext to use
        startidx = endidx+1;
        endidx = str.find_first_of(seperators, startidx+1);
    }

    // the set has been created, now generate the kmv estimator
    val.buildkmv(set);
}

void LogDistributerAnalyzer_JaccardEstimator::StatsInColumn(const std::vector<kmv*>* input,
                   kmv* compare,
                   float &average,
                   float &best)
{
    int count = 0;
    average   = 0.0;
    best      = 0.0;

    // this is what we do so we can compare individual elements in the column
    vector<kmv*> compareGroup(2);
    compareGroup[0] = compare;

    vector< kmv* >::const_iterator it = input->begin();
    for(; it != input->end(); it++)
    {
        compareGroup[1] = *it;
        float tmp = kmv::DVJaccardEst(compareGroup);
        average += tmp;
        best = max(best, (float)tmp);

        count++;
    }

    average = count == 0 ? 0.0 : average / count;
}

