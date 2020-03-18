#include "DDSelector.h"
#include "../Settings.h"

#include <vector>

using std::vector;


DDSelectorCut::DDSelectorCut()
  : Cut("DDSelectorCut")
{}

void DDSelectorCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    int tDiff = d.s2AftT1s[index] - d.s2AftT0s[index];
    if(d.s1s[index] > 100 or
       d.drifts[index] < 60 or d.drifts[index] > 80
       or d.rs[index] > 17){
      d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
