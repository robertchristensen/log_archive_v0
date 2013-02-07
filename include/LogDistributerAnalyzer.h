#ifndef LOGDISTRIBUTERANALYZER_H
#define LOGDISTRIBUTERANALYZER_H

#include <string>

// this class is an abstract class for used in the LogDistributer in order to seperate different log entries into
// seperate bins for archiving
class LogDistributerAnalyzer
{
public:
    virtual ~LogDistributerAnalyzer( ) { };

    virtual int getBucket(const std::string&) = 0;
    virtual int getBucket(const char*) = 0;
private:

};

#endif
