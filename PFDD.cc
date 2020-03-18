#include <string>
#include <vector>
#include <iostream>

#include "Settings.h"
#include "utility/LoadData.h"
#include "utility/HistInfo.h"
#include "cuts/Cut.h"
#include "cuts/PromptFraction.h"

#include "TStopwatch.h"

#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TColor.h"
#include "TLegend.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;




int main(int argc, char* argv[]){
  TStopwatch timer;
  timer.Start();

  // get the list of files to process from passed in args
  string DDFilename = "data/EFTData_post_DDSelectorCut.root";
  vector<string> DDFilenames;
  DDFilenames.push_back(DDFilename);
 

  // load in some data to be cut
  dataContainer dd = loadData(DDFilenames);

  
  int dBins = 25;
  int dMin = 1;
  int dMax = 100;
  int index;
  HistInfo preInfo(dMin, dMax, dBins);
  HistInfo postInfo(dMin, dMax, dBins);
    
  for(int aaEvent = 0; aaEvent < dd.keepIndex.size(); aaEvent++){
    index = dd.keepIndex[aaEvent];
    preInfo.addPoint(dd.s1s[index], dd.promptFractions[index]);
  }


  // perform cuts
  PromptFractionCut pfCut;
  pfCut.execute(dd, false, true);

  for(int aaEvent = 0; aaEvent < dd.keepIndex.size(); aaEvent++){
    index = dd.keepIndex[aaEvent];
    postInfo.addPoint(dd.s1s[index], dd.promptFractions[index]);
  }



  // plot stats
  vector<double> dCenters = preInfo.getBinCenters();
  vector<int> preCounts = preInfo.getCounts();
  TGraph* dCountGraph = new TGraph(dBins);
  for(int aaBin = 0; aaBin < dBins; aaBin++){
    dCountGraph->SetPoint(aaBin, dCenters[aaBin],
                          (double)preCounts[aaBin]);
  }
  
  TCanvas* dCountsCanvas = new TCanvas();
  dCountGraph->Draw("al");
  dCountsCanvas->SaveAs("plots/pfDDCounts.png");


  // efficiencies, dd only in one spot in drift and phi, so only the others

  // s1
  vector<int> postCounts = postInfo.getCounts();
  vector<double> s1Effs;
  for(int aaBin = 0; aaBin < preCounts.size(); aaBin++){
    if(preCounts[aaBin] == 0){s1Effs.push_back(0); continue;}
    s1Effs.push_back(1 - (((double)(preCounts[aaBin] -
                                    postCounts[aaBin])) /
                          ((double)(preCounts[aaBin]))));
  }
  TGraph* s1EffGraph = new TGraph(dCenters.size(), &dCenters[0], &s1Effs[0]);
  TCanvas* s1EffCanvas = new TCanvas();
  s1EffGraph->Draw("al");
  s1EffCanvas->SaveAs("plots/pfDDEffS1.png");
  
  
  

  cout << "Time to run: " << timer.RealTime() << " seconds" << endl;
  
  return 0;
}
