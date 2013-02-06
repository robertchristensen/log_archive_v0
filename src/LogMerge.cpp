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

    // create a heap to maintain sorted order of the archives.
    if(m_sources.size() == 0)
        return; // nothing to do

    make_heap(m_sources.begin(), m_sources.end(), LogUnArchiver::compareGreater);
}

LogMerge::~LogMerge()
{
    for(size_t i=0; i<m_sources.size(); i++)
        delete m_sources[i];
}

bool LogMerge::getNextRecord_string(std::string &str)
{
    int retval;
    str.clear();
    // if it can't get any reconrds:
    if(m_sources.size() == 0)
        return false;

    // pull the top value from the vector.
    str.assign(m_sources[0]->get_CurrentRecord()->getLogRecord());

    // pop the element to be back of the heap
    pop_heap(m_sources.begin(), m_sources.end(), LogUnArchiver::compareGreater);

    // update the last element with a new string value
    retval = m_sources.back()->pop( );

    if(retval != 0)
    {
        delete m_sources.back();
        m_sources.pop_back();
        return true;
    }
    else
    {
        // reinsert the last element into the heap, maintaining the sorted structure
        push_heap(m_sources.begin(), m_sources.end(), LogUnArchiver::compareGreater);
        return true;
    }
}
