#ifndef CHARACTERCOUNT_H
#define CHARACTERCOUNT_H

#include <string>

struct characterCount
{
    public:
        characterCount();
        characterCount(const std::string str);
        characterCount(const char* str);
        virtual ~characterCount();

        void add(const characterCount& c);
        void subtract(const characterCount& c);

        void increase(unsigned char i, short value=1);
        void decrease(unsigned char i, short value=1);

        int  linearDifference(const characterCount& c) const;

        // I will use short just cause I feel like it is safe to use this size
        short m_buckets[sizeof(unsigned char) << 8];
    protected:
    private:
        int abs(int i);
};

#endif // CHARACTERCOUNT_H
