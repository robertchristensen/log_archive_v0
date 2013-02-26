#ifndef KMV
#define KMV
#include <stdio.h>
#include <stdlib.h>
#include "Rijndael.h"
#include <iostream>
#include <vector>
#include <map>
#include <time.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <string.h>
#define AESHASH string
#define KEYSIZE 17
#define M 4147483648
#include "kmvtool.h"
#include "sha1.h"
using namespace std;

class kmv
{
 public:
  uint k;
  uint Distinct_Size;
  uint DV;
  char AESkey[KEYSIZE];
  map<AESHASH,string> U;
  map<AESHASH,uint> UK;
  CRijndael oRijndael;
  SHA1 sha;
 public:
  kmv(){};
  kmv(int size);
  kmv & operator = (const kmv & a);
  void buildkmv(vector<string> & V);
  void printDV();
  void printD();

  static void DVunion(const vector<kmv> & KV, kmv & objkmv);
  static void DVunion(const vector<kmv*> & KV, kmv & objkmv);

  static uint DVintersection(vector<kmv>  & KV);
  static uint DVintersection(const vector<kmv*> & KV);

  // added by Robert Christensen
  static float DVJaccardEst(const vector<kmv*> & KV);

  static void AFMVintersection(vector<kmv>  & KV, kmv & objkmv);
  static void AFMVintersection(vector<kmv*> & KV, kmv & objkmv);

  static uint computeDV(AESHASH uk, uint & kv);
  static void printAKMV(map<AESHASH,uint> & H);
  static uint IntersectCountDistinct(vector< vector<string> > & kset);
};

#endif
