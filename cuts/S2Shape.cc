#include "S2Shape.h"
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

S2ShapeCut::S2ShapeCut()
  : Cut("S2ShapeCut")
{}

void S2ShapeCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  int count = 0;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    float width = d.s2AftT1s[index] - d.s2AftT0s[index];
    float pulseQuality = width/d.s2GausSigmas[index];
    float PQc = pulseQuality -
      (2.235
       - 0.4331 * exp(-d.drifts[index] / 45.01)
       - 0.2373 * exp(-d.drifts[index] / 13.25));
    if(
       PQc > (0.5092 * exp(-d.s2Raws[index] / 10802.9)
              + 1.792 * exp(-d.s2Raws[index] / 224.1)
              + 1.758 * exp(-d.s2Raws[index] / 1095)
              + 0.1468
              + 0.2336 * exp((d.s2Raws[index] - 1.1e5) / 56100.5))
       or
       PQc < (-0.2327 * exp(-d.s2Raws[index] / 5322)
              - 0.9724 * exp(-d.s2Raws[index] / 642.6)
              - 0.3547
              - 3.6e-7 * d.s2Raws[index])
       ){
      d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}


void S2ShapeCut::plot(dataContainer &d, string plotPrefix){
  plotStandards(d, plotPrefix);
  plotUniques(d, plotPrefix);
}


void S2ShapeCut::plotUniques(dataContainer& d, string plotPrefix){

  // set up plots
  TH2F* tDiffFitSigHist = new TH2F((plotPrefix + "_tDiffFitSig").c_str(),
                             (plotPrefix + "_tDiffFitSig").c_str(),
                             100, -100, 500, 100, 0, 100);
  TH1F* fitSigHist = new TH1F((plotPrefix + "_fitSig").c_str(),
                              (plotPrefix + "_fitSig").c_str(),
                              100, 0, 100);

  // loop through and add non-masked events to the histogram
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    tDiffFitSigHist->Fill(d.s2AftT1s[index] - d.s2AftT0s[index],
                          d.s2GausSigmas[index]);
    fitSigHist->Fill(d.s2GausSigmas[index]);
  } // end event loop

  // save an image of the histograms
  TCanvas* tDiffFitSigCanvas = new TCanvas();
  tDiffFitSigHist->Draw("COLZ");
  tDiffFitSigCanvas->SaveAs(("plots/" + plotPrefix + "_tDiffFitSig.png").c_str());

  TCanvas* fitSigCanvas = new TCanvas();
  fitSigHist->Draw();
  fitSigCanvas->SaveAs(("plots/" + plotPrefix + "_fitSig.png").c_str());

  // make plots again with cut events indicated (1d plots just have different
  // color hist)
  if(d.recentlyCutIndex.size() > 0){
    TGraph* tDiffFitSigGraph = new TGraph(d.recentlyCutIndex.size());
    TH1F* fitSigCutHist = new TH1F("fitSigCutHist", "fitSigCutHist",
                                   100, 0, 100);
    
    for(int aaPoint = 0; aaPoint < d.recentlyCutIndex.size(); aaPoint++){
      int index = d.recentlyCutIndex[aaPoint];
      tDiffFitSigGraph->SetPoint(aaPoint,
                                 d.s2AftT1s[index] - d.s2AftT0s[index],
                                 d.s2GausSigmas[index]);
      fitSigCutHist->Fill(d.s2GausSigmas[index]);
    }

    // plot them
    TCanvas* tDiffFitSigCutCanvas = new TCanvas();
    tDiffFitSigHist->Draw("COLZ");
    stylizeCutGraph(tDiffFitSigGraph);
    tDiffFitSigGraph->Draw("psame");
    tDiffFitSigCutCanvas->SaveAs(("plots/" + plotPrefix +
                                  "_wCuts_tDiffFitSig.png").c_str());

    TCanvas* fitSigCutCanvas = new TCanvas();
    fitSigHist->Draw();
    fitSigCutHist->SetLineColor(kRed);
    fitSigCutHist->Draw("same");
    fitSigCutCanvas->SaveAs(("plots/" + plotPrefix +
                             "_wCuts_fitSig.png").c_str());
  }
  
}
