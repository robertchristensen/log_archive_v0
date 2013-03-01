#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <cstring>

#include <bzlib.h>

#include "../include/LogDistributer.h"
#include "../include/LogUnArchiver.h"
#include "../include/LogMerge.h"

using namespace std;

int INPUT_BUFFER_SIZE = 200000;
int DECOMPRESS = 0;
int BUCKETS = 8;
LogDistributer::DistributerType DISTRIBUTER = LogDistributer::JACCARD_COMBINE;
bool quite = false;


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
    }
}


int main(int argc, char **argv)
{
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
            logger->insert(inputBuffer, size);
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
