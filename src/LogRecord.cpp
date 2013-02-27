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
{
    m_str.assign(str);

    return m_str.size();
}

int LogRecord::ChangeRecord(const string& str)
{
    m_str.assign(str, 0, string::npos);

    return m_str.size();
}

const char* LogRecord::getLogRecord() const
{
    return m_str.c_str();
}

int LogRecord::getLogRecordSize() const
{
    return m_str.size();
}
