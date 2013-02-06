#ifndef LOGDISTRIBUTER_H
#define LOGDISTRIBUTER_H

#include <inttypes.h>
#include "../include/LogArchiver.h"
#include "../include/LogDistributerAnalyzer.h"

class LogDistributer
{
public:
    enum DistributerType { ROUND_ROBIN, EDIT_DISTANCE, JACCARD };


    LogDistributer(int num_archivers, DistributerType distrib = ROUND_ROBIN);
    virtual ~LogDistributer();

    int insert(char *, int size);

    int close();
protected:
private:
    LogArchiver **mp_archivers;
    LogDistributerAnalyzer *mp_analyzer;
    int m_numberOfArchivers;

    int64_t m_index;

    bool m_closed;
};

#endif // LOGDISTRIBUTER_H
