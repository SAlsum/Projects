#include "S1RangeExtended.h"
#include "../Settings.h"

#include <vector>

using std::vector;


S1RangeExtendedCut::S1RangeExtendedCut()
  : Cut("S1RangeExtendedCut")
{}

void S1RangeExtendedCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.s1s[index] < 0.5 or d.s1s[index] > 350){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
