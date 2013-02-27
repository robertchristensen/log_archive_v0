#ifndef LOGMERGE_H
#define LOGMERGE_H

#include <iostream>
#include <vector>
#include <string>

#include "../include/LogUnArchiver.h"
#include "../include/BZIP_lineBuffer.h"

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

        BZIP_lineBuffer *m_index_buffer;
        FILE* mp_rawFileOut;
        BZFILE* mp_bzip2File;
};

#endif // LOGMERGE_H
