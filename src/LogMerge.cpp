#include "../include/LogMerge.h"

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

LogMerge::LogMerge()
{
    m_sources.clear();
    int sources = 0;

    // search for a sequence of archive logs
    LogUnArchiver* tmpArch;
    do
    {
        tmpArch = new LogUnArchiver(sources++);
        if(!tmpArch->isClosed())
        {
            m_sources.push_back(tmpArch);
        }
        else
        {
            delete tmpArch;
            break;
        }
    } while(!tmpArch->isClosed());

    // create the object that will read the index
    const char* fileName = "index.bz2";

    mp_rawFileOut = fopen(fileName, "r");
    if(mp_rawFileOut == NULL)
    {
        cerr << "ERROR opening index file " << fileName << "!" << endl;
        //m_closed = true;
        //m_available = false;
        return;
    }

    int returnValue;
    mp_bzip2File = BZ2_bzReadOpen(&returnValue, mp_rawFileOut, 0, 0, NULL, 0);
    if(returnValue != BZ_OK)
    {
        cerr << "ERROR creating unarchiver";
        fclose(mp_rawFileOut);
        //m_closed = true;
        return;
    }
    m_index_buffer = new BZIP_lineBuffer(mp_bzip2File);
}

LogMerge::~LogMerge()
{
    for(size_t i=0; i<m_sources.size(); i++)
        delete m_sources[i];

    delete m_index_buffer;

    int returnValue;
    BZ2_bzReadClose( &returnValue, mp_bzip2File );
    fclose(mp_rawFileOut);
}

// return true if it was able to get a record
bool LogMerge::getNextRecord_string(std::string &str)
{
    int retval;
    str.clear();

    // get the bucket we will be taking the next record from.
    char bucket;
    retval = m_index_buffer->ReadByte(bucket);
    if(retval != 0)
        return false;

    //str = m_sources[bucket]->get_CurrentRecord( )->getLogRecord( );
    m_sources[bucket]->pop( );

    return true;
}
