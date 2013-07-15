#ifndef KMV_EST_H
#define KMV_EST_H

#include <string>
#include <vector>
#include <inttypes.h>

#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/sha.h>


/*
this will do the disticint value estimation found in the paper
"On Synopses for Distinct-Value Estimation Under Multiset Operations"
[Kevin Beyer et. al.]

A few specifics about the implementation:
 * This uses hash values that are 64 bit.
 * Hash function can easily be changed, but we currently use the first 64 bits
     of the md4 hash function for speed of calculation.
 * This creates the set from a character string, grabs q-grams from the string
     and inserts the values into the estimator.
*/
class kmv_est
{
    public:
        kmv_est(const char* str, int k = 60, int q_gram_length = 6);
        virtual ~kmv_est();

        // get the estimation of the number of distinct values in the set
        int get_DV();

        // get the estimation of the number of distinct values in the
        // intersection of this and the other set without creating
        // a new set
        int intersection_DV(const kmv_est *val);

        // get the estimation of the number of distinct values in the
        // union of this set and the other set without creating a
        // new set
        int union_DV(const kmv_est *val);

        // estimate teh jaccard value between this set and the
        // set being passed in.
        float jaccard_est(const kmv_est *val) const;

        // gets the estimation of the number of distinct values in the
        // intersection and union between this set and the set
        // passed in without creating a new set.
        void combine_DV(const kmv_est *val, int &intersection_DV, int &union_DV, float &jaccard_est) const;

        // calculating and creating a set of the intersection or union
        // can be implemented later when it is found to be important.
    protected:
    private:
        std::vector<uint64_t> *m_kmv_syn;
        SHA_CTX* mp_hashstruct;  // for creating the object using the hash function

        uint64_t hash(const char* str, int len);
};

#endif // KMV_EST_H
