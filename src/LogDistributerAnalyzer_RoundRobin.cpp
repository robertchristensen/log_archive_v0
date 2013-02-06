#include "../include/LogDistributerAnalyzer_RoundRobin.h"

/*
This method distributes each log entry into each bucket in a round robin way.
*/
LogDistributerAnalyzer_RoundRobin::LogDistributerAnalyzer_RoundRobin(int number_of_buckets)
: m_buckets(number_of_buckets)
, m_index(0)
{
    // nothing to do here
}

LogDistributerAnalyzer_RoundRobin::~LogDistributerAnalyzer_RoundRobin()
{
    // nothing to do here
}

int LogDistributerAnalyzer_RoundRobin::getBucket(const std::string&)
{
    return getNextIndex();
}

int LogDistributerAnalyzer_RoundRobin::getBucket(const char*)
{
    return getNextIndex();
}

int LogDistributerAnalyzer_RoundRobin::getNextIndex()
{
    int toRet = m_index;
    m_index = (m_index + 1) % m_buckets;
    return toRet;
}
