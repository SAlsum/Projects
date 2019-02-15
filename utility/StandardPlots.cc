#include "StandardPlots.h"

#include "../Settings.h"

#include <string>
#include <vector>
#include <math.h>

#include "TH2F.h"
#include "TCanvas.h"

using std::string;

void plotStandards(dataContainer &d, string plotPrefix){
  // set up histograms
  TH2F* s1S2Hist = new TH2F((plotPrefix + "_S1S2").c_str(),
                            (plotPrefix + "_S1S2").c_str(),
                            299, 1, 300, 157, 200, 31600);
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
                            299, 1, 90, 157, 2.3, 3.8);
  
  // loop through and add non-masked events to the histogram
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    s1S2Hist->Fill(d.s1s[index], d.s2s[index]);
    spatialHist->Fill(d.rs[index], d.drifts[index]);
    spatialR2Hist->Fill(pow(d.rs[index],2), d.drifts[index]);
    s1S2LowEnergyHist->Fill(d.s1s[index], d.s2s[index]);
    s1LogS2LEHist->Fill(d.s1s[index], log10(d.s2s[index]));
  } // end event loop

  // save an image of the histograms
  TCanvas* s1S2Canvas = new TCanvas();
  s1S2Hist->Draw("COLZ");
  s1S2Canvas->SaveAs(("plots/" + plotPrefix + "_S1S2.png").c_str());

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
}
