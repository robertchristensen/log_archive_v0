#include "../include/SetTools.h"

#include <set>
#include <string>

using namespace std;

float Jaccard(const set<string> &A, const set<string> &B)
{
    int count_int, count_uni;
    union_intersection_size(A, B, count_int, count_uni);
    return ( (float) count_int) / ( (float) count_uni);
}

// this function assumes the set is sorted and it is a set (each element only appears once)
int union_size(const set<string> &A, const set<string> &B)
{
    int count, junk;
    union_intersection_size(A, B, junk, count);
    return count;
}

int intersection_size(const set<string> &A, const set<string> &B)
{
    int count, junk;
    union_intersection_size(A, B, count, junk);
    return count;
}

void union_intersection_size(const set<string> &A, const set<string> &B, int &count_intersect, int &count_union)
{
    count_intersect = 0;
    count_union     = 0;
    set<string>::iterator A_it = A.begin();
    set<string>::iterator B_it = B.begin();

    while( A_it != A.end() && B_it != B.end() ) {
        ++count_union;
        if( *A_it == *B_it ) {
            ++count_intersect;
            ++A_it;
            ++B_it;
        } else if( *A_it < *B_it ) {
            ++A_it;
        } else {
            ++B_it;
        }
    }

    // cleanup, we know one of the iterater
    while( A_it != A.end() )
    {
        ++count_union;
        ++A_it;
    }
    while( B_it != B.end() )
    {
        ++count_union;
        ++B_it;
    }
}
