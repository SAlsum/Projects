#include "PromptFraction.h"
#include "../Settings.h"
#include "../utility/StandardPlots.h"
#include "../utility/PlotUtils.h"

#include <vector>
#include <math.h>

#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"

using std::vector;
using std::string;


PromptFractionCut::PromptFractionCut()
  : Cut("PromptFractionCut")
{}

void PromptFractionCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(
       /*
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
       */

       // new cut
       (d.s1s[index] <= 40 and
        d.promptFractions[index] <
        -3.21552 * pow(d.s1s[index], -3)
        + 5.13049 * pow(d.s1s[index], -2)
        -2.28076 * pow(d.s1s[index], -1)
        + 0.867027
        - 0.00249864 * d.s1s[index]
        + 5.56275e-05 * pow(d.s1s[index], 2)
        - 4.28088e-07 * pow(d.s1s[index], 3)
        ) or
       (d.s1s[index] > 40 and
        d.promptFractions[index] <
        -4.68072 * pow(d.s1s[index], -1)
        + 0.96818
        - 0.00222693 * d.s1s[index]
        + 1.13698e-05 * pow(d.s1s[index], 2)
        - 1.83202e-08 * pow(d.s1s[index], 3)
        )
       ){
      d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}


void PromptFractionCut::plot(dataContainer &d, std::string plotPrefix){
  plotStandards(d, plotPrefix);
  plotUniques(d, plotPrefix);
}


void PromptFractionCut::plotUniques(dataContainer &d, std::string plotPrefix){

  // set up histograms
  TH2F* s1PFHist = new TH2F((plotPrefix + "_s1PF").c_str(),
                            (plotPrefix + "_s1PF").c_str(),
                            299, 1, 300, 100, 0, 1);
  TH2F* s1PFLEHist = new TH2F((plotPrefix + "_LE_s1PF").c_str(),
                              (plotPrefix + "_LE_s1PF").c_str(),
                              299, 1, 90, 100, 0, 1);

    // loop through and add non-masked events to the histogram
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    s1PFHist->Fill(d.s1s[index], d.promptFractions[index]);
    s1PFLEHist->Fill(d.s1s[index], d.promptFractions[index]);
  } // end event loop

  // save an image of the histograms
  TCanvas* s1PFCanvas = new TCanvas();
  s1PFHist->Draw("COLZ");
  s1PFCanvas->SaveAs(("plots/" + plotPrefix + "_S1PF.png").c_str());

  TCanvas* s1PFLECanvas = new TCanvas();
  s1PFLEHist->Draw("COLZ");
  s1PFLECanvas->SaveAs(("plots/" + plotPrefix + "_LE_S1PF.png").c_str());

    // make plots again with recently cut events
  if(d.recentlyCutIndex.size() > 0){
    TGraph* s1PFGraph = new TGraph(d.recentlyCutIndex.size());
    TGraph* s1PFLEGraph = new TGraph(d.recentlyCutIndex.size());

    for(int aaPoint = 0; aaPoint < d.recentlyCutIndex.size(); aaPoint++){
      int index = d.recentlyCutIndex[aaPoint];
      s1PFGraph->SetPoint(aaPoint, d.s1s[index], d.promptFractions[index]);
      s1PFLEGraph->SetPoint(aaPoint, d.s1s[index], d.promptFractions[index]);
    }

    // save image
    TCanvas* s1PFCutCanvas = new TCanvas();
    s1PFHist->Draw("COLZ");
    stylizeCutGraph(s1PFGraph);
    s1PFGraph->Draw("psame");
    s1PFCutCanvas->SaveAs(("plots/" + plotPrefix +
                            "_wCuts_S1PF.png").c_str());

    TCanvas* s1PFLECutCanvas = new TCanvas();
    s1PFLEHist->Draw("COLZ");
    stylizeCutGraph(s1PFLEGraph);
    s1PFLEGraph->Draw("psame");
    s1PFLECutCanvas->SaveAs(("plots/" + plotPrefix +
                              "_wCuts_LE_S1PF.png").c_str());
  }
}
