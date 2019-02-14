#include "S2Shape.h"
#include "../Settings.h"

#include <vector>

using std::vector;


S2ShapeCut::S2ShapeCut()
  : Cut("S2ShapeCut")
{}

void S2ShapeCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.s2GausSigmas[index] < 35
       or (d.s2AftT1s[index] - d.s2AftT0s[index])/d.s2GausSigmas[index]> 2.5){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
