#include "../include/LogArchiver.h"
#include "../include/ThreadSafeQueue.h"
#include "../include/LogRecord.h"
#include <bzlib.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <thread>

using namespace std;

int LogArchiver::instances = 0;
int LogArchiver::blockSize = 9;

LogArchiver::LogArchiver(int instance)
: m_instance(instance),
  closed(false)
{
    ++instances;

    int returnValue;
    char fileName[16];
    snprintf(fileName, 16, "log%04d.txt.bz2", m_instance);

    mp_rawFileOut = fopen(fileName, "w");
    if(mp_rawFileOut == NULL)
    {
        cerr << "ERROR opening file " << fileName << " to write!" << endl;
        closed = true;
        return;
    }

    mp_bzip2File = BZ2_bzWriteOpen(&returnValue, mp_rawFileOut, blockSize, verbosity, workFactor);
    if(returnValue != BZ_OK)
    {
        cerr << "ERROR creating archiver";
        fclose(mp_rawFileOut);
        closed = true;
        return;
    }

    mp_insertQueue = new ThreadSafeQueue(DEFAULT_QUEUE_SIZE);
    mp_thisWorker = new thread(ThreadStart, this);
}

LogArchiver::LogArchiver(char *name)
: m_instance(),
  closed(false)
{
    ++instances;

    int returnValue;
    char fileName[16];
    snprintf(fileName, 16, "%s.bz2", name);

    mp_rawFileOut = fopen(fileName, "w");
    if(mp_rawFileOut == NULL)
    {
        cerr << "ERROR opening file " << fileName << " to write!" << endl;
        closed = true;
        return;
    }

    mp_bzip2File = BZ2_bzWriteOpen(&returnValue, mp_rawFileOut, blockSize, verbosity, workFactor);
    if(returnValue != BZ_OK)
    {
        cerr << "ERROR creating archiver";
        fclose(mp_rawFileOut);
        closed = true;
        return;
    }

    mp_insertQueue = new ThreadSafeQueue(DEFAULT_QUEUE_SIZE);
    mp_thisWorker = new thread(ThreadStart, this);
}

LogArchiver::~LogArchiver()
{
    if(closed != true)
        this->close();

     delete mp_insertQueue;
     delete mp_thisWorker;
}

void LogArchiver::ThreadStart(LogArchiver* i)
{
    i->ThreadLoop();
    return;
}


void LogArchiver::ThreadLoop()
{
    void* from_queue;
    int returnValue;
    LogRecord* data;
    while((from_queue = this->mp_insertQueue->pop_front( )) != NULL)
    {
        // get the record to insert into the archive
        data = (LogRecord*)from_queue;
        // insert the record into the archive.  NOTE: for backwards
        // compatibility I don't do locking!
        BZ2_bzWrite(&returnValue, mp_bzip2File, data->m_str, data->m_str_size);

        // clean up the memory used by the data.  The data is deleted.
        delete data;
        data = NULL;
    }
}


int LogArchiver::close()
{
    int returnValue;
    if(closed == true)
        return 0;

    // tell the thread to end (by writing null to the thread buffer)
    mp_insertQueue->push( NULL );
    // wait for the thread to die
    mp_thisWorker->join();

    BZ2_bzWriteClose( &returnValue, mp_bzip2File, 0, NULL, NULL );

    fclose(mp_rawFileOut);
    closed = true;

    return returnValue;
}
bool LogArchiver::isClosed()
{
    return closed;
}

int LogArchiver::InsertRecord(char *str, int size)
{
    int returnValue;
    // each string should end in a new line character
    BZ2_bzWrite(&returnValue, mp_bzip2File, str, size);

    return returnValue;
}

int LogArchiver::InsertRecord(std::string *str)
{
    int returnValue;

    BZ2_bzWrite(&returnValue, mp_bzip2File, &str[0], str->size());

    return returnValue;
}

int LogArchiver::setBlockSize(int value)
{
    if( instances != 0 ||value < 0 || value > 9 )
        return blockSize;
    blockSize = value;

    return blockSize;
}

int LogArchiver::getBlockSize()
{
    return blockSize;
}

void LogArchiver::BackgroundInsertRecord(char *str, int size)
{
    if(!closed)
        mp_insertQueue->push(new LogRecord(str, size));
}

void LogArchiver::BackgroundInsertRecord(std::string *str)
{
    if(!closed)
        mp_insertQueue->push(new LogRecord(str->c_str(), str->size()));
}

void LogArchiver::BackgroundInsertRecord(LogRecord *data)
{
    if(!closed)
        mp_insertQueue->push(data);
}
