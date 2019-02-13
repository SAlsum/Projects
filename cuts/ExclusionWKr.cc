#include "ExclusionWKr.h"
#include "../Settings.h"

#include <vector>


using std::vector;


ExclusionCut::ExclusionCut()
  : Cut("ExclusionCut")
{
}

void ExclusionCut::performCut(dataContainer &d){
    vector<int> newKeepIndex;
    for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
      if(d.excludeds[d.keepIndex[aaEvent]]){
        d.cutMask[d.keepIndex[aaEvent]] = true;
      }
      else{newKeepIndex.push_back(d.keepIndex[aaEvent]);}
    }
    d.keepIndex = newKeepIndex;
  }
