#include <string>
#include <vector>
#include <iostream>

#include "Settings.h"
#include "utility/LoadData.h"
#include "utility/HistInfo.h"
#include "cuts/Cut.h"
#include "cuts/MaxPeakArea.h"

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


double meanDriftAdjust(double drift, double mpa){
  return mpa/(0.0546048
              + 7.94486e-05 * drift
              - 7.78426e-07 * pow(drift, 2)
              + 2.65714e-09 * pow(drift, 3));
}

double medianDriftAdjust(double drift, double mpa){
  return mpa/(0.0465434
              + 3.42106e-05 * drift
              - 1.06475e-07 * pow(drift, 2)
              - 2.54004e-10 * pow(drift, 3)
              + 9.47525e-13 * pow(drift, 4)
              + 1.11788e-14 * pow(drift, 5));
}


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
  HistInfo dPreS1Info(dMin, dMax, dBins);
  HistInfo dPostS1Info(dMin, dMax, dBins);
  HistInfo dPreS2Info(200, 36000, 300);
  HistInfo dPostS2Info(200, 36000, 300);
  HistInfo dPreRInfo(0, 17, 17);
  HistInfo dPostRInfo(0, 17, 17);
  HistInfo dPreDriftInfo(40, 300, 26);
  HistInfo dPostDriftInfo(40, 300, 26);
  HistInfo dPrePhiInfo(0, 3.15, 20);
  HistInfo dPostPhiInfo(0, 3.15, 20);
    
  for(int aaEvent = 0; aaEvent < dd.keepIndex.size(); aaEvent++){
    index = dd.keepIndex[aaEvent];
    dPreS1Info.addPoint(dd.s1s[index], dd.maxPeakAreas[index]);
    dPreS2Info.addPoint(dd.s2s[index], dd.maxPeakAreas[index]);
    dPreRInfo.addPoint(dd.rs[index], dd.maxPeakAreas[index]);
    dPreDriftInfo.addPoint(dd.drifts[index], dd.maxPeakAreas[index]);
    dPrePhiInfo.addPoint(dd.phis[index], dd.maxPeakAreas[index]);
  }


  // perform cuts
  MaxPeakAreaCut MPACut;
  MPACut.execute(dd, false, true);

  for(int aaEvent = 0; aaEvent < dd.keepIndex.size(); aaEvent++){
    index = dd.keepIndex[aaEvent];
    dPostS1Info.addPoint(dd.s1s[index], dd.maxPeakAreas[index]);
    dPostS2Info.addPoint(dd.s2s[index], dd.maxPeakAreas[index]);
    dPostRInfo.addPoint(dd.rs[index], dd.maxPeakAreas[index]);
    dPostDriftInfo.addPoint(dd.drifts[index], dd.maxPeakAreas[index]);
    dPostPhiInfo.addPoint(dd.phis[index], dd.maxPeakAreas[index]);
  }



  // plot stats
  vector<double> dCenters = dPreS1Info.getBinCenters();
  vector<int> dS1PreCounts = dPreS1Info.getCounts();
  TGraph* dCountGraph = new TGraph(dBins);
  for(int aaBin = 0; aaBin < dBins; aaBin++){
    dCountGraph->SetPoint(aaBin, dCenters[aaBin],
                          (double)dS1PreCounts[aaBin]);
  }
  
  TCanvas* dCountsCanvas = new TCanvas();
  dCountGraph->Draw("al");
  dCountsCanvas->SaveAs("plots/mpaDDCounts.png");


  // efficiencies, dd only in one spot in drift and phi, so only the others

  // s1
  vector<int> dS1PostCounts = dPostS1Info.getCounts();
  vector<double> s1Effs;
  for(int aaBin = 0; aaBin < dS1PreCounts.size(); aaBin++){
    if(dS1PreCounts[aaBin] == 0){s1Effs.push_back(0); continue;}
    s1Effs.push_back(1 - (((double)(dS1PreCounts[aaBin] -
                                    dS1PostCounts[aaBin])) /
                          ((double)(dS1PreCounts[aaBin]))));
  }
  TGraph* s1EffGraph = new TGraph(dCenters.size(), &dCenters[0], &s1Effs[0]);
  TCanvas* s1EffCanvas = new TCanvas();
  s1EffGraph->Draw("al");
  s1EffCanvas->SaveAs("plots/MPADDEffS1.png");
  
  
  

  cout << "Time to run: " << timer.RealTime() << " seconds" << endl;
  
  return 0;
}
