#ifndef ARCHIVER_GROUP_H
#define ARCHIVER_GROUP_H


class Archiver_Group
{
    public:
        Archiver_Group();
        virtual ~Archiver_Group();


    protected:
        unsigned char data_indx;
        unsigned char length_indx;

        unsigned char header;
        unsigned char bitmap[4];
        unsigned char data[32];
        unsigned char lengths[32];

        unsigned char buffer_data;
        unsigned char buffer_length;
    private:

};

#endif // ARCHIVER_GROUP_H
