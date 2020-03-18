#include "LuxstampRange.h"
#include "../Settings.h"
#include "../utility/StandardPlots.h"
#include "../utility/PlotUtils.h"

#include <vector>
#include <math.h>

#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"

using std::vector;
using std::string;


LuxstampRangeCut::LuxstampRangeCut()
  : Cut("LuxstampRangeCut")
{}

void LuxstampRangeCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.luxstamps[index] >= 13417740114865221 and
       d.luxstamps[index] <= 13420693915509349
       or
       d.luxstamps[index] >= 13416788213758865 and
       d.luxstamps[index] <= 13417725956242816
       or
       d.luxstamps[index] >= 15764394103529565 and
       d.luxstamps[index] <= 15766869016455991
       ){
      d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}


void LuxstampRangeCut::plot(dataContainer &d, string plotPrefix){
  plotStandards(d, plotPrefix);
}

  

