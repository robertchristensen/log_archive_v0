#include "../include/BZIP_lineBuffer.h"

#include <cstring>
#include <string>
#include <iostream>
#include <inttypes.h>

#include <bzlib.h>

using namespace std;

BZIP_lineBuffer::BZIP_lineBuffer(BZFILE* inputBuffer)
: mp_bzip2File(inputBuffer),
  m_index(0)
{
    m_bufferSize = INPUT_BUFFER_SIZE;
    m_inputbuffer = new char[m_bufferSize];

    m_available = BZ2_bzRead(&m_errorCode, mp_bzip2File, m_inputbuffer, m_bufferSize);
}

BZIP_lineBuffer::~BZIP_lineBuffer()
{
    delete []m_inputbuffer;
}

void BZIP_lineBuffer::readData( void )
{
    // move existing data to the front of the buffer
    memmove(m_inputbuffer, m_inputbuffer + m_index, m_bufferSize - m_index);

    // fill in the rest of the buffer with uncompressed data
    if(m_errorCode == BZ_OK)
    {
        m_available = BZ2_bzRead(&m_errorCode, mp_bzip2File, m_inputbuffer + m_bufferSize - m_index, m_index) + m_bufferSize - m_index;
        m_index = 0;
    }
}

int BZIP_lineBuffer::get_error( )
{
    return m_errorCode;
}

// return non zero if the operation could not be done
int BZIP_lineBuffer::ReadLine( string& str )
{
    //int retVal = 0;

    // first, see if a newline can be found in the current buffer, without resizing
    //int length = strcspn(m_inputbuffer + m_index, "\n");

    int length = sizeof(int64_t);  // skip over the first few bytes so we don't have problems bby parsing the index value

    for(; length + m_index <= m_available && m_inputbuffer[m_index+length] != '\n'; length++);



    // if it did not finish reading the entire buffer
    if(length < m_available - m_index)
    {
        ++length; // so the end line will be included in the message
        str.assign(m_inputbuffer + m_index, length);
        m_index += length;
        return 0;
    }

    if(m_errorCode == BZ_STREAM_END)
        return -1;

    // otherwise, the buffer must be refreshed
    // first we consume as many bytes as we can, so we don't have to reread those bytes.
    str.assign(m_inputbuffer + m_index, m_available - m_index);
    m_index = m_available;

    do
    {
      if( m_errorCode != BZ_OK )
      {
        cerr << "ERROR, unable to find correct end of buffer in BZIP_lineBuffer" << endl;
        return -1;
      }

      this->readData( );

      length = 0;
      // if the string buffer does not compleatly contain the header value

      if((int) sizeof(int64_t) - (int) str.size() < 0)
        length = 0;
      else
        length = (int) sizeof(int64_t) - (int) str.size();
      for( ; length + m_index <= m_available && m_inputbuffer[m_index+length] != '\n'; length++);

    if(length < m_available - m_index)
        ++length; // so the end line will be included in the message

      str.append(m_inputbuffer + m_index, length);
      m_index += length;

    } while(m_index >= m_available);

    return 0;
}
