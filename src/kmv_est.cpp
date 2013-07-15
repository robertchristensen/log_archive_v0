#include "../include/kmv_est.h"

#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <cstring>
#include <limits>

#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

using namespace std;

kmv_est::kmv_est(const char* str, int k, int q_gram_length)
{
    m_kmv_syn = new vector<uint64_t>( );
    mp_hashstruct = (SHA_CTX*) malloc( sizeof(SHA_CTX) );

    unordered_set<uint64_t> contained;

    int strLen = strlen(str); // can optomize here
    int end_idx = strLen - q_gram_length;
    uint64_t max_value_in_set = 0; // the max value in the k min value estimator list
    for(int i = 0; i < end_idx; i++)
    {
        uint64_t val = hash(str+i, q_gram_length);

        if(!(contained.count(val) > 0)) // this is true if the value does not already exists in the set
        {
            if( m_kmv_syn->size() < k )
            {
                int cap1 = m_kmv_syn->capacity();
                m_kmv_syn->push_back(val);
                int cap2 = m_kmv_syn->capacity();
                push_heap(m_kmv_syn->begin(), m_kmv_syn->end());

                // insert the value into the hash set
                contained.insert(val);
                max_value_in_set = m_kmv_syn->front();
            }else if( val < max_value_in_set )
            {
                // insert the new element into kmv
                m_kmv_syn->push_back(val);
                push_heap(m_kmv_syn->begin(), m_kmv_syn->end());

                // remove the largest element in kmv from
                pop_heap(m_kmv_syn->begin(), m_kmv_syn->end());
                contained.erase(m_kmv_syn->back()); // the last element in the vector will be the largest value.  remove this value from the containes set
                m_kmv_syn->pop_back();

                // insert the new value into the hash set
                contained.insert(val);
                max_value_in_set = m_kmv_syn->front();
            }
        }
    }

    // sort the elements of m_kmv_syn (so it is sorted, not a heap anymore)
    sort_heap(m_kmv_syn->begin(), m_kmv_syn->end());
}

kmv_est::~kmv_est()
{
    free(mp_hashstruct);
    delete m_kmv_syn;
}


// get the estimation of the number of distinct values in the set
int kmv_est::get_DV()
{
    // use double.  I don't know if accuricy will be affected by using a float
    double U = (0.0 + m_kmv_syn->back()) / (sizeof(uint64_t) << 8);
    return (int) (m_kmv_syn->size() - 1) / U;
}

// get the estimation of the number of distinct values in the
// intersection of this and the other set without creating
// a new set
int kmv_est::intersection_DV(const kmv_est *val)
{
    int int_DV;
    int un_DV;
    float jac_est;
    combine_DV(val, int_DV, un_DV, jac_est);

    return int_DV;
}

// get the estimation of the number of distinct values in the
// union of this set and the other set without creating a
// new set
int kmv_est::union_DV(const kmv_est *val)
{
    int int_DV;
    int un_DV;
    float jac_est;
    combine_DV(val, int_DV, un_DV, jac_est);

    return un_DV;
}

// estimate teh jaccard value between this set and the
// set being passed in.
float kmv_est::jaccard_est(const kmv_est *val) const
{
    int int_DV;
    int un_DV;
    float jac_est;
    combine_DV(val, int_DV, un_DV, jac_est);

    return jac_est;
}

// gets the estimation of the number of distinct values in the
// intersection and union between this set and the set
// passed in without creating a new set.
void kmv_est::combine_DV(const kmv_est *val, int &intersection_DV, int &union_DV, float &jaccard_est) const
{
    // get k = min(k_a, k_b)
    int min_k = min( this->m_kmv_syn->size(), val->m_kmv_syn->size() );

    // if the number of elemetns in the estimation is too small, no use
    // trying to calculate everything.  Just say it is invalid.
    if(min_k < 1)
    {
        intersection_DV = 0;
        union_DV        = 0;
        jaccard_est     = 0;
        return;
    }

    uint64_t last_hash;
    int current = 0;
    int intersection_count = 0;
    vector<uint64_t>::iterator it_a = this->m_kmv_syn->begin();
    vector<uint64_t>::iterator it_b =  val->m_kmv_syn->begin();
    // do the operation L_a (+) L_b. In order to calculate the union
    // estimation, I only need the value of U(k), and k.  I throw
    // away all other values.
    while(current < min_k)
    {
        current++;
        if(*it_a < *it_b)
        {
            last_hash = *it_a;
            it_a++;
        }
        else if(*it_a > *it_b)
        {
            last_hash = *it_b;
            it_b++;
        }
        else
        {
            intersection_count++;
            last_hash = *it_b;
            it_a++;
            it_b++;
        }
    }

    // first, normalize the value of U(k)
    double U        = (0.0 + last_hash) / (numeric_limits<uint64_t>::max());
    union_DV        = (min_k - 1) / U;
    jaccard_est     = (float) intersection_count / min_k;
    intersection_DV = jaccard_est * union_DV;
}

uint64_t kmv_est::hash(const char* str, int len)
{
    static unsigned char data[SHA_DIGEST_LENGTH];
    uint64_t* r_val = (uint64_t*) &data;

    SHA1_Init( mp_hashstruct );
    SHA1_Update(mp_hashstruct, str, len);
    SHA1_Final(data, mp_hashstruct);

    return *r_val;
}
