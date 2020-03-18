#include "Chi2.h"
#include "../Settings.h"
#include "../utility/StandardPlots.h"
#include "../utility/PlotUtils.h"

#include <vector>
#include <iostream>

#include "TH2F.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TCanvas.h"

using std::vector;
using std::string;

Chi2Cut::Chi2Cut()
  : Cut("Chi2Cut")
{}

void Chi2Cut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  int count = 0;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if((d.s2Raws[index] <= 32774
        and d.chi2s[index] > 51.6 * (1 + pow(0.0149, 2) * d.s2Raws[index]))
       or
       (d.s2Raws[index] > 32774
        and d.chi2s[index] > 11359 * exp((d.s2Raws[index] - pow(10, 5))/20494))){
    d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}


void Chi2Cut::plot(dataContainer &d, string plotPrefix){
  plotStandards(d, plotPrefix);
  plotUniques(d, plotPrefix);
}


void Chi2Cut::plotUniques(dataContainer& d, string plotPrefix){

  // set up plots
  TH2F* s2Chi2Hist = new TH2F((plotPrefix + "_s2Chi2").c_str(),
                             (plotPrefix + "_s2Chi2").c_str(),
                             100, 100, 30000, 100, 0, 40);

  // loop through and add non-masked events to the histogram
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    s2Chi2Hist->Fill(d.s2Raws[index],
                 d.chi2s[index]/21.3);
  } // end event loop

  // save an image of the histograms
  TCanvas* s2Chi2Canvas = new TCanvas();
  s2Chi2Hist->Draw("COLZ");
  s2Chi2Canvas->SaveAs(("plots/" + plotPrefix + "_s2Chi2.png").c_str());

  if(d.recentlyCutIndex.size() > 0){
    TGraph* s2Chi2Graph = new TGraph(d.recentlyCutIndex.size());
    
    for(int aaPoint = 0; aaPoint < d.recentlyCutIndex.size(); aaPoint++){
      int index = d.recentlyCutIndex[aaPoint];
      s2Chi2Graph->SetPoint(aaPoint,
                            d.s2Raws[index], d.chi2s[index]/21.3);
    }

    // plot them
    TCanvas* s2Chi2CutCanvas = new TCanvas();
    s2Chi2Hist->Draw("COLZ");
    stylizeCutGraph(s2Chi2Graph);
    s2Chi2Graph->Draw("psame");
    s2Chi2CutCanvas->SaveAs(("plots/" + plotPrefix +
                                  "_wCuts_s2Chi2.png").c_str());

  }
  
}
