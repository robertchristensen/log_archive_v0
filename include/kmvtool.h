#ifndef KMVTOOL
#define KMVTOOL
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
#define KEYSIZE 17
#define AESHASH string
using namespace std;

class kmvtool
{
 public:
  static uint readHex(const char* s);
  static void Char2Hex(unsigned char ch, char* szHex);
  static void Hex2Char(char const* szHex, unsigned char& rch);
  static void CharStr2HexStr(unsigned char const* pucCharStr, char* pszHexStr, int iSize);
  static void HexStr2CharStr(char const* pszHexStr, unsigned char* pucCharStr, int iSize);
  static void padding(string & str);
  static void myreverse(char s[]);
  static void myitoa(int n, char s[]);
  static void genstr(char * str, uint size);
  static void printAES(map<AESHASH,string> & UK);
};
#endif
