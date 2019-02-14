#include "BadArea.h"
#include "../Settings.h"

#include <vector>
#include <math.h>

using std::vector;


BadAreaCut::BadAreaCut()
  : Cut("BadAreaCut")
{}

void BadAreaCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if((d.goodAreas[index] <= 253 and d.badAreas[index] > 80)
       or (253 < d.goodAreas[index] and d.goodAreas[index] <= 10000
           and d.badAreas[index] > 80*pow(d.goodAreas[index]*pow(10, -2.4),
                                          4.668) )
       or (d.goodAreas[index] > 10000
           and d.badAreas[index] > pow(10,
                                       .7004*log10(d.goodAreas[index])
                                       - .1073))){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
