#include "BadArea.h"
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


BadAreaCut::BadAreaCut()
  : Cut("BadAreaCut")
{}

void BadAreaCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if((d.timeBins[index] == 1
        and (
             (d.s2s[index] and d.badAreas[index] > 80)
             or (1472 <= d.s2s[index] /*and d.s2s[index] < 10000*/
                 and d.badAreas[index] >  .095 * (d.goodAreas[index] - 630))
             )
        )
       or
       d.timeBins[index] != 1
       and (d.goodAreas[index] > 200 and d.goodAreas[index] < 10000
            and
            d.badAreas[index] > 0.05167 * d.goodAreas[index]
            + 79.39
            - 57.04 * exp(-d.goodAreas[index] / 224.2)
            )
       ){
      d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}


void BadAreaCut::plot(dataContainer &d, string plotPrefix){
  plotStandards(d, plotPrefix);
  plotUniques(d, plotPrefix);
}


void BadAreaCut::plotUniques(dataContainer &d, string plotPrefix){

  gStyle->SetOptStat(10);
  
  // set up histograms
  TH2F* BAGAHist = new TH2F((plotPrefix + "_BAGA").c_str(),
                            (plotPrefix + "_BAGA").c_str(),
                            200, 0, 8000, 400, 0, 30000);

  // loop through and add non-masked events to the histogram
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    BAGAHist->Fill(d.badAreas[index], d.goodAreas[index]);
  } // end event loop

  // save an image of the histogram
  TCanvas* BAGACanvas = new TCanvas();
  BAGAHist->Draw("COLZ");
  BAGACanvas->SaveAs(("plots/" + plotPrefix + "_BAGA.png").c_str());

  TCanvas* BAGALogCanvas = new TCanvas();
  BAGALogCanvas->SetLogx();
  BAGALogCanvas->SetLogy();
  BAGAHist->Draw("COLZ");
  BAGALogCanvas->SaveAs(("plots/" + plotPrefix + "_logBAGA.png").c_str());

  // make plot again with recently cut events
  if(d.recentlyCutIndex.size() > 0){
    TGraph* BAGAGraph = new TGraph(d.recentlyCutIndex.size());

    for(int aaPoint = 0; aaPoint < d.recentlyCutIndex.size(); aaPoint++){
      int index = d.recentlyCutIndex[aaPoint];
      BAGAGraph->SetPoint(aaPoint, d.badAreas[index], d.goodAreas[index]);
    }

    // save image
    TCanvas* BAGACutCanvas = new TCanvas();
    BAGAHist->Draw("COLZ");
    stylizeCutGraph(BAGAGraph);
    BAGAGraph->Draw("psame");
    BAGACutCanvas->SaveAs(("plots/" + plotPrefix +
                           "_wCuts_BAGA.png").c_str());

    TCanvas* BAGACutLogCanvas = new TCanvas();
    BAGACutLogCanvas->SetLogx();
    BAGACutLogCanvas->SetLogy();
    BAGAHist->Draw("COLZ");
    stylizeCutGraph(BAGAGraph);
    BAGAGraph->Draw("psame");
    BAGACutLogCanvas->SaveAs(("plots/" + plotPrefix +
                           "_wCuts_logBAGA.png").c_str());
  }

  
}
