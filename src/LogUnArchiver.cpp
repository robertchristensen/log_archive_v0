#include "../include/LogUnArchiver.h"

#include <iostream>

using namespace std;

int LogUnArchiver::instances = 0;

LogUnArchiver::LogUnArchiver(int instance)
: mp_lineBuffer(0L)
, mp_currentRecord(0L)
, m_instance(instance)
, m_available(true)
, m_closed(false)
{
    ++instances;

    int returnValue;
    char fileName[16];
    snprintf(fileName, 16, "log%04d.log", m_instance);

    mp_rawFileOut = fopen(fileName, "r");
    if(mp_rawFileOut == NULL)
    {
        //cerr << "ERROR opening file " << fileName << "!" << endl;
        m_closed = true;
        m_available = false;
        return;
    }

    mp_bzip2File = BZ2_bzReadOpen(&returnValue, mp_rawFileOut, verbosity, small, NULL, 0);
    if(returnValue != BZ_OK)
    {
        cerr << "ERROR creating unarchiver";
        fclose(mp_rawFileOut);
        m_closed = true;
        return;
    }

    mp_lineBuffer = new BZIP_lineBuffer(mp_bzip2File);
    mp_currentRecord = new LogRecord( );

    // at the end we call pop so the current record is actaully referencing something
    this->pop( );
}

LogUnArchiver::~LogUnArchiver()
{
    if(m_closed != true)
        this->close();

    if(mp_lineBuffer == NULL)
        delete mp_lineBuffer;
    if(mp_currentRecord)
        delete mp_currentRecord;
}

bool LogUnArchiver::isClosed()
{
    return this->m_closed;
}

int LogUnArchiver::close()
{
    int returnValue;
    if(m_closed == true)
        return 0;

    BZ2_bzReadClose( &returnValue, mp_bzip2File );

    fclose(mp_rawFileOut);
    m_closed = true;
    m_available = false;

    return returnValue;
}

int LogUnArchiver::pop( )
{
    if(m_closed == true)
        return -1;

    string tmp;
    int retval = mp_lineBuffer->ReadLine(tmp);

    if(retval == 0)
        mp_currentRecord->ChangeRecord(tmp);
    else
        m_available = false;

    return retval;
}

const LogRecord* LogUnArchiver::get_CurrentRecord()
{
    return mp_currentRecord;
}

bool LogUnArchiver::compareLess(const LogUnArchiver* a, const LogUnArchiver* b)
{
    if(a == NULL)
        return false;
    if(b == NULL)
        return false;

    return LogRecord::compareLess(*(a->mp_currentRecord), *(b->mp_currentRecord));
}

bool LogUnArchiver::compareGreater(const LogUnArchiver* a, const LogUnArchiver* b)
{
    if(a == NULL)
        return false;
    if(b == NULL)
        return false;

    return LogRecord::compareGreater(*(a->mp_currentRecord), *(b->mp_currentRecord));
}

bool LogUnArchiver::isAvailable( )
{
    if(m_closed == true)
        return false;

    return m_available;
}
