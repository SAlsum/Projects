#include "PromptFraction.h"
#include "../Settings.h"

#include <vector>
#include <math.h>

using std::vector;


PromptFractionCut::PromptFractionCut()
  : Cut("PromptFractionCut")
{}

void PromptFractionCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(
       (d.s1s[index] <= 47 and
        -2.311319979e-7 * pow(d.s1s[index], 4)
        + 3.19166994e-5 * pow(d.s1s[index], 3)
        - 1.63849839e-3 * pow(d.s1s[index], 2)
        + 3.88637498e-2 * d.s1s[index]
        + .377155251
        > d.promptFractions[index])
       or
       (d.s1s[index] > 47 and
        (.5066 + .0001311 * d.s1s[index])
         * (1.0 - exp(-d.s1s[index]/8.875))
         + .26
        > d.promptFractions[index])
){
      d.cutMask[index] = true;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}
