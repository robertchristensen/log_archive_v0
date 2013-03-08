/*
this class is a test to see if combining the set to do the jaccard function changes the
compression ratio drastically.

The initial implementation is likely to be very slow.  If the compression ratio is good,
optimization can easily be applied.
*/
#include <vector>
#include <iostream>
#include <set>
#include <string>

using namespace std;


#include "../include/LogDistributerAnalyzer_JaccardSimilarityCombined.h"
#include "../include/LogDistributerAnalyzer_JaccardSimilarity.h"
#include "../include/SetTools.h"

LogDistributerAnalyzer_JaccardSimilarityCombined::LogDistributerAnalyzer_JaccardSimilarityCombined(int buckets, int depth)
:  m_stringsDistributed(0)
,  m_bucketcount(buckets)
,  m_historyDepth(depth)
,  m_leastUsedBucketUsed(0)
,  m_lastTimeBucketUsed(0)
,  m_maxleastBucketUsed(0)
,  m_avgleadBucketUsed(0.0)
,  m_threshold(default_threshold)
{
    if(m_historyDepth <= 0)
    m_historyDepth = history_depth_default;

    mp_history = new vector< list< set<string>* >* >(m_bucketcount);

    vector< list< set<string>* >* >::iterator it;
    for(it = mp_history->begin(); it != mp_history->end(); ++it)
    {
        *it = new list< set<string>* >();
    }
}

LogDistributerAnalyzer_JaccardSimilarityCombined::~LogDistributerAnalyzer_JaccardSimilarityCombined()
{
    vector< list< set<string>* >* >::iterator it_v;
    for(it_v = mp_history->begin(); it_v != mp_history->end(); ++it_v)
    {
        list< set<string>* >::iterator it_l;
        for(it_l = (*it_v)->begin(); it_l != (*it_v)->end(); ++it_l)
        {
            delete *it_l;
        }
        delete *it_v;
    }
    delete mp_history;
}

int LogDistributerAnalyzer_JaccardSimilarityCombined::getBucket(const std::string& input)
{
    int best_index = -1;
    int index = 0;
    list< set<string>* >* best_location = NULL;
    float best_value = m_threshold; // we only want to consiter the bucket if it is better then the min threshold

    // if all the buckets have already been used, it would be best to place the entry
    // in whatever bucket it would work best in.
    if( m_leastUsedBucketUsed >= m_bucketcount )
        best_value = 0.0;

    set<string>* set_input = new set<string>();
    LogDistributerAnalyzer_JaccardSimilarity::makeParsedSet(input, set_input);

//    best_index = this->ThreadedBestBucket( set_input, best_value );

    vector< list< set<string>* >* >::iterator it;
    for( it = mp_history->begin(), index = 0; index < m_leastUsedBucketUsed && it != mp_history->end(); it++, index++ )
    {
        float average;
        float best_in_column;
        JaccardStatsInColumn( *it, set_input, average, best_in_column);
        if( average > best_value )
        {
            best_index = index;
            best_value = average;
            best_location = *it;
        }
    }

    // this if statment is used if not all the buckets have been filled or if
    // no similarity is detected.  So similartiy detected should rarely happen.
    if( best_index == -1 || best_location == NULL ){
          best_index = m_leastUsedBucketUsed % mp_history->size();

          ++m_leastUsedBucketUsed;
          //m_threshold *=  0.95;
    }

    best_location = mp_history->at( best_index );
    best_location->push_front( set_input );


    // remove the last element in the history if it exists, to keep the number of records maintanable
    while(best_location->size() > m_historyDepth)
    {
        delete best_location->back();
        best_location->pop_back();
    }

    return best_index;
}

int LogDistributerAnalyzer_JaccardSimilarityCombined::getBucket(const char* str)
{
    return this->getBucket(string(str));
}

/*
returns the average jaccard distance of a column.  The Jaccard distance is calculated for each value
in the set compared to the input
*/
float LogDistributerAnalyzer_JaccardSimilarityCombined::averageJaccardInColumn(const list< set<string>* >* input, const set<string>* compare)
{
    float average;
    float best;

    JaccardStatsInColumn(input, compare, average, best);
    return average;
}

float LogDistributerAnalyzer_JaccardSimilarityCombined::bestJaccardInColumn(const list< set<string>* >* input, const set<string>* compare)
{
    float average;
    float best;

    JaccardStatsInColumn(input, compare, average, best);
    return best;
}

void LogDistributerAnalyzer_JaccardSimilarityCombined::JaccardStatsInColumn(const std::list< std::set<std::string>* >* input,
                                                                    const std::set<std::string>* compare,
                                                                    float &average,
                                                                    float &best)
{
    average = 0.0;
    best    = 0.0;

    set<string> newGroup;

    // build a set will all values from the list of sets
    list< set<string>* >::const_iterator it = input->begin();
    for(; it != input->end(); it++)
        newGroup.insert((*it)->begin(), (*it)->end());

    average = best = Jaccard(newGroup, *compare);
}
