#include "../include/LogRecord.h"

#include <string>
#include <cstring>

using namespace std;

LogRecord::LogRecord(const char* str, int size)
: extra(NULL),
  m_str_size(size)
{
    // allocate enough memory so the string can be copied to the buffer
    // (include enough space for an extra null character in case the original
    //  string is not null terminated)
    m_str = new char[size+1];
    //m_str = m_back_str;

    // copy the string and make sure it is null terminated by adding a null
    // character at the end of the string.  a null character should appear
    // before this.  In other words, the common case will be to have two
    // null characters at the end of the string
    strncpy(m_str, str, size);
    m_str[size] = '\0';
}

LogRecord::~LogRecord()
{
    delete []m_str;
}
