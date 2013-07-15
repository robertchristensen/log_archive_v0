#ifndef LOGDISTRIBUTERANALYZER_H
#define LOGDISTRIBUTERANALYZER_H

#include <string>

#include "../include/LogRecord.h"

// this class is an abstract class for used in the LogDistributer in order to seperate different log entries into
// seperate bins for archiving
class LogDistributerAnalyzer
{
public:
    virtual ~LogDistributerAnalyzer( ) { };

    // requests what bucket an item should be placed when
    virtual int getBucket(const std::string&) = 0;
    virtual int getBucket(const char*) = 0;

    // prebuild will setup information used for threadedgetbucket.  This was
    // added to be an additional step in the pipeline, so more threads could
    // be doing something when it came time to do something
    virtual void* preBuild(const char*) = 0;

    // threaded get bucket is meant to be a threaded version of the getBucket function
    // call.  the object passed into this function contians the data structure
    // created by the preBuild step.
    // be defualt, it just calls the single threaded get bucket function.
    //virtual int ThreadedgetBucket(void*) = 0;
    virtual int ThreadedgetBucket(LogRecord* rec)
    {
        return getBucket(rec->m_str);
    }
private:

};

#endif
