#include "Fiducial.h"
#include "../Settings.h"

#include <vector>

using std::vector;


FiducialCut::FiducialCut()
  : Cut("FiducialCut")
{}

void FiducialCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.drifts[index] < 40 or d.drifts[index] > 300
       or d.rWalls[index] - d.rs[index] < 3){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
