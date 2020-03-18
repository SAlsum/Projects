#include "StandardPlots.h"
#include "PlotUtils.h"

#include "../Settings.h"

#include <string>
#include <vector>
#include <math.h>

#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"

using std::string;

void plotStandards(dataContainer &d, string plotPrefix){
  // set up histograms
  TH2F* s1S2Hist = new TH2F((plotPrefix + "_S1S2").c_str(),
                            (plotPrefix + "_S1S2").c_str(),
                            299, 1, 300, 157, 200, 31600);
  TH2F* s1LogS2Hist = new TH2F((plotPrefix + "_S1LogS2").c_str(),
                            (plotPrefix + "_S1LogS2").c_str(),
                            299, 1, 300, 157, 2, 5);
  s1LogS2Hist->SetTitle((plotPrefix + "_S1LogS2" + ";S1c (phd);Log(S2c) (phd)").c_str());
  TH2F* spatialHist = new TH2F((plotPrefix + "_spatial").c_str(),
                               (plotPrefix + "_spatial").c_str(),
                               23, 0, 23, 40, 40, 300);
  TH2F* spatialR2Hist = new TH2F((plotPrefix + "_r2spatial").c_str(),
                                 (plotPrefix + "_r2spatial").c_str(),
                                 40, 0, 529, 40, 40, 300);
  TH2F* s1S2LowEnergyHist = new TH2F((plotPrefix + "_LE_S1S2").c_str(),
                                     (plotPrefix + "_LE_S1S2").c_str(),
                                     299, 1, 90, 157, 200, 6000);
  TH2F* s1LogS2LEHist = new TH2F((plotPrefix + "_LE_S1LogS2").c_str(),
                            (plotPrefix + "_LE_S1LogS2").c_str(),
                            299, 1, 90, 157, 2.3, 4.5);
  
  // loop through and add non-masked events to the histogram
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    s1S2Hist->Fill(d.s1s[index], d.s2s[index]);
    s1LogS2Hist->Fill(d.s1s[index], log10(d.s2s[index]));
    spatialHist->Fill(d.rs[index], d.drifts[index]);
    spatialR2Hist->Fill(pow(d.rs[index],2), d.drifts[index]);
    s1S2LowEnergyHist->Fill(d.s1s[index], d.s2s[index]);
    s1LogS2LEHist->Fill(d.s1s[index], log10(d.s2s[index]));
  } // end event loop

  // save an image of the histograms
  TCanvas* s1S2Canvas = new TCanvas();
  s1S2Hist->Draw("COLZ");
  s1S2Canvas->SaveAs(("plots/" + plotPrefix + "_S1S2.png").c_str());

  TCanvas* s1LogS2Canvas = new TCanvas();
  s1LogS2Hist->Draw("COLZ");
  s1LogS2Canvas->SaveAs(("plots/" + plotPrefix + "_S1LogS2.png").c_str());
  
  TCanvas* spatialCanvas = new TCanvas();
  spatialHist->Draw("COLZ");
  spatialCanvas->SaveAs(("plots/" + plotPrefix + "_spatial.png").c_str());

  TCanvas* spatialR2Canvas = new TCanvas();
  spatialR2Hist->Draw("COLZ");
  spatialR2Canvas->SaveAs(("plots/" + plotPrefix
                           + "_r2spatial.png").c_str());

  TCanvas* s1S2LECanvas = new TCanvas();
  s1S2LowEnergyHist->Draw("COLZ");
  s1S2LECanvas->SaveAs(("plots/" + plotPrefix + "_LE_S1S2.png").c_str());

  TCanvas* s1S2LEScatCanvas = new TCanvas();
  s1S2LowEnergyHist->Draw("AP");
  s1S2LEScatCanvas->SaveAs(("plots/" + plotPrefix +
                            "_LE_S1S2_Scatter.png").c_str());

  TCanvas* s1LogS2LECanvas = new TCanvas();
  s1LogS2LEHist->Draw("COLZ");
  s1LogS2LECanvas->SaveAs(("plots/" + plotPrefix +
                           "_LE_S1LogS2.png").c_str());

  
  // make plots again with recently cut events
  if(d.recentlyCutIndex.size() > 0){
    TGraph* s1S2Graph = new TGraph(d.recentlyCutIndex.size());
    TGraph* s1LogS2Graph = new TGraph(d.recentlyCutIndex.size());
    TGraph* spatialGraph = new TGraph(d.recentlyCutIndex.size());
    TGraph* spatialR2Graph = new TGraph(d.recentlyCutIndex.size());
    TGraph* s1S2LEGraph = new TGraph(d.recentlyCutIndex.size());
    TGraph* s1LogS2LEGraph = new TGraph(d.recentlyCutIndex.size());

    for(int aaPoint = 0; aaPoint < d.recentlyCutIndex.size(); aaPoint++){
      int index = d.recentlyCutIndex[aaPoint];
      s1S2Graph->SetPoint(aaPoint, d.s1s[index], d.s2s[index]);
      s1LogS2Graph->SetPoint(aaPoint, d.s1s[index], log10(d.s2s[index]));
      spatialGraph->SetPoint(aaPoint, d.rs[index], d.drifts[index]);
      spatialR2Graph->SetPoint(aaPoint, pow(d.rs[index],2), d.drifts[index]);
      s1S2LEGraph->SetPoint(aaPoint, d.s1s[index], d.s2s[index]);
      s1LogS2LEGraph->SetPoint(aaPoint, d.s1s[index], log10(d.s2s[index]));
    }
    
 // save an image of the histograms
  TCanvas* s1S2CutCanvas = new TCanvas();
  s1S2Hist->Draw("COLZ");
  stylizeCutGraph(s1S2Graph);
  s1S2Graph->Draw("psame");
  s1S2CutCanvas->SaveAs(("plots/" + plotPrefix + "_wCuts_S1S2.png").c_str());

  TCanvas* s1LogS2CutCanvas = new TCanvas();
  s1LogS2Hist->Draw("COLZ");
  stylizeCutGraph(s1LogS2Graph);
  s1LogS2Graph->Draw("psame");
  s1LogS2CutCanvas->SaveAs(("plots/" + plotPrefix + "_wCuts_S1LogS2.png").c_str());
  
  TCanvas* spatialCutCanvas = new TCanvas();
  spatialHist->Draw("COLZ");
  stylizeCutGraph(spatialGraph);
  spatialGraph->Draw("psame");
  spatialCutCanvas->SaveAs(("plots/" + plotPrefix + "_wCuts_spatial.png").c_str());

  TCanvas* spatialR2CutCanvas = new TCanvas();
  spatialR2Hist->Draw("COLZ");
  stylizeCutGraph(spatialR2Graph);
  spatialR2Graph->Draw("psame");
  spatialR2CutCanvas->SaveAs(("plots/" + plotPrefix
                           + "_wCuts_r2spatial.png").c_str());

  TCanvas* s1S2LECutCanvas = new TCanvas();
  s1S2LowEnergyHist->Draw("COLZ");
  stylizeCutGraph(s1S2LEGraph);
  s1S2LEGraph->Draw("psame");
  s1S2LECutCanvas->SaveAs(("plots/" + plotPrefix + "_wCuts_LE_S1S2.png").c_str());

  TCanvas* s1S2LEScatCutCanvas = new TCanvas();
  s1S2LowEnergyHist->Draw("AP");
  stylizeCutGraph(s1S2LEGraph);
  s1S2LEGraph->Draw("psame");
  s1S2LEScatCutCanvas->SaveAs(("plots/" + plotPrefix +
                            "_wCuts_LE_S1S2_Scatter.png").c_str());

  TCanvas* s1LogS2LECutCanvas = new TCanvas();
  s1LogS2LEHist->Draw("COLZ");
  stylizeCutGraph(s1LogS2LEGraph);
  s1LogS2LEGraph->Draw("psame");
  s1LogS2LECutCanvas->SaveAs(("plots/" + plotPrefix +
                           "_wCuts_LE_S1LogS2.png").c_str());
    
  }
}
