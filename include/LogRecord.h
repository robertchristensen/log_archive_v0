#ifndef LOGRECORD_H
#define LOGRECORD_H

#include <string>
#include <inttypes.h>

/*
A LogRecord is given responsibility
*/
struct LogRecord
{
    public:
        LogRecord(const char* str, int size);

        ~LogRecord();

        char* m_str;
        void* extra;
        int m_str_size;
    private:
        //char* m_back_str;
};

#endif // LOGRECORD_H
