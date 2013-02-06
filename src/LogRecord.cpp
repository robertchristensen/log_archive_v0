#include "../include/LogRecord.h"

#include <string>

using namespace std;

LogRecord::LogRecord(const char* str)
{
    this->ChangeRecord(str);
}

LogRecord::LogRecord(const string* str)
{
    this->ChangeRecord(*str);
}

LogRecord::LogRecord( )
{
    // nothing to do
}

LogRecord::~LogRecord()
{
}

int LogRecord::ChangeRecord(const char* str)
{    // currently, the index value is the first few bytes of the string, fixed length
    m_indexValue = *((int64_t*) str);

    //m_str = (const char*)(str + sizeof(int64_t));
    m_str.assign((str + sizeof(int64_t)));

    return m_str.size();
}

int LogRecord::ChangeRecord(const string& str)
{
    m_indexValue = *((int64_t*) str.c_str());

    m_str.assign(str, sizeof(int64_t), string::npos);

    return m_str.size();
}

int64_t LogRecord::getIndexNumber() const
{
    return m_indexValue;
}

const char* LogRecord::getLogRecord() const
{
    return m_str.c_str();
}

int LogRecord::getLogRecordSize() const
{
    return m_str.size();
}

bool LogRecord::compareLess(const LogRecord &a, const LogRecord &b)
{
    return a.m_indexValue < b.m_indexValue;
}

bool LogRecord::compareGreater(const LogRecord &a, const LogRecord &b)
{
    return a.m_indexValue > b.m_indexValue;
}
