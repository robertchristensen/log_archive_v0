#include <cstring>

#include "../include/characterCount.h"



#define ABSV(x) ( (x<0) ? -x : x )

characterCount::characterCount()
{
    // since m_buckets is created staticlly, this should be valid.
    memset(this->m_buckets, 0, sizeof(this->m_buckets));
}

characterCount::characterCount(const std::string str)
{
    memset(this->m_buckets, 0, sizeof(this->m_buckets));

    // note: this is not optimized yet
    int len = str.size();
    for(int i=0; i < len; i++)
    {
        this->increase(str[i]);
    }
}

characterCount::characterCount(const char* str)
{
    memset(this->m_buckets, 0, sizeof(this->m_buckets));

    // note: this is not opitimized yet
    int len = strlen(str);
    for(int i=0; i < len; i++)
    {
        this->increase(str[i]);
    }
}

characterCount::~characterCount()
{
    // nothing to do.  All memory is allocated statically
}

void characterCount::add(const characterCount& c)
{
    const short* from = &(c.m_buckets[0]);
    short* to   = &(this->m_buckets[0]);
    for(int i = 0; i < sizeof(char) << 8; i+=4)
    {
        *to     = *to + *from;
        *(to+1) = *(to+1) + *(from+1);
        *(to+2) = *(to+2) + *(from+2);
        *(to+3) = *(to+3) + *(from+3);

        from += 4;
        to   += 4;
    }
}

void characterCount::subtract(const characterCount& c)
{
    const short* from = &(c.m_buckets[0]);
    short* to   = &(this->m_buckets[0]);
    short* end  = &(this->m_buckets[sizeof(char) << 8]);
    for( ; to < end; )
    {
        *to     = *to     - *from;
        *(to+1) = *(to+1) - *(from+1);
        *(to+2) = *(to+2) - *(from+2);
        *(to+3) = *(to+3) - *(from+3);

        from += 4;
        to   += 4;
    }
}

void characterCount::increase(unsigned char i, short value)
{
    this->m_buckets[i] += value;
}

void characterCount::decrease(unsigned char i, short value)
{
    this->m_buckets[i] -= value;
}

int  characterCount::linearDifference(const characterCount& c) const
{
    int count1 = 0;
    int count2 = 0;
    int count3 = 0;
    int count4 = 0;
    int tmp1;
    int tmp2;
    int tmp3;
    int tmp4;

    const short* from = &(c.m_buckets[0]);
    const short* to   = &(this->m_buckets[0]);
    const short* end  = &(this->m_buckets[sizeof(char) << 8]);
    for( ; to < end; )
    {
        tmp1     = *to     - *from;
        tmp2 = *(to+1) - *(from+1);
        tmp3 = *(to+2) - *(from+2);
        tmp4 = *(to+3) - *(from+3);

        count1 += ABSV( tmp1 );
        count2 += ABSV( tmp2 );
        count3 += ABSV( tmp3 );
        count4 += ABSV( tmp4 );

        from += 4;
        to   += 4;
    }

    return ((count1 + count2) + (count3 + count4));
}


int characterCount::abs(int i)
{
    return (i < 0) ? -i : i;
}
