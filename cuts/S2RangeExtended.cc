#include "S2RangeExtended.h"
#include "../Settings.h"

#include <vector>

using std::vector;


S2RangeExtendedCut::S2RangeExtendedCut()
  : Cut("S2RangeExtendedCut")
{}

void S2RangeExtendedCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.s2s[index] < 100 or d.s2s[index] > 38000){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
