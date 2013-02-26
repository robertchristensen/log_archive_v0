#include "../include/kmv.h"
kmv::kmv(int size)
  : k(size),
    DV(0)
{
  memset(AESkey,0,KEYSIZE);
  strcat(AESkey,"1234567890123456");
  oRijndael.MakeKey(AESkey, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
}

void kmv::buildkmv(vector<string> & V)
{
  /*  if(V.size()<k)
    {
      cerr<<"Data size < K."<<endl;
      exit(-1);
      }*/
  // buildkmv builds the kmv from scratch.  All data structures should be
  // empty
 // U.clear();
 // UK.clear();
 // k = max_k;

  char *char_arry=NULL;
  char szHex[5]="";

  for(uint i=0;i<V.size();i++)
    {
      unsigned message_digest[5];
      string str="";
      string str1=V[i];
      char *mssge=new char[41];
      memset(mssge,0,41);

      sha.Reset();
      sha << str1.data();
      if (!sha.Result(message_digest))
	{
	  cerr << "ERROR-- could not compute message digest" << endl;
	  exit(-1);
	}
      else
	{
	  kmvtool::CharStr2HexStr((unsigned char*)message_digest, mssge, 16);
	}
      str=mssge;
      kmvtool::padding(str);
      char_arry=new char[str.size()+1];
      memset(char_arry,0,str.size()+1);
      //      cout<<str<<"|"<<endl<<flush;
      //      getchar();
      oRijndael.Encrypt(str.data(), char_arry, str.size(), CRijndael::CBC);
      oRijndael.ResetChain();
      kmvtool::CharStr2HexStr((unsigned char*)char_arry, szHex, 4);
      if(U.find(string(szHex))==U.end())
	{
	  U[szHex]=str;//str;
	  UK[szHex]=1;//str;
	  if(UK.size()>k)
	    {
	      map<AESHASH,uint>::iterator it=UK.end();
	      UK.erase(--it);
	    }
	}
      else
	{
	  if(UK.find(szHex)!=UK.end())
	    UK[szHex]++;
	}
      delete[] mssge;
      delete[] char_arry;
    }
  map<AESHASH,uint>::iterator it=UK.end();
  k=UK.size();
  DV=(unsigned int)(((double)k-1)/((double)kmvtool::readHex((*(--it)).first.data())/M));
  Distinct_Size=U.size();
  //  U.clear();
}

void kmv::printDV()
{
  cout<<DV<<endl;
}

void kmv::printD()
{
  cout<<Distinct_Size<<endl;
}

void kmv::DVunion(const vector<kmv> & KV, kmv & N)
{
  uint NewK=KV[0].k;
  map<AESHASH,uint> NewU;
  map<AESHASH,uint>::iterator it;
  for(uint i=0;i<KV.size();i++)
    {
      if(NewK>KV[i].k)
	NewK=KV[i].k;
      NewU.insert(KV[i].UK.begin(),KV[i].UK.end());
    }
  N.k=NewK;
  uint counter=NewK;
  it=NewU.begin();
  while(counter)
    {
      counter--;
      it++;
    }
  N.UK.insert(NewU.begin(),it);
  N.DV=kmv::computeDV((*(--it)).first,NewK);
  N.Distinct_Size=0;
}

void kmv::DVunion(const vector<kmv*> & KV, kmv & N)
{
  uint NewK=KV[0]->k;
  map<AESHASH,uint> NewU;
  map<AESHASH,uint>::iterator it;
  for(uint i=0;i<KV.size();i++)
    {
      if(NewK>KV[i]->k)
	NewK=KV[i]->k;
      NewU.insert(KV[i]->UK.begin(),KV[i]->UK.end());
    }
  N.k=NewK;
  uint counter=NewK;
  it=NewU.begin();
  while(counter)
    {
      counter--;
      it++;
    }
  N.UK.insert(NewU.begin(),it);
  N.DV=kmv::computeDV((*(--it)).first,NewK);
  N.Distinct_Size=0;
}

uint kmv::DVintersection(vector<kmv> & KV)
{
  kmv N(0), UUnion(0);
  DVunion(KV, UUnion);
  uint K = UUnion.k;
  uint size = KV.size();
  map<AESHASH,uint> UIntersect;
  map<AESHASH,uint>::iterator vis;
  vector<AESHASH> tmp;
  vector<AESHASH>::iterator it, cur;
  for(uint i=0;i<size;i++)
    {
      for(vis=KV[i].UK.begin();vis!=KV[i].UK.end();++vis)
	tmp.push_back((*vis).first);
    }
  /*for(vis=UUnion.UK.begin(); vis!=UUnion.UK.end();++vis)
    {
      tmp.push_back((*vis).first);
      }*/
  sort(tmp.begin(),tmp.end());
  for(it=tmp.begin(), cur=it; it!=tmp.end(); ++it)
    {
      if((*it)!=(*cur))
	{
	  if(it-cur==size)
	    UIntersect[*cur]=0;
	  cur=it;
	}
    }
  if(it-cur==size)
    UIntersect[*cur]=0;

  return (uint)((double(UIntersect.size())/(double)K)*UUnion.DV);
}

uint kmv::DVintersection(const vector<kmv*> & KV)
{
  kmv N(0), UUnion(0);
  DVunion(KV, UUnion);
  uint K = UUnion.k;
  uint size = KV.size();
  map<AESHASH,uint> UIntersect;
  map<AESHASH,uint>::iterator vis;
  vector<AESHASH> tmp;
  vector<AESHASH>::iterator it, cur;
  for(uint i=0;i<size;i++)
    {
      for(vis = KV[i]->UK.begin() ; vis!=KV[i]->UK.end() ; ++vis)
	tmp.push_back((*vis).first);
    }
  /*for(vis=UUnion.UK.begin(); vis!=UUnion.UK.end();++vis)
    {
      tmp.push_back((*vis).first);
      }*/
  sort(tmp.begin(), tmp.end());
  for(it = tmp.begin(), cur=it; it!=tmp.end(); ++it)
    {
      if((*it)!=(*cur))
	{
	  if(it-cur==size)
	    UIntersect[*cur]=0;
	  cur=it;
	}
    }
  if(it-cur==size)
    UIntersect[*cur]=0;

  return (uint)((double(UIntersect.size())/(double)K)*UUnion.DV);
  //return (float) ((float) UIntersect.size()) / ((float) K) * UUnion.DV;
}

float kmv::DVJaccardEst(const vector<kmv*> & KV)
{
  kmv UUnion(0);
  DVunion(KV, UUnion);
  uint K = UUnion.k;
  uint size = KV.size();
  map<AESHASH,uint> UIntersect;
  map<AESHASH,uint>::iterator vis;
  vector<AESHASH> tmp;
  vector<AESHASH>::iterator it, cur;
  for(uint i=0;i<size;i++)
  {
      for(vis = KV[i]->UK.begin() ; vis!=KV[i]->UK.end() ; ++vis)
	    tmp.push_back((*vis).first);
  }
  /*for(vis=UUnion.UK.begin(); vis!=UUnion.UK.end();++vis)
    {
      tmp.push_back((*vis).first);
      }*/
  sort(tmp.begin(), tmp.end());
  for(it = tmp.begin(), cur=it; it!=tmp.end(); ++it)
  {
    if((*it)!=(*cur))
	{
	  if(it-cur==size)
	    UIntersect[*cur]=0;
	  cur=it;
	}
  }
  if(it-cur==size)
    UIntersect[*cur]=0;

  float Isize = (uint)((double(UIntersect.size())/(double)K)*UUnion.DV);

  //return (float) ((float) UIntersect.size()) / ((float) K) * UUnion.DV;
  return (Isize) / (UUnion.DV);
}

uint kmv::computeDV(AESHASH uk, uint & kv)
{
  return (unsigned int)(((double)kv-1)/((double)kmvtool::readHex(uk.data())/M));
}

uint kmv::IntersectCountDistinct(vector< vector<string> > & kset)
{
  uint count=0;
  vector<AESHASH> UH;
  vector<AESHASH>::iterator it, cur;
  map<AESHASH,string> MArry;
  map<AESHASH,string>::iterator vis;
  for(uint i=0;i<kset.size();i++)
    {
      MArry.clear();
      for(uint j=0;j<kset[i].size();j++)
	MArry[kset[i][j]]="";
      for(vis=MArry.begin();vis!=MArry.end();vis++)
	UH.push_back((*vis).first);
    }
  sort(UH.begin(),UH.end());
  for(it=UH.begin(), cur=it; it!=UH.end(); ++it)
    {
      if((*it)!=(*cur))
	{
	  //cout<<it-cur<<endl;
	  if(uint(it-cur)==kset.size())
	    count++;
	  cur=it;
	}
    }
  if(uint(it-cur)==kset.size())
    count++;
  return count;
}

void kmv::printAKMV(map<AESHASH,uint> & H)
{
  map<AESHASH,uint>::iterator vis;
  for(vis=H.begin();vis!=H.end();++vis)
    cout<<(*vis).first.data()<<" "<<(*vis).second<<endl;
}

void kmv::AFMVintersection(vector<kmv> & KV, kmv & objkmv)
{
  uint ke = 0;
  kmv tmp(0), AKMVSect(0);
  map<AESHASH,uint>::iterator vis;
  AKMVSect = KV.back();
  KV.pop_back();
  while(KV.size())
    {
      vector<kmv> kvec;
      kmv check(0);
      tmp=KV.back();
      KV.pop_back();
      kvec.push_back(AKMVSect);
      kvec.push_back(tmp);
      DVunion(kvec, check);
      for(vis = check.UK.begin() ; vis != check.UK.end() ; ++vis)
	{
	  if(tmp.UK.find((*vis).first) == tmp.UK.end()
          || AKMVSect.UK.find((*vis).first) == tmp.UK.end())
	    (*vis).second=0;
	  else
	    {
	      if(tmp.UK[(*vis).first] < AKMVSect.UK[(*vis).first])
		(*vis).second=tmp.UK[(*vis).first];
	      else
		(*vis).second = AKMVSect.UK[(*vis).first];
	      if(KV.size() == 0 && (*vis).second != 0)
		ke++;
	    }
	}//for
      AKMVSect = check;
    }//while
  objkmv = AKMVSect;
  objkmv.DV = uint((double)AKMVSect.DV*(double)ke/(double)objkmv.k);
}

kmv & kmv::operator = (const kmv & a)
{
  UK=a.UK;
  k=a.k;
  DV=a.DV;
  return *this;
}
