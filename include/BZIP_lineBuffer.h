#ifndef BZIP_LINEBUFFER_H
#define BZIP_LINEBUFFER_H

#include <string>
#include <bzlib.h>

extern int INPUT_BUFFER_SIZE;

class BZIP_lineBuffer
{
    public:
        BZIP_lineBuffer(BZFILE* inputBuffer);
        virtual ~BZIP_lineBuffer();

        int ReadLine( std::string& str );
        int ReadByte( char& val);

        int get_error( );
    protected:
    private:
        void readData( void );

        BZFILE* mp_bzip2File;
        char *m_inputbuffer;
        int m_bufferSize;
        int m_index;
        int m_errorCode;
        int m_available;
};

#endif // BZIP_LINEBUFFER_H
