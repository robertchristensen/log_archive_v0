#include "../include/LogDistributerAnalyzer_JaccardSimilarity.h"
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

char* LogDistributerAnalyzer_JaccardSimilarity::seperators = " \t\n:.\\/@[]();\"&=-";

LogDistributerAnalyzer_JaccardSimilarity::LogDistributerAnalyzer_JaccardSimilarity(int buckets)
:  m_stringsDistributed(0)
,  m_bucketcount(buckets)
,  m_leastUsedBucketUsed(0)
,  m_lastTimeBucketUsed(0)
,  m_maxleastBucketUsed(0)
,  m_avgleadBucketUsed(0.0)
{
    mp_history = new vector< list< set<string>* >* >(m_bucketcount);

    vector< list< set<string>* >* >::iterator it;
    for(it = mp_history->begin(); it != mp_history->end(); ++it)
    {
        *it = new list< set<string>* >();
    }
}

LogDistributerAnalyzer_JaccardSimilarity::~LogDistributerAnalyzer_JaccardSimilarity()
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

int LogDistributerAnalyzer_JaccardSimilarity::getBucket(const std::string& input)
{
    int best_index = -1;
    int index = 0;
    list< set<string>* >* best_location = NULL;
    float best_value = threshold; // we only want to consiter the bucket if it is better then the min threshold

    // if all the buckets have already been used, it would be best to place the entry
    // in whatever bucket it would work best in.
    if( m_leastUsedBucketUsed > m_bucketcount )
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

    if( best_location == NULL ){
          best_location = mp_history->at(m_leastUsedBucketUsed);
          best_index = m_leastUsedBucketUsed;

          ++m_leastUsedBucketUsed;
    }
//        cerr << "                                                                                      \r";
//        printf("lrb: %6d at %8d since %5d max: %5d avg: %f\r"
//               , ++m_leastUsedBucketUsed
//               , (int)m_stringsDistributed
//               , (int)m_stringsDistributed - m_lastTimeBucketUsed
//               , m_maxleastBucketUsed
//               , (float) m_stringsDistributed / (float) m_leastUsedBucketUsed);

//        if(m_stringsDistributed - m_lastTimeBucketUsed > m_maxleastBucketUsed)
//            m_maxleastBucketUsed = m_stringsDistributed - m_lastTimeBucketUsed;
//        m_lastTimeBucketUsed = m_stringsDistributed;

//        int64_t leasedValue = mp_last_time_used->at(0);
//        best_location = mp_history->at(0);
//        best_index = 0;
//        for(int i = 1; i < mp_last_time_used->size(); i++){
//            if( mp_last_time_used->at(i) < leasedValue ){
//                leasedValue = mp_last_time_used->at(i);
//                best_location = mp_history->at(i);
//                best_index = i;
//            }
//        }
    //}

    // after the two loops, we should have the bucket to place the set

    //mp_last_time_used->at(best_index) = ++m_stringsDistributed;
    best_location->push_front( set_input );

    // remove the last element in the history if it exists, to keep the number of records maintanable
    while(best_location->size() > history_depth)
    {
        delete best_location->back();
        best_location->pop_back();
    }

    //cerr << "best jaccard: " << best_value <<;
    cerr << "bucket: " << best_index << endl;
    return best_index;
}

int LogDistributerAnalyzer_JaccardSimilarity::getBucket(const char* str)
{
    return this->getBucket(string(str));
}

/*
returns the average jaccard distance of a column.  The Jaccard distance is calculated for each value
in the set compared to the input
*/
float LogDistributerAnalyzer_JaccardSimilarity::averageJaccardInColumn(const list< set<string>* >* input, const set<string>* compare)
{
    float average;
    float best;

    JaccardStatsInColumn(input, compare, average, best);
    return average;
}

float LogDistributerAnalyzer_JaccardSimilarity::bestJaccardInColumn(const list< set<string>* >* input, const set<string>* compare)
{
    float average;
    float best;

    JaccardStatsInColumn(input, compare, average, best);
    return best;
}

void LogDistributerAnalyzer_JaccardSimilarity::JaccardStatsInColumn(const std::list< std::set<std::string>* >* input,
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

void LogDistributerAnalyzer_JaccardSimilarity::makeParsedSet(const std::string& input, set<std::string>* output) const
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
