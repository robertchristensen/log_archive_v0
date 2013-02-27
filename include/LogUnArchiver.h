#ifndef LOGUNARCHIVER_H
#define LOGUNARCHIVER_H

#include <cstdlib>
#include <iostream>
#include <bzlib.h>

#include "../include/BZIP_lineBuffer.h"
#include "../include/LogRecord.h"

extern int INPUT_BUFFER_SIZE;

class LogUnArchiver
{
    public:
        LogUnArchiver(int);
        virtual ~LogUnArchiver();

        bool isClosed();
        int close();

        const LogRecord* get_CurrentRecord();
        int pop( );
        bool isAvailable( );

        //static bool compareLess(const LogUnArchiver* a, const LogUnArchiver* b);
        //static bool compareGreater(const LogUnArchiver* a, const LogUnArchiver* b);

    protected:
    private:
        static int instances;

        FILE* mp_rawFileOut;
        BZFILE* mp_bzip2File;

        BZIP_lineBuffer* mp_lineBuffer;
        LogRecord*       mp_currentRecord;

        int m_instance;
        bool m_closed;
        bool m_available;

        const static int small = 0;
        const static int verbosity = 0;
};

#endif // LOGUNARCHIVER_H
