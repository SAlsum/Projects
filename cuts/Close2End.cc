#include "Close2End.h"
#include "../Settings.h"

#include <vector>

using std::vector;


Close2EndCut::Close2EndCut()
  : Cut("Close2EndCut")
{}

void Close2EndCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.close2Ends[index] > 0){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
