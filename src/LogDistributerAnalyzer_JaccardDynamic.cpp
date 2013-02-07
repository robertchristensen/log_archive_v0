#include "../include/LogDistributerAnalyzer_JaccardDynamic.h"
#include "../include/SetTools.h"

#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <set>
#include <list>
#include <inttypes.h>
#include <limits.h>
#include <climits>
#include <iostream>
#include <cstdio>

using namespace std;

char* LogDistributerAnalyzer_JaccardDynamic::seperators = " \t\n:.\\/@[]();\"&=-";

LogDistributerAnalyzer_JaccardDynamic::LogDistributerAnalyzer_JaccardDynamic(int buckets, int depth)
:  m_bucketcount(buckets)
,  m_historyDepth(depth)
,  m_leastUsedBucketUsed(0)
,  m_lastTimeBucketUsed(0)
,  m_maxleastBucketUsed(0)
,  m_avgleadBucketUsed(0.0)
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

LogDistributerAnalyzer_JaccardDynamic::~LogDistributerAnalyzer_JaccardDynamic()
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

int LogDistributerAnalyzer_JaccardDynamic::getBucket(const std::string& input)
{
    int best_index = -1;
    int index = 0;
    list< set<string>* >* best_location = NULL;
    float best_value = threshold; // we only want to consiter the bucket if it is better then the min threshold

    // if all the buckets have already been used, it would be best to place the entry
    // in whatever bucket it would work best in.
    if( m_leastUsedBucketUsed >= m_bucketcount )
        best_value = 0.0;

    set<string>* set_input = new set<string>();
    makeParsedSet(input, set_input);

    vector< list< set<string>* >* >::iterator it;
    for( it = mp_history->begin(), index = 0; it != mp_history->end(); it++, index++ )
    {
        float calculated_jaccard = averageJaccardInColumn( *it, set_input );
        if( calculated_jaccard > best_value )
        {
            best_index = index;
            best_value = calculated_jaccard;
            best_location = *it;
        }
    }

    // this if statment is used if not all the buckets have been filled or if
    // no similarity is detected.  So similartiy detected should rarely happen.
    if( best_location == NULL ){
          best_index = m_leastUsedBucketUsed % mp_history->size();
          best_location = mp_history->at( best_index );

          ++m_leastUsedBucketUsed;
    }

    // after the two loops, we should have the bucket to place the set

    //mp_last_time_used->at(best_index) = ++m_stringsDistributed;
    best_location->push_front( set_input );

    // remove the last element in the history if it exists, to keep the number of records maintanable
    while(best_location->size() > m_historyDepth)
    {
        delete best_location->back();
        best_location->pop_back();
    }

    return best_index;
}

int LogDistributerAnalyzer_JaccardDynamic::getBucket(const char* str)
{
    return this->getBucket(string(str));
}

/*
returns the average jaccard distance of a column.  The Jaccard distance is calculated for each value
in the set compared to the input
*/
float LogDistributerAnalyzer_JaccardDynamic::averageJaccardInColumn(const list< set<string>* >* input, const set<string>* compare)
{
    float average;
    float best;

    JaccardStatsInColumn(input, compare, average, best);
    return average;
}

float LogDistributerAnalyzer_JaccardDynamic::bestJaccardInColumn(const list< set<string>* >* input, const set<string>* compare)
{
    float average;
    float best;

    JaccardStatsInColumn(input, compare, average, best);
    return best;
}

void LogDistributerAnalyzer_JaccardDynamic::JaccardStatsInColumn(const std::list< std::set<std::string>* >* input,
                                                                    const std::set<std::string>* compare,
                                                                    float &average,
                                                                    float &best)
{
    int count = 0;
    average = 0;
    best    = 0;

    list< set<string>* >::const_iterator it = input->begin();
    for(; it != input->end(); it++)
    {
        float tmp = Jaccard(*(*(it)), *compare);
        average += tmp;
        best = best > tmp ? best : tmp;
        count++;
    }

    average = count == 0 ? 0.0 : average / count;
}

void LogDistributerAnalyzer_JaccardDynamic::makeParsedSet(const std::string& input, set<std::string>* output) const
{
    output->clear();

    unsigned startidx = 0;
    unsigned endidx = 0;

    startidx = 0;
    endidx   = input.find_first_of(seperators, startidx+1);
    while(endidx < input.size())
    {
        // no use inserting a string with zero length
        if(endidx >= startidx + 1)
        {
            output->insert(input.substr(startidx, endidx - startidx ));
        }

        startidx = endidx+1;
        endidx = input.find_first_of(seperators, startidx+1);
    }
}
