#include "Selection.h"
#include "../Settings.h"

#include <vector>

using std::vector;


SelectionCut::SelectionCut()
  : Cut("SelectionCut")
{}

void SelectionCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    int tDiff = d.s2AftT1s[index] - d.s2AftT0s[index];
    if((d.rs[index] < 15.75 or d.rs[index] > 15.95)){
      d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
