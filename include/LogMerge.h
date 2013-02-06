#ifndef LOGMERGE_H
#define LOGMERGE_H

#include <iostream>
#include <vector>
#include <string>

#include "../include/LogUnArchiver.h"

class LogMerge
{
    public:
        LogMerge(std::string filePrefix);
        LogMerge();
        virtual ~LogMerge();

        bool getNextRecord_string(std::string &);
    protected:
    private:
        std::vector<LogUnArchiver*> m_sources;

};

#endif // LOGMERGE_H
