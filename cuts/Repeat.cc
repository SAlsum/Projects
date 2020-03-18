#include "Repeat.h"
#include "../Settings.h"
#include "../utility/StandardPlots.h"
#include "../utility/PlotUtils.h"

#include <vector>
#include <math.h>
#include <string>
#include <unordered_set>
#include <fstream>

#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"

using std::vector;
using std::string;
using std::unordered_set;


RepeatCut::RepeatCut()
  : Cut("RepeatCut")
{}

void RepeatCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(mStamps.count(d.luxstamps[index])){ //cut if present
      d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
    } else {
      mStamps.insert(d.luxstamps[index]);
      newKeepIndex.push_back(index);
    }
  }
  d.keepIndex = newKeepIndex;
}


void RepeatCut::plot(dataContainer &d, string plotPrefix){
  plotStandards(d, plotPrefix);
}


