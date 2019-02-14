#include "S1Range.h"
#include "../Settings.h"

#include <vector>

using std::vector;


S1RangeCut::S1RangeCut()
  : Cut("S1RangeCut")
{}

void S1RangeCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.s1s[index] < 1 or d.s1s[index] > 300){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
