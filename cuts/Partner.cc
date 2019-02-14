#include "Partner.h"
#include "../Settings.h"

#include <vector>

using std::vector;


PartnerCut::PartnerCut()
  : Cut("PartnerCut")
{}

void PartnerCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.partners[index] > 0){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
