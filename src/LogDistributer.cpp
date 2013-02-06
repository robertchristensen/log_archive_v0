#include "../include/LogDistributer.h"
#include "../include/LogArchiver.h"
#include "../include/LogDistributerAnalyzer.h"
#include "../include/LogDistributerAnalyzer_RoundRobin.h"
#include "../include/LogDistributerAnalyzer_EditDistance.h"
#include "../include/LogDistributerAnalyzer_JaccardSimilarity.h"

#include <cstring>

LogDistributer::LogDistributer(int num_archivers, DistributerType distrib)
: m_numberOfArchivers(num_archivers),
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
            mp_analyzer = new LogDistributerAnalyzer_JaccardSimilarity(num_archivers);
            break;
        default:
            mp_analyzer = new LogDistributerAnalyzer_RoundRobin(num_archivers);
    }
    mp_archivers = new LogArchiver*[m_numberOfArchivers];
    for(int i=0; i<m_numberOfArchivers; i++)
        mp_archivers[i] = new LogArchiver(i);
}

LogDistributer::~LogDistributer()
{
    if(m_closed != true)
        this->close();

    for(int i=0; i<m_numberOfArchivers; i++)
        delete mp_archivers[i];

    delete []mp_archivers;
    delete mp_analyzer;
}

int LogDistributer::close()
{
    if(m_closed == true)
        return 0;

    m_closed = true;
    for(int i=0; i<m_numberOfArchivers; i++)
        mp_archivers[i]->close();

    return 0;
}

int LogDistributer::insert(char *str, int size)
{
    // TODO: it would increase the speed of the algorithm to not dynamically allocate memmory
    //char *tmp_str = new char[sizeof(m_index) + size];

    // find string similarity before repacking
    int bucket = mp_analyzer->getBucket(str);

    // insert index at the begining of the array, then place the string at the end
    //*((int64_t*) tmp_str) = m_index++;
    //memcpy(tmp_str + sizeof(m_index), str, size);

    //int retVal = mp_archivers[bucket]->InsertRecord(tmp_str, size + sizeof(m_index));
    int retVal = mp_archivers[bucket]->InsertRecord(str, size);

    //delete []tmp_str;

    return retVal;
}
