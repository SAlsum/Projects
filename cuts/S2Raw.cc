#include "S2Raw.h"
#include "../Settings.h"

#include <vector>

using std::vector;


S2RawCut::S2RawCut()
  : Cut("S2RawCut")
{}

void S2RawCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.s2Raws[index] < 200){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
