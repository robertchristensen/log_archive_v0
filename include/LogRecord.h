#ifndef LOGRECORD_H
#define LOGRECORD_H

#include <string>
#include <inttypes.h>

class LogRecord
{
    public:
        LogRecord( );
        LogRecord(const char* str);
        LogRecord(const std::string* str);
        virtual ~LogRecord();

        int ChangeRecord(const char* str);
        int ChangeRecord(const std::string& str);


        //int64_t getIndexNumber() const;

        //std::string getLogRecordString();

        const char* getLogRecord() const;
        int getLogRecordSize() const;

        static bool compareLess(const LogRecord &, const LogRecord &);
        static bool compareGreater(const LogRecord &, const LogRecord &);
    protected:
    private:
        //int64_t m_indexValue;
        std::string m_str;
};

#endif // LOGRECORD_H
