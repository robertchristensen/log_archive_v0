#ifndef SETTOOLS_H
#define SETTOOLS_H

#include <set>
#include <string>

float Jaccard(const std::set<std::string> &A, const std::set<std::string> &B);

// this function assumes the sets are sorted
int union_size(const std::set<std::string> &A, const std::set<std::string> &B);

int intersection_size(const std::set<std::string> &A, const std::set<std::string> &B);

void union_intersection_size(const std::set<std::string> &A, const std::set<std::string> &B, int &count_intersect, int &count_union);


#endif // SETTOOLS_H
