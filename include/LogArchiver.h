#ifndef LOGARCHIVER_H
#define LOGARCHIVER_H

#include <bzlib.h>
#include <cstdlib>
#include <string>


class LogArchiver
{
public:
    LogArchiver(int instance);
    virtual ~LogArchiver();

    int InsertRecord(char *str, int size);
    int InsertRecord(std::string *str);

    int close();

    bool isClosed();

    // sets the block size only if no instances of LogArchiver have been created
    static int setBlockSize(int value);
    static int getBlockSize();
protected:

private:
    int m_instance;
    FILE* mp_rawFileOut;
    BZFILE* mp_bzip2File;

    bool closed;

    static int instances;
    static int blockSize;
    const static int verbosity = 0;
    const static int workFactor = 30;
};

#endif // LOGARCHIVER_H
