#include "../include/LogDistributerAnalyzer_EditDistance.h"

#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <climits>
#include <cmath>
#include <limits>

#include <inttypes.h>

using namespace std;

/*
This method uses edit distance to try to find the best bucket to place the next log record.
The algorithm works this way:
1) find the bucket where the last element inserted has a smallest edit distance with the current element
     (if the edit distance is larger then 1/3 the string, it does not count as a valid compairson for this portion)
2) If no bucket was found that has a close match, find the least recently used bucket to place the string into.
*/

LogDistributerAnalyzer_EditDistance::LogDistributerAnalyzer_EditDistance(int buckets)
: m_buckets(buckets),
  m_stringsDistributed(0)
{
    mp_last_bucket = new vector<std::string>(m_buckets);
    mp_last_time_used = new vector<int64_t>(m_buckets);
}

LogDistributerAnalyzer_EditDistance::~LogDistributerAnalyzer_EditDistance()
{
    delete mp_last_bucket;
}

int LogDistributerAnalyzer_EditDistance::getBucket(const std::string& str)
{
    int best_index = -1;
    int best_value = INT_MAX;
    int maximum_acceptance_distance_b = (int) ceil(str.size() * CONSISTANCY_THRESHOLD);

    for(int i = 0; i < mp_last_bucket->size(); i++)
    {
        int maximum_acceptance_distance_a = (int) ceil(mp_last_bucket->at(i).size() * CONSISTANCY_THRESHOLD);

        int size_difference = mp_last_bucket->at(i).size() - str.size();
        if(size_difference < 0)
            size_difference *= -1;

        if(size_difference > maximum_acceptance_distance_a || size_difference > maximum_acceptance_distance_b)
            continue;

        int temp_edit_distace = getEditDistance(str, mp_last_bucket->at(i));

        if(temp_edit_distace <= maximum_acceptance_distance_a && temp_edit_distace <= maximum_acceptance_distance_b && temp_edit_distace < best_value)
        {
            best_value = temp_edit_distace;
            best_index = i;
        }
    }

    // if it was able to find a best value within the accepted edit distance, go with that one, otherwise,
    // we have to search for the least recently used bucket to replace
    if(best_index < 0)
    {
        int64_t leasedValue = numeric_limits<int64_t>::max();
        for(int i = 0; i < mp_last_time_used->size(); i++)
        {
            if(mp_last_time_used->at(i) < leasedValue)
            {
                leasedValue = mp_last_time_used->at(i);
                best_index = i;
            }
        }
    }

    // after the two loops, we should have a bucket to place the string

    // update the internal memory and string value
    mp_last_time_used->at(best_index) = ++m_stringsDistributed;
    mp_last_bucket->at(best_index)    = str;


    return best_index;
}

int LogDistributerAnalyzer_EditDistance::getBucket(const char* str)
{
    return this->getBucket(string(str));
}

int LogDistributerAnalyzer_EditDistance::getEditDistance(const char* str_a, const char* str_b)
{
    unsigned int str_a_size = strlen(str_a);
    unsigned int str_b_size = strlen(str_b);

	if(str_a_size == 0)
		return str_b_size;
	if(str_b_size == 0)
		return str_a_size;

	vector<vector<int> > *editDis = new vector<vector<int> >(2, vector<int>(str_b_size+1));

	vector<int>* editPrev = &editDis->at(0);
	vector<int>* editNext = &editDis->at(1);

	for(unsigned j=1; j < editPrev->size(); j++)
		editPrev->at(j) = j;

	for(unsigned i=1; i < str_a_size+1; i++)
	{
		editNext->at(0) = i;
		for(unsigned j=1; j < str_b_size+1; j++)
		{
			int insertVal = editPrev->at(j) + 1;
			int deleteVal = editNext->at(j - 1) + 1;
			int replaceVal = editPrev->at(j - 1) + (str_b[j-1] != str_b[i-1]);
			editNext->at(j) = min(insertVal, min(deleteVal, replaceVal));
		}
		swap(editPrev, editNext);
	}
	int distance = editPrev->at(editPrev->size() - 1);

	delete editDis;
	return distance;
}

// this algorithm implementation comes from my homework solution to CS5150 -- Advanced Algorithms
// solves the edit distance problem using dynamic programming.  Returns only the value of the edit distance
int LogDistributerAnalyzer_EditDistance::getEditDistance(const std::string& A, const std::string& B)
{
	if(A.size() == 0)
		return B.size();
	if(B.size() == 0)
		return A.size();

	vector<vector<int> > *editDis = new vector<vector<int> >(2, vector<int>(B.size()+1));

	vector<int>* editPrev = &editDis->at(0);
	vector<int>* editNext = &editDis->at(1);

	for(unsigned j=1; j < editPrev->size(); j++)
		editPrev->at(j) = j;

	for(unsigned i=1; i < A.size()+1; i++)
	{
		editNext->at(0) = i;
		for(unsigned j=1; j < B.size()+1; j++)
		{
			int insertVal = editPrev->at(j) + 1;
			int deleteVal = editNext->at(j - 1) + 1;
			int replaceVal = editPrev->at(j - 1) + (B[j-1] != A[i-1]);
			editNext->at(j) = min(insertVal, min(deleteVal, replaceVal));
		}
		swap(editPrev, editNext);
	}
	int distance = editPrev->at(editPrev->size() - 1);

	delete editDis;
	return distance;
}
