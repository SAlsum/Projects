#include "KrExclusion.h"
#include "../Settings.h"

#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>

using std::vector;
using std::string;
using std::cout;
using std::endl;


KrExclusionCut::KrExclusionCut()
  : Cut("KrExclusionCut")
{}

void KrExclusionCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(checkRange(d.luxstamps[index])){
      d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}


void KrExclusionCut::loadInjections(string filename){
  unsigned long long int theStamp;
  unsigned long long int theEnd;
  std::ifstream infile(filename);
  while(infile >> theStamp >> theEnd){
    mInjections.push_back(theStamp);
    mEnds.push_back(theEnd);
  }
  return;
}


bool KrExclusionCut::checkRange(unsigned long long int stamp){
  for(int aaInj = 0; aaInj < mInjections.size(); aaInj++){
    if(/*aaStamp < stamp and
         stamp < aaStamp + mHalflifeSamples*/
       mNewEnds.push_back(mEnds[aaInj] - mHalflifeSamples);
       mInjections[aaInj] < stamp and
       stamp < mNewEnds[aaInj]){
      return true;
    }
  }
  return false;
}


void KrExclusionCut::setHalflives(float nHalflives){
  mHalflives = nHalflives;
  mHalflifeSamples = nHalflives * 6.588 * pow(10,11);
}


double KrExclusionCut::timeExcluded(){
  unsigned long long int totalSamplesExcluded = 0;
  for(int aaInj = 0; aaInj < mInjections.size(); aaInj++){
    if(mNewEnds[aaInj] <= mInjections[aaInj]){
      cout << "problem: allowing entire injeciton.  (Injection " << aaInj
           << ")" << endl;
    }
    totalSamplesExcluded += (mNewEnds[aaInj] - mInjections[aaInj]);
  }
  return (double)totalSamplesExcluded * pow((double)10, -8);
}
