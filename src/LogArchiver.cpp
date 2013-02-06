#include "../include/LogArchiver.h"
#include <bzlib.h>
#include <cstdlib>
#include <string>
#include <iostream>

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
    snprintf(fileName, 16, "log%04d.txt.bz", m_instance);

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
}

LogArchiver::~LogArchiver()
{
    if(closed != true)
        this->close();
}

int LogArchiver::close()
{
    int returnValue;
    if(closed == true)
        return 0;

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
