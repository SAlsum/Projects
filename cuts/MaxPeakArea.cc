#include "MaxPeakArea.h"
#include "../Settings.h"
#include "../utility/StandardPlots.h"
#include "../utility/PlotUtils.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;



MaxPeakAreaCut::MaxPeakAreaCut()
  : Cut("MaxPeakAreaCut")
{}

MaxPeakAreaCut::MaxPeakAreaCut(string MPADir)
  : Cut("MaxPeakAreaCut"){
  loadQuantDiffParams(MPADir);
  loadMedianParams(MPADir);
  // cout << "sanity check seizes " << mMedianParams.size() << " "
  //      << mMedianParams[0].size() << " "
  //      << mMedianParams[0][0].size() << endl;
  // cout << "sanity check offset params " << mQuantDiffParams[0][0] << " "
  //      << mQuantDiffParams[0][1] << " "
  //      << mQuantDiffParams[0][2] << " "
  //      << mQuantDiffParams[0][3] << " " << endl;
}


void MaxPeakAreaCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(aaEvent < 200){
       cout << "s1Raw: " << d.s1Raws[index] << " "
            << "z: " << d.zs[index] << " "
            << "median: " << getMedianValue(d.s1Raws[index], d.zs[index])
            << " offset: " << getDiffValue(d.s1Raws[index]) << " sum "
            << getMedianValue(d.s1Raws[index], d.zs[index]) + getDiffValue(d.s1Raws[index]) << endl;
    }
    if(d.maxPeakAreas[index] >
       getMedianValue(d.s1Raws[index], d.zs[index]) +
       getDiffValue(d.s1Raws[index])
       ){
      d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
      //      cout << "cutting event" << endl;
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
  cout << "counts after: " << d.keepIndex.size() << endl;
}

void MaxPeakAreaCut::plot(dataContainer &d, string plotPrefix){
  plotStandards(d, plotPrefix);
  plotUniques(d, plotPrefix);
}

void MaxPeakAreaCut::plotUniques(dataContainer &d, string plotPrefix){

  // set up histograms
  TH2F* s1MPAHist = new TH2F((plotPrefix + "_s1MPA").c_str(),
                             (plotPrefix + "_s1MPA").c_str(),
                             299, 1, 300, 100, 0, 100);
  TH2F* s1MPALEHist = new TH2F((plotPrefix + "_LE_s1MPA").c_str(),
                               (plotPrefix + "_LE_s1MPA").c_str(),
                               299, 1, 90, 100, 0, 100);

  // loop through and add non-masked events to the histogram
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    s1MPAHist->Fill(d.s1s[index], d.maxPeakAreas[index]);
    s1MPALEHist->Fill(d.s1s[index], d.maxPeakAreas[index]);
  } // end event loop

  // save an image of the histograms
  TCanvas* s1MPACanvas = new TCanvas();
  s1MPAHist->Draw("COLZ");
  s1MPACanvas->SaveAs(("plots/" + plotPrefix + "_S1MPA.png").c_str());

  TCanvas* s1MPALECanvas = new TCanvas();
  s1MPALEHist->Draw("COLZ");
  s1MPALECanvas->SaveAs(("plots/" + plotPrefix + "_LE_S1MPA.png").c_str());

  
  // make plots again with recently cut events
  if(d.recentlyCutIndex.size() > 0){
    TGraph* s1MPAGraph = new TGraph(d.recentlyCutIndex.size());
    TGraph* s1MPALEGraph = new TGraph(d.recentlyCutIndex.size());

    for(int aaPoint = 0; aaPoint < d.recentlyCutIndex.size(); aaPoint++){
      int index = d.recentlyCutIndex[aaPoint];
      s1MPAGraph->SetPoint(aaPoint, d.s1s[index], d.maxPeakAreas[index]);
      s1MPALEGraph->SetPoint(aaPoint, d.s1s[index], d.maxPeakAreas[index]);
    }

    // save image
    TCanvas* s1MPACutCanvas = new TCanvas();
    s1MPAHist->Draw("COLZ");
    stylizeCutGraph(s1MPAGraph);
    s1MPAGraph->Draw("psame");
    s1MPACutCanvas->SaveAs(("plots/" + plotPrefix +
                            "_wCuts_S1MPA.png").c_str());

    TCanvas* s1MPALECutCanvas = new TCanvas();
    s1MPALEHist->Draw("COLZ");
    stylizeCutGraph(s1MPALEGraph);
    s1MPALEGraph->Draw("psame");
    s1MPALECutCanvas->SaveAs(("plots/" + plotPrefix +
                              "_wCuts_LE_S1MPA.png").c_str());
  }
  
}


void MaxPeakAreaCut::loadQuantDiffParams(string MPADir){
  string thisFilename = MPADir + "/diffFitParams.txt";
  std::ifstream diffFile(thisFilename);
  string line;
  while(std::getline(diffFile, line)){
    vector<float> thisFitParams;
    std::istringstream iss(line);
    string item;
    float entry;
    while(iss >> item){
      entry = std::stof(item);
      thisFitParams.push_back(entry);
    }
    mQuantDiffParams.push_back(thisFitParams);
  }
} // end loadQuantDiffParams()


void MaxPeakAreaCut::loadMedianParams(string MPADir){
string thisFilename = MPADir + "/medFitParams.txt";
  std::ifstream medFile(thisFilename);
  string line;
  while(std::getline(medFile, line)){
    vector< vector<float> > thisSliceParams;
    vector<float> thisFitParams;
    std::istringstream iss(line);
    string item;
    float entry;
    while(iss >> item){
      if(item == "|"){
        thisSliceParams.push_back(thisFitParams);
        thisFitParams.clear();
      }
      else{
        entry = std::stof(item);
        thisFitParams.push_back(entry);
      }
    }
    thisSliceParams.push_back(thisFitParams);
    mMedianParams.push_back(thisSliceParams);
  }
} // end loadMedianParams()


int MaxPeakAreaCut::getZSlice(float z){
  if(z < 8){return 0;}
  else if(z > 50){return 15;}
  return floor((z - 8) / 1.75 /*step size*/);
  //return 7;
} // end getZSlice()



float MaxPeakAreaCut::getDiffValue(float s1Raw){
  float value = 0;
  int fitIndex;
  if(s1Raw < 35){fitIndex = 0;}
  else if(s1Raw < 110){fitIndex = 1;}
  else{fitIndex = 2;}
  for(int aaParam = 0; aaParam < mQuantDiffParams[fitIndex].size();
      aaParam++){
    value += mQuantDiffParams[fitIndex][aaParam] * pow(s1Raw, aaParam);
  }
  return value;
} // end getDiffValue()


float MaxPeakAreaCut::getMedianValue(float s1Raw, float z){
  float value = 0;
  int fitIndex;
  int zSlice = getZSlice(z);
  if(s1Raw < 29){fitIndex = 0;}
  else{fitIndex = 1;}
  //cout << "zSlice: " << zSlice << " fit: " << fitIndex << endl;
  for(int aaParam = 0; aaParam < mMedianParams[zSlice][fitIndex].size();
      aaParam++){
    value += mMedianParams[zSlice][fitIndex][aaParam] * pow(s1Raw, aaParam);
  }
  return value;
} // end getMedianValue
