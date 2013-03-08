#ifndef LOGARCHIVER_H
#define LOGARCHIVER_H

#include <bzlib.h>
#include <cstdlib>
#include <string>
#include <thread>
#include <mutex>

#include "../include/ThreadSafeQueue.h"
#include "../include/LogRecord.h"


class LogArchiver
{
public:
    LogArchiver(int instance);
    LogArchiver(char* name);
    virtual ~LogArchiver();

    int InsertRecord(char *str, int size);
    int InsertRecord(std::string *str);

    void BackgroundInsertRecord(char *str, int size);
    void BackgroundInsertRecord(std::string *str);
    void BackgroundInsertRecord(LogRecord *data);

    int close();

    bool isClosed();

    // sets the block size only if no instances of LogArchiver have been created
    static int setBlockSize(int value);
    static int getBlockSize();

    static void ThreadStart(LogArchiver*);
    void ThreadLoop();
protected:

private:
    int m_instance;
    FILE* mp_rawFileOut;
    BZFILE* mp_bzip2File;

    // values queued to be inserted by the running thread for this archiver
    ThreadSafeQueue* mp_insertQueue;
    std::thread* mp_thisWorker;

    bool closed;

    static int instances;
    static int blockSize;
    const static int verbosity = 0;
    const static int workFactor = 30;

    // default max number of elements the queue can hold
    const static int DEFAULT_QUEUE_SIZE = 2000;
};

#endif // LOGARCHIVER_H
