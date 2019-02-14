#include "S2Range.h"
#include "../Settings.h"

#include <vector>

using std::vector;


S2RangeCut::S2RangeCut()
  : Cut("S2RangeCut")
{}

void S2RangeCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.s2s[index] < 200 or d.s2s[index] > 31600){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
