#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <cstring>
#include <execinfo.h>
#include <signal.h>

#include <bzlib.h>

#include "../include/LogDistributer.h"
#include "../include/LogUnArchiver.h"
#include "../include/LogMerge.h"

using namespace std;

int INPUT_BUFFER_SIZE = 200000;
int DECOMPRESS = 0;
int BUCKETS = 8;
LogDistributer::DistributerType DISTRIBUTER = LogDistributer::JAC_EST;
bool quite = false;

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(1);
}

void printHelp()
{
    std::cout << "input data will come from stdin.  when data is decompressed, it is written"
              << "to stdout\n\n"
              << "The following are single character arguments:\n"
              << " -q quite operations.  Print very little to the screen\n"
              << " -c compress data.  This will initialize the compressor algorithm\n"
              << " -d decompress data.  This will initalize the decompression algorithm\n"
              << " -[1-9] compression block size passed to the compressor (bzip2 in this case)\n\n"
              << "The followng are additional arguments which can be given which require a full name\n\n"
              << " --jhistory [value] the size of the sliding window history.  This value is only used if needed\n"
              << " --buckets [1-255] the number of buckets partition the data into\n"
              << " --qgram_size [value] the length of he qgram when qgrams are used\n"
              << " --max_k [value] the number of values to keep in the kmv estimator.  Used for jaccard estimation\n"
              << " --help display this message and exit\n"
              << "The following are similarity functions which can be used for the algorithm\n"
              << " --round_robin distribution where a round robin scheme is used\n"
              << " --edit_dist distribution where the bucket with the lowest edit distance is used\n"
              << " --jaccard distribution where set comparison between log record words is used\n"
              << " --jaccard_combine distribution similar to jaccard except history is averaged\n"
              << " --char distribution where character count similarity is used\n"
              << " --jaccard_qgram distribution where the set contains all qgrams of the log record\n"
              << " --est distribution where the jaccard value using qgrams is estimated\n\n"
              << "As a note, some of the distribution functions are now defunt.  As the development\n"
              << "of this project progressed, some were found to not be useful and were never updated\n"
              << "when changes were made"
              << endl;

}


void parseInput(int argc, char**argv)
{
    int i=1; // start at arguments
    while(i < argc)
    {
        if(argv[i][0] == '-' && argv[i][1] != '-')
        {
            int t=1;
            int length = strlen(argv[i]);
            while(t < length)
            {
                switch(argv[i][t])
                {
                    case 'c':
                        DECOMPRESS = 0;
                        break;
                    case 'd':
                        DECOMPRESS = 1;
                        break;
                    case 'q':
                        quite = true;
                        break;
                    case '1':
                        LogArchiver::setBlockSize(1);
                        break;
                    case '2':
                        LogArchiver::setBlockSize(2);
                        break;
                    case '3':
                        LogArchiver::setBlockSize(3);
                        break;
                    case '4':
                        LogArchiver::setBlockSize(4);
                        break;
                    case '5':
                        LogArchiver::setBlockSize(5);
                        break;
                    case '6':
                        LogArchiver::setBlockSize(6);
                        break;
                    case '7':
                        LogArchiver::setBlockSize(7);
                        break;
                    case '8':
                        LogArchiver::setBlockSize(8);
                        break;
                    case '9':
                        LogArchiver::setBlockSize(9);
                        break;
                    default:
                        cerr << "unknown option -" << argv[i][t] << endl;
                        break;
                }
                t++;
            }
            i++;
        }
        else if(strcmp("--help", argv[i]) == 0)
        {
            i++;
            cout << "printing help" << endl;
            printHelp();
            exit(0);
        }
        else if(strcmp("--round_robin", argv[i]) == 0)
        {
            i++;
            DISTRIBUTER = LogDistributer::ROUND_ROBIN;
            //cerr << "using round robin" << endl;
        }
        else if(strcmp("--edit_dist", argv[i]) == 0)
        {
            i++;
            DISTRIBUTER = LogDistributer::EDIT_DISTANCE;
            //cerr << "using edit distance" << endl;
        }
        else if(strcmp("--jaccard", argv[i]) == 0)
        {
            i++;
            DISTRIBUTER = LogDistributer::JACCARD;
            //cerr << "using jaccard distance" << endl;
        }
        else if(strcmp("--jaccard_combine", argv[i]) == 0)
        {
            i++;
            DISTRIBUTER = LogDistributer::JACCARD_COMBINE;
        }
        else if(strcmp("--char", argv[i]) == 0)
        {
            i++;
            DISTRIBUTER = LogDistributer::CHAR;
        }
        else if(strcmp("--est", argv[i]) == 0)
        {
            i++;
            DISTRIBUTER = LogDistributer::JAC_EST;
        }
        else if(strcmp("--jaccard_qgram", argv[i]) == 0)
        {
            i++;
            DISTRIBUTER = LogDistributer::JACCARD_QGRAM;
        }
        else if(strcmp("--jhistory", argv[i]) == 0)
        {
            i++;
            char *end;
            int x = strtol(argv[i], &end, 10);
            if(x > 0 && x < 1000)
                LogDistributer::JACCARD_HISTORY = x;
            else
                cerr << "ERROR: unable to parse the history depth for jaccard algorithm" << endl;
            i++;
        }
        else if(strcmp("--buckets", argv[i]) == 0)
        {
            i++;
            char *end;
            int x = strtol(argv[i], &end, 10);
            if(x > 0 && x < 1000)
                BUCKETS = x;
            else
                cerr << "ERROR: problem parsing the number of buckets requested" << endl;
            i++;
        }
        else if(strcmp("--qgram_size", argv[i]) == 0)
        {
            i++;
            char *end;
            int x = strtol(argv[i], &end, 10);
            if(x > 0 && x < 100)
                LogDistributer::Q_GRAM_LENGTH = x;
            else
                cerr << "ERROR: unable to parse the qgram length correctly" << endl;
            i++;
        }
        else if(strcmp("--max_k", argv[i]) == 0)
        {
            i++;
            char *end;
            int x = strtol(argv[i], &end, 10);
            if(x > 0 && x < 1000)
                LogDistributer::KMV_MAX_K = x;
            else
                cerr << "ERROR: unable to parse the value of max k correctly" << endl;
            i++;
        }
        else
        {
            cerr << "ERROR: unable to parse the argument \'" << argv[i] << "\'" << endl;
            exit(0);
        }
    }
}


int main(int argc, char **argv)
{
    signal(SIGSEGV, handler);   // install our handler
    parseInput(argc, argv);

    if(DECOMPRESS == false)
    {
        if(!quite)
            cerr << "starting compression" << endl;

        LogDistributer* logger = new LogDistributer(BUCKETS, DISTRIBUTER);

        // create input buffer for file stream
        char *inputBuffer = new char[INPUT_BUFFER_SIZE];

        int64_t totalLines = 0;

        if(!quite)
            cerr << "lines archived so far:" << endl;

        do
        {

            cin.getline(inputBuffer, INPUT_BUFFER_SIZE);
            int size = strlen(inputBuffer);
            if(cin.eof())
                continue;
            inputBuffer[size] = '\n';
            inputBuffer[++size] = '\0';

            ++totalLines;
            //logger->insert(inputBuffer, size);
            logger->Background_insert(inputBuffer, size);
            if(!quite && totalLines % 137 == 0)
                cerr << '\r' << totalLines;// << '\r';
        }while(!cin.eof());

        if(!quite)
            cerr << "Lines Archived: " << totalLines << endl;

        delete logger;
    }
    else
    {
        cerr << "doing decompression" << endl;

        LogMerge unlogger;
        string logMessage;

        while(unlogger.getNextRecord_string(logMessage))
            cout << logMessage;
    }

    cerr << "DONE" << endl;

    return 0;
}
