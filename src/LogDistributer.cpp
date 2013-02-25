#include "../include/LogDistributer.h"
#include "../include/LogArchiver.h"
#include "../include/LogDistributerAnalyzer.h"
#include "../include/LogDistributerAnalyzer_RoundRobin.h"
#include "../include/LogDistributerAnalyzer_EditDistance.h"
#include "../include/LogDistributerAnalyzer_JaccardSimilarity.h"
#include "../include/LogDistributerAnalyzer_characterSimilarity.h"

#include <cstring>
#include <vector>

using namespace std;

int LogDistributer::JACCARD_HISTORY = -1;

LogDistributer::LogDistributer(int num_archivers, DistributerType distrib)
: //m_numberOfArchivers(num_archivers),
  m_index(0),
  m_closed(0)
{
    switch(distrib)
    {
         case ROUND_ROBIN:
            mp_analyzer = new LogDistributerAnalyzer_RoundRobin(num_archivers);
            break;
        case EDIT_DISTANCE:
            mp_analyzer = new LogDistributerAnalyzer_EditDistance(num_archivers);
            break;
        case JACCARD:
            mp_analyzer = new LogDistributerAnalyzer_JaccardSimilarity(num_archivers, JACCARD_HISTORY);
            break;
        case CHAR:
            mp_analyzer = new LogDistributerAnalyzer_characterSimilarity(num_archivers, JACCARD_HISTORY);
            break;
        default:
            mp_analyzer = new LogDistributerAnalyzer_RoundRobin(num_archivers);
    }
    mp_archivers = new vector<LogArchiver*>( );
    mp_index = new LogArchiver("index");
}

LogDistributer::~LogDistributer()
{
    if(m_closed != true)
        this->close();

    vector<LogArchiver*>::iterator it = mp_archivers->begin();
    for( ; it != mp_archivers->end() ; it++ )
        delete *it;

    delete mp_archivers;
    delete mp_analyzer;
    delete mp_index;
}

int LogDistributer::close()
{
    if(m_closed == true)
        return 0;

    m_closed = true;
    vector<LogArchiver*>::iterator it = mp_archivers->begin();
    for( ; it != mp_archivers->end() ; it++)
        (*it)->close();

    return 0;
}

int LogDistributer::insert(char *str, int size)
{
    //const int index_size = 5;

    // TODO: it would increase the speed of the algorithm to not dynamically allocate memmory
    //char *tmp_str = new char[index_size + size];

    // find string similarity before repacking
    int bucket = mp_analyzer->getBucket(str);
    char b = (char) bucket;

    // this will happen when we need to add a new bucket.
    if(bucket == mp_archivers->size())
    {
        mp_archivers->push_back(new LogArchiver(bucket));
    }

    // using string values to record the record number:
    // we start by forcing the number of digits to be 8


    // insert index at the begining of the array, then place the string at the end
    //snprintf(tmp_str, index_size+1, "%05d", m_index++);
    //memcpy(tmp_str + index_size, str, size);

    //int retVal = mp_archivers->at(bucket)->InsertRecord(tmp_str, size + index_size);
    int retVal = mp_archivers->at(bucket)->InsertRecord(str, size);

    mp_index->InsertRecord( &b, 1 );

    //delete []tmp_str;

    return retVal;
}
