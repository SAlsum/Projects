#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include "Settings.h"
#include "utility/LoadData.h"
#include "utility/HistInfo.h"
#include "utility/HistInfo2d.h"
#include "cuts/Cut.h"
#include "cuts/MaxPeakArea.h"

#include "TStopwatch.h"

#include "TStyle.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1I.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::unordered_map;
using std::unordered_set;

struct HistParams{
  int nBins;
  double min;
  double max;
};


float medS1RawAdjust(float s1Raw, float mpaf){
  return mpaf/(0.051161
               - 0.000127281 * s1Raw
               + 3.49059e-08 * pow(s1Raw, 2)
               + 1.05165e-09 * pow(s1Raw, 3)
               + 1.83668e-12 * pow(s1Raw, 4)
               - 9.94122e-15 * pow(s1Raw, 5)
               + 1.51508 * pow(s1Raw, -1)
               - 0.695723 * pow(s1Raw, -2)
               - 0.944691 * pow(s1Raw, -3));
}


float quantOffset(vector< vector <float> >& params, float s1Raw){
  float value = 0;
  if(s1Raw < 35){
    for(int aaParam = 0; aaParam < params[0].size(); aaParam++){
      value += params[0][aaParam] * pow(s1Raw, aaParam);
    }
    return value;
  } else if(s1Raw < 110){
    for(int aaParam = 0; aaParam < params[1].size(); aaParam++){
      value += params[1][aaParam] * pow(s1Raw, aaParam);
    }
    return value;
  } else {
    for(int aaParam = 0; aaParam < params[2].size(); aaParam++){
      value += params[2][aaParam] * pow(s1Raw, aaParam);
    }
    return value;
  }
}


float medValue(vector< vector< vector<float> > >& params, int slice, float s1Raw){
  float value = 0;
  if(s1Raw < 29){
    for(int aaParam = 0; aaParam < params[slice][0].size(); aaParam++){
      value += params[slice][0][aaParam] * pow(s1Raw, aaParam);
    }
  } else {
    for(int aaParam = 0; aaParam < params[slice][1].size(); aaParam++){
      value += params[slice][1][aaParam] * pow(s1Raw, aaParam);
    }
  }

  return value;
}



int getCompareBin(float s1Raw){
  return (int)floor((s1Raw - 0) / 20);
}


int main(int argc, char* argv[]){
  gStyle->SetOptStat(0);
 
  // map for datasets
  //enum dataSet{C14, CH3T, DD};
  //unordered_map<dataSet, dataContainer> data;
  unordered_map<string, dataContainer> data;
  
  //carbon 14 data (main analysis)
  string c14Filename = "data/EFTData_BasePlusExtended_Run04_C14.root";
  vector<string> c14Filenames;
  c14Filenames.push_back(c14Filename);
  data["C14"] = loadData(c14Filenames);
  
  
  //run04 tritium data (to compare efficiency)
  string tritiumFilename = "data/EFTData_BasePlusExtended_Run04_CH3T.root";
  vector<string> tritiumFilenames;
  tritiumFilenames.push_back(tritiumFilename);
  data["CH3T"] = loadData(tritiumFilenames);

  //run04 DD data (to compare efficiency)
  string DDFilename = "data/EFTData_BasePlusExtended_Run04_DD.root";
  vector<string> DDFilenames;
  DDFilenames.push_back(DDFilename);
  data["DD"] = loadData(DDFilenames);

  //run03 tritium data (to compare efficiency)
  string r3TritiumFilename = "data/EFTData_BasePlus_Run03_CH3T.root";
  vector<string> r3TritiumFilenames;
  r3TritiumFilenames.push_back(r3TritiumFilename);
  data["R3CH3T"] = loadData(r3TritiumFilenames);

  // parameters for viewing data
  unordered_map<string, HistParams> histParams;
  histParams["C14"] = {100, 1, 300};
  histParams["CH3T"] = {25, 1, 100};
  histParams["DD"] = {25, 1, 100};
  histParams["R3CH3T"] = {25, 1, 100};
  histParams["drift"] = {52, 40, 300}; // same for all sources
  histParams["z"] = {52, 10, 50}; // same for all sources
  histParams["s1"] = {75, 1, 300}; // for use comparing across sources
  histParams["s2"] = {75, 200, 31600};
  histParams["r"] = {56, 0, 23};
  histParams["phi"] = {26,-3.14, 3.14};
  unordered_map<string, HistParams> histParams2d;
  histParams2d["C14"] = {20, 0, 300};
  histParams2d["CH3T"] = {20, 1, 40};
  histParams2d["DD"] = {25, 1, 100};
  histParams2d["R3CH3T"] = {20, 1, 40};
  histParams2d["drift"] = {8, 40, 300};
  histParams2d["z"] = {24, 8, 50};

  // list of observables to find efficiency for
  unordered_set<string> observables;
  observables.insert("s1");
  observables.insert("s2");
  observables.insert("r");
  observables.insert("drift");
  observables.insert("phi");

  //*********
  // ANALYZE
  //*********
  
  // plot each un-corrected MPA vs s1c and vs s1Raw
  unordered_map<string, HistInfo> MPAVsS1s;
  unordered_map<string, HistInfo> MPAVsS1Raws;
  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;
    TH2F* thisMPAS1Hist = new TH2F((thisName + " MPA vs s1c").c_str(),
                                   (thisName + " MPA vs s1c").c_str(),
                                   histParams[thisName].nBins,
                                   histParams[thisName].min,
                                   histParams[thisName].max,
                                   100, //ybins
                                   0, //ymin
                                   histParams[thisName].max * 0.13);
    TH2F* thisMPAS1RawHist = new TH2F((thisName + " MPA vs s1_raw").c_str(),
                                      (thisName + " MPA vs s1_raw").c_str(),
                                      histParams[thisName].nBins,
                                      histParams[thisName].min,
                                      histParams[thisName].max,
                                      100, //ybins
                                      0, //ymin
                                      histParams[thisName].max * 0.13);
    HistInfo thisHistInfo(histParams[thisName].min,
                          histParams[thisName].max,
                          histParams[thisName].nBins);
    HistInfo thisRawHistInfo(histParams[thisName].min,
                             histParams[thisName].max,
                             histParams[thisName].nBins);
    for(int aaEvent = 0; aaEvent < thisData.keepIndex.size(); aaEvent++){
      int index = thisData.keepIndex[aaEvent];
      thisHistInfo.addPoint(thisData.s1s[index],
                            thisData.maxPeakAreas[index]);
      thisRawHistInfo.addPoint(thisData.s1Raws[index],
                               thisData.maxPeakAreas[index]);
      thisMPAS1Hist->Fill(thisData.s1s[index], thisData.maxPeakAreas[index]);
      thisMPAS1RawHist->Fill(thisData.s1Raws[index],
                             thisData.maxPeakAreas[index]);
    }
    MPAVsS1s[thisName] = thisHistInfo;
    MPAVsS1Raws[thisName] = thisRawHistInfo;

    // get quantile values
    vector<double> binCenters = thisHistInfo.getBinCenters(); //same for both
    vector<double> cMedians = thisHistInfo.getQuantileSigma(0);
    TGraph* medGraph = new TGraph(binCenters.size(), &binCenters[0],
                                  &cMedians[0]);
    medGraph->SetLineColor(2);
    medGraph->SetLineWidth(2);
    vector<double> rawMedians = thisRawHistInfo.getQuantileSigma(0);
    TGraph* rawMedGraph = new TGraph(binCenters.size(), &binCenters[0],
                                     &rawMedians[0]);
    rawMedGraph->SetLineColor(2);
    rawMedGraph->SetLineWidth(2);
    vector<double> c99s = thisHistInfo.getQuantile(.99);
    TGraph* c99Graph = new TGraph(binCenters.size(), &binCenters[0],
                                  &c99s[0]);
    c99Graph->SetLineColor(5);
    c99Graph->SetLineWidth(2);
    vector<double> raw99s = thisRawHistInfo.getQuantile(.99);
    TGraph* raw99Graph = new TGraph(binCenters.size(), &binCenters[0],
                                    &raw99s[0]);
    raw99Graph->SetLineColor(5);
    raw99Graph->SetLineWidth(2);

    //plot
    TCanvas* s1MPACanvas = new TCanvas();
    //legend later
    thisMPAS1Hist->Draw("colz");
    medGraph->Draw("samel");
    c99Graph->Draw("samel");
    //draw legend
    s1MPACanvas->SaveAs(("plots/MaxPeakAreaStudy/MPAS1c_Run04_" + thisName
                         + ".png").c_str());

    TCanvas* s1RawMPACanvas = new TCanvas();
    //legend later
    thisMPAS1RawHist->Draw("colz");
    rawMedGraph->Draw("samel");
    raw99Graph->Draw("samel");
    s1RawMPACanvas->SaveAs(("plots/MaxPeakAreaStudy/MPAS1raw_Run04_"
                            + thisName + ".png").c_str());

  }

  // plot s1raw medians on top of each other
  TGraph* c14MedGraph =
    new TGraph(MPAVsS1Raws["C14"].getBinCenters().size(),
               &MPAVsS1Raws["C14"].getBinCenters()[0],
               &MPAVsS1Raws["C14"].getQuantileSigma(0)[0]);
  TGraph* tritMedGraph =
    new TGraph(MPAVsS1Raws["CH3T"].getBinCenters().size(),
               &MPAVsS1Raws["CH3T"].getBinCenters()[0],
               &MPAVsS1Raws["CH3T"].getQuantileSigma(0)[0]);
  tritMedGraph->SetLineColor(3);
  TCanvas* rawMedCompareCanvas = new TCanvas();
  c14MedGraph->Draw("");
  tritMedGraph->Draw("same");
  rawMedCompareCanvas->SaveAs("plots/MaxPeakAreaStudy/MPAS1Raw_Run04_SourceCompare.png");

  //**************************
  // FACTOR OUT S1 DEPENDENCE
  //**************************

  // attempt to correct for the dependence on s1.
  // First the obvious correlation linearly with total collected light
  unordered_map<string, unordered_map<string, vector<float> > > derivedData;

  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;

    // actually do all events, not only kept, so in the future index will
    // line up
    vector<float> mpafs;
    for(int aaEvent = 0; aaEvent < thisData.s1s.size(); aaEvent++){
      mpafs.push_back(thisData.maxPeakAreas[aaEvent]
                      / thisData.s1Raws[aaEvent]);
    } // end looping over data points
    derivedData[thisName]["MPAF"] = mpafs;
  } // end data type loop

    // plot MPAF vs s1 raw
  unordered_map<string, HistInfo> MPAFVsS1Raws; // save for later
  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;
    TH2F* thisMPAFS1Hist = new TH2F((thisName + " MPAF vs s1c").c_str(),
                                    (thisName + " MPAF vs s1c").c_str(),
                                    histParams[thisName].nBins,
                                    histParams[thisName].min,
                                    histParams[thisName].max,
                                    100, //ybins
                                    0, //ymin
                                    1);
    TH2F* thisMPAFS1RawHist = new TH2F((thisName + " MPAF vs s1_raw").c_str(),
                                       (thisName + " MPAF vs s1_raw").c_str(),
                                       histParams[thisName].nBins,
                                       histParams[thisName].min,
                                       histParams[thisName].max,
                                       100, //ybins
                                       0, //ymin
                                       1);
    HistInfo thisHistInfo(histParams[thisName].min,
                          histParams[thisName].max,
                          histParams[thisName].nBins);
    HistInfo thisRawHistInfo(histParams[thisName].min,
                             histParams[thisName].max,
                             histParams[thisName].nBins);
    for(int aaEvent = 0; aaEvent < thisData.keepIndex.size(); aaEvent++){
      int index = thisData.keepIndex[aaEvent];
      thisHistInfo.addPoint(thisData.s1s[index],
                            derivedData[thisName]["MPAF"][index]);
      thisRawHistInfo.addPoint(thisData.s1Raws[index],
                               derivedData[thisName]["MPAF"][index]);
      thisMPAFS1Hist->Fill(thisData.s1s[index],
                           derivedData[thisName]["MPAF"][index]);
      thisMPAFS1RawHist->Fill(thisData.s1Raws[index],
                              derivedData[thisName]["MPAF"][index]);
    }
    MPAFVsS1Raws[thisName] = thisRawHistInfo;
    // get medians
    TGraph* cMedGraph = new TGraph(thisHistInfo.getBinCenters().size(),
                                   &thisHistInfo.getBinCenters()[0],
                                   &thisHistInfo.getQuantileSigma(0)[0]);
    cMedGraph->SetLineWidth(2);
    TGraph* rawMedGraph = new TGraph(thisRawHistInfo.getBinCenters().size(),
                                     &thisRawHistInfo.getBinCenters()[0],
                                     &thisRawHistInfo.getQuantileSigma(0)[0]);
    rawMedGraph->SetLineWidth(2);

    TCanvas* mpafcCanvas = new TCanvas();
    thisMPAFS1Hist->Draw("colz");
    cMedGraph->Draw("samel");
    mpafcCanvas->SaveAs(("plots/MaxPeakAreaStudy/MPAFVsS1c_Run04_" + thisName
                         + ".png").c_str());

    TCanvas* mpafrawCanvas = new TCanvas();
    thisMPAFS1RawHist->Draw("colz");
    rawMedGraph->Draw("samel");
    mpafrawCanvas->SaveAs(("plots/MaxPeakAreaStudy/MPAFVsS1Raw_Run04_"
                           + thisName + ".png").c_str());
  } //end loop over calibration type


  //***********************************
  // CORRECT STATISTICAL S1 DEPENDENCE
  //***********************************

  // work with just c14 for the fit, verify with tritium later
  // if I'm right there should be a remaining statistical piece that depends
  // on s1_raw multiplied by a bunchiness term depending on depth (drift)
  // I think these should only be weakly correlated, so they can be corrected
  // independently.
  // Due to tritium and c14 being uniform sources, the depth dependent piece
  // should be mostly taken care of (though may change some with time bin)
  // so first get rid of statisticl dependence.

  // fit function to s1 median
  TF1* fMPAF = new TF1("fMPAF", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)+[6]*pow(x,-1)+[7]*pow(x,-2)+[8]*pow(x,-3)", 1, 300);
  TGraph* c14MPAFS1RawGraph =
    new TGraph(MPAFVsS1Raws["C14"].getBinCenters().size(),
               &MPAFVsS1Raws["C14"].getBinCenters()[0],
               &MPAFVsS1Raws["C14"].getQuantileSigma(0)[0]);
  c14MPAFS1RawGraph->Fit(fMPAF,"R0");
  c14MPAFS1RawGraph->SetLineWidth(2);
  fMPAF->SetLineColor(2);
  fMPAF->SetLineWidth(2);

  // plot the result
  TH2F* c14MPAFS1RawHist = new TH2F("C14 MPAF vs s1_raw",
                                     "C14 MPAF vs s1_raw",
                                     histParams["C14"].nBins,
                                     histParams["C14"].min,
                                     histParams["C14"].max,
                                     100, //ybins
                                     0, //ymin
                                     1);
   for(int aaEvent = 0; aaEvent < data["C14"].keepIndex.size(); aaEvent++){
      int index = data["C14"].keepIndex[aaEvent];
      c14MPAFS1RawHist->Fill(data["C14"].s1Raws[index],
                             derivedData["C14"]["MPAF"][index]);
   }
  TCanvas* c14MPAFFitCanvas = new TCanvas();
  c14MPAFS1RawHist->Draw("colz");
  c14MPAFS1RawGraph->Draw("samel");
  fMPAF->Draw("samel");
  c14MPAFFitCanvas->SaveAs("plots/MaxPeakAreaStudy/MPAFC14Fit.png");

  
  // adjust for the dependence
 for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;

    // actually do all events, not only kept, so in the future index will
    // line up
    vector<float> s1AdjMPAF;
    for(int aaEvent = 0; aaEvent < thisData.s1s.size(); aaEvent++){
      s1AdjMPAF.push_back(medS1RawAdjust(thisData.s1Raws[aaEvent],
                                     derivedData[thisName]["MPAF"][aaEvent]));
    } // end looping over data points
    derivedData[thisName]["s1AdjMPAF"] = s1AdjMPAF;
  } // end data type loop


 // plot the adjusted stuff
 for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;

    TH2F* thisS1AdjHist = new TH2F((thisName +
                                    " s1AdjMPAF vs s1_raw").c_str(),
                                   (thisName +
                                    " s1AdjMPAF vs s1_raw").c_str(),
                                   histParams[thisName].nBins,
                                   histParams[thisName].min,
                                   histParams[thisName].max,
                                   100, //ybins
                                   0, //ymin
                                   5);
    HistInfo thisHistInfo(histParams[thisName].min,
                          histParams[thisName].max,
                          histParams[thisName].nBins);
    
    for(int aaEvent = 0; aaEvent < thisData.keepIndex.size(); aaEvent++){
      int index = thisData.keepIndex[aaEvent];
      thisHistInfo.addPoint(thisData.s1Raws[index],
                            derivedData[thisName]["s1AdjMPAF"][index]);
      thisS1AdjHist->Fill(thisData.s1Raws[index],
                          derivedData[thisName]["s1AdjMPAF"][index]);
    } // end event loop
    TGraph* medGraph = new TGraph(thisHistInfo.getBinCenters().size(),
                                 &thisHistInfo.getBinCenters()[0],
                                 &thisHistInfo.getQuantileSigma(0)[0]);
    medGraph->SetLineWidth(2);
    
    TCanvas* s1AdjCanvas = new TCanvas();
    thisS1AdjHist->Draw("colz");
    medGraph->Draw("samel");
    s1AdjCanvas->SaveAs(("plots/MaxPeakAreaStudy/s1AdjustedMPAF_s1Raw_"
                         + (string)"Run04_" + thisName + ".png").c_str());
 } // end calibration type loop

 //************************
 // STUDY DRIFT DEPENDENCE
 //************************
 
 //plot vs drift to see if there is a correlation there
  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;

    TH2F* thisS1AdjDriftHist = new TH2F((thisName +
                                    " s1AdjMPAF vs drift").c_str(),
                                   (thisName +
                                    " s1AdjMPAF vs drift").c_str(),
                                   histParams["drift"].nBins,
                                   histParams["drift"].min,
                                   histParams["drift"].max,
                                   100, //ybins
                                   0, //ymin
                                   5);
    HistInfo thisHistInfo(histParams["drift"].min,
                          histParams["drift"].max,
                          histParams["drift"].nBins);
    
    for(int aaEvent = 0; aaEvent < thisData.keepIndex.size(); aaEvent++){
      int index = thisData.keepIndex[aaEvent];
      thisHistInfo.addPoint(thisData.drifts[index],
                            derivedData[thisName]["s1AdjMPAF"][index]);
      thisS1AdjDriftHist->Fill(thisData.drifts[index],
                          derivedData[thisName]["s1AdjMPAF"][index]);
    } // end event loop
    TGraph* medGraph = new TGraph(thisHistInfo.getBinCenters().size(),
                                 &thisHistInfo.getBinCenters()[0],
                                 &thisHistInfo.getQuantileSigma(0)[0]);
    medGraph->SetLineWidth(2);
    
    TCanvas* s1AdjDriftCanvas = new TCanvas();
    thisS1AdjDriftHist->Draw("colz");
    medGraph->Draw("samel");
    s1AdjDriftCanvas->SaveAs(("plots/MaxPeakAreaStudy/s1AdjustedMPAF_drift_"
                         + (string)"Run04_" + thisName + ".png").c_str());
 } // end calibration type loop


  // plot medians vs drift for various s1 slices to see if there is lingering
  // dependence
  // just use carbon 14 for this
  vector< vector< float > > MPAFs;
  vector< vector< float > > s1AdjMPAFs;
  vector< vector< float > > drifts;
  vector< HistInfo > MPAFInfo;
  vector< HistInfo > s1AdjMPAFInfo;
  for(int s1Bin = 0; s1Bin < 10; s1Bin++){
    vector<float> initVec;
    MPAFs.push_back(initVec);
    s1AdjMPAFs.push_back(initVec);
    drifts.push_back(initVec);

    HistInfo initInfo(histParams["drift"].min,
                      histParams["drift"].max,
                      histParams["drift"].nBins);
    MPAFInfo.push_back(initInfo);
    s1AdjMPAFInfo.push_back(initInfo);
  }

  // sort events into the bins (bins of 20 s1raw)
  for(int aaEvent = 0; aaEvent < data["C14"].keepIndex.size(); aaEvent++){
      int index = data["C14"].keepIndex[aaEvent];
      if(data["C14"].s1Raws[index] < 200){
        int cBin = getCompareBin(data["C14"].s1Raws[index]);

        MPAFInfo[cBin].addPoint(data["C14"].drifts[index],
                                derivedData["C14"]["MPAF"][index]);
        s1AdjMPAFInfo[cBin].addPoint(data["C14"].drifts[index],
                                     derivedData["C14"]["s1AdjMPAF"][index]);
        /*
        MPAFs[cBin].push_back(derivedData["C14"]["MPAF"][index]);
        s1AdjMPAFs[cBin].push_back(derivedData["C14"]["MPAF"][index]);
        drifts[cBin].push_back(data["C14"].drifts[index]);
        */
      }
  }
  
  //plot all the medians
  TCanvas* MPAFDriftCanvas = new TCanvas();
  for(int s1Bin = 0; s1Bin < 10; s1Bin++){
    TGraph* thisGraph = new TGraph(MPAFInfo[s1Bin].getBinCenters().size(),
                                   &MPAFInfo[s1Bin].getBinCenters()[0],
                                   &MPAFInfo[s1Bin].getQuantileSigma(0)[0]);
    thisGraph->SetLineWidth(2);
    thisGraph->SetLineColor(s1Bin + 40);
    thisGraph->GetYaxis()->SetRangeUser(0, .25);
    if(s1Bin < 1){thisGraph->Draw("al");}
    else{thisGraph->Draw("samel");}
  }
  MPAFDriftCanvas->SaveAs("plots/MaxPeakAreaStudy/s1Dependence_Drift.png");

  TCanvas* s1AdjMPAFDriftCanvas = new TCanvas();
  for(int s1Bin = 0; s1Bin < 10; s1Bin++){
    TGraph* thisGraph =
      new TGraph(s1AdjMPAFInfo[s1Bin].getBinCenters().size(),
                 &s1AdjMPAFInfo[s1Bin].getBinCenters()[0],
                 &s1AdjMPAFInfo[s1Bin].getQuantileSigma(0)[0]);
    thisGraph->SetLineWidth(2);
    thisGraph->SetLineColor(s1Bin + 40);
    thisGraph->GetYaxis()->SetRangeUser(0.8, 1.5);
    if(s1Bin < 1){thisGraph->Draw("al");}
    else{thisGraph->Draw("samel");}
  }
  s1AdjMPAFDriftCanvas->SaveAs("plots/MaxPeakAreaStudy/AdjustedS1Dependence_Drift.png");


  //************************************
  // LOOK AT FULL 2D S1_RAW DRIFT SPACE
  //************************************

  // ok, so s1 dependence still there, just do full 2d characterization
  unordered_map<string, HistInfo2d> MPAFVsS1RawsDrifts;
  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;

    HistInfo2d this2DInfo(histParams2d[thisName].min,
                          histParams2d[thisName].max,
                          histParams2d[thisName].nBins,
                          histParams2d["drift"].min,
                          histParams2d["drift"].max,
                          histParams2d["drift"].nBins);

    for(int aaEvent = 0; aaEvent < thisData.keepIndex.size(); aaEvent++){
      int index = thisData.keepIndex[aaEvent];

      this2DInfo.addPoint(thisData.s1Raws[index], thisData.drifts[index],
                          derivedData[thisName]["MPAF"][index]);
    }

    MPAFVsS1RawsDrifts[thisName] = this2DInfo;


    //graph stuff

    TGraph2D* MPAFS1RawDrift = new TGraph2D(histParams2d[thisName].nBins *
                                            histParams2d["drift"].nBins);
    TGraph2D* s1RawDriftStats = new TGraph2D(histParams2d[thisName].nBins *
                                             histParams2d["drift"].nBins);
    TH2F* s1RawDriftStatHist = new TH2F(("2dStats" + thisName).c_str(),
                                      ("2dStats" + thisName).c_str(),
                                      histParams2d[thisName].nBins,
                                      histParams2d[thisName].min,
                                      histParams2d[thisName].max,
                                      histParams2d["drift"].nBins,
                                      histParams2d["drift"].min,
                                      histParams2d["drift"].max);
    TH2F* s1RawDriftMPAFHist = new TH2F(("2dMPAF" + thisName).c_str(),
                                      ("2dMPAF" + thisName).c_str(),
                                      histParams2d[thisName].nBins,
                                      histParams2d[thisName].min,
                                      histParams2d[thisName].max,
                                      histParams2d["drift"].nBins,
                                      histParams2d["drift"].min,
                                      histParams2d["drift"].max);
    TH2F* s1RawDriftMPAF99Hist = new TH2F(("2dMPAF_.99Quantile_"
                                           + thisName).c_str(),
                                      ("2dMPAF_.99Quantile_"
                                       + thisName).c_str(),
                                      histParams2d[thisName].nBins,
                                      histParams2d[thisName].min,
                                      histParams2d[thisName].max,
                                      histParams2d["drift"].nBins,
                                      histParams2d["drift"].min,
                                      histParams2d["drift"].max);
    vector< vector<int> > statsMatrix = this2DInfo.getCounts();
    vector<float> s1BinCenters = this2DInfo.getXBinCenters();
    vector<float> driftBinCenters = this2DInfo.getYBinCenters();
    vector< vector<float> > medians = this2DInfo.getQuantileSigma(0);
    vector< vector<float> > quant99s = this2DInfo.getQuantile(.99);
    for(int s1Bin = 0; s1Bin < histParams2d[thisName].nBins; s1Bin++){
      for(int driftBin = 0; driftBin < histParams2d["drift"].nBins;
          driftBin++){
        s1RawDriftStats->SetPoint(s1Bin * histParams2d[thisName].nBins +
                                  driftBin,
                                  s1BinCenters[s1Bin],
                                  driftBinCenters[driftBin],
                                  statsMatrix[s1Bin][driftBin]);
        s1RawDriftStatHist->SetBinContent(s1Bin, driftBin,
                                          statsMatrix[s1Bin][driftBin]);
        s1RawDriftMPAFHist->SetBinContent(s1Bin, driftBin,
                                          medians[s1Bin][driftBin]);
        MPAFS1RawDrift->SetPoint(s1Bin * histParams2d[thisName].nBins +
                                 driftBin,
                                 s1BinCenters[s1Bin],
                                 driftBinCenters[driftBin],
                                 medians[s1Bin][driftBin]);
        s1RawDriftMPAF99Hist->SetBinContent(s1Bin, driftBin,
                                            quant99s[s1Bin][driftBin]);
      }
    } // end bin loop

    TCanvas* statsCanvas = new TCanvas();
    s1RawDriftStats->SetMarkerSize(3);
    s1RawDriftStats->Draw("pcol");
    statsCanvas->SaveAs(("plots/MaxPeakAreaStudy/s1Raw_Drift_stats_" +
                         thisName + ".png").c_str());

    TCanvas* statsHistCanvas = new TCanvas();
    s1RawDriftStatHist->Draw("colz");
    statsHistCanvas->SaveAs(("plots/MaxPeakAreaStudy/s1Raw_Drift_stats_" +
                         thisName + "_Hist.png").c_str());

    TCanvas* MPAFHistCanvas = new TCanvas();
    s1RawDriftMPAFHist->Draw("colz");
    MPAFHistCanvas->SaveAs(("plots/MaxPeakAreaStudy/s1Raw_Drift_MPAF_" +
                            thisName + "_Hist.png").c_str());

    TCanvas* MPAF99HistCanvas = new TCanvas();
    s1RawDriftMPAF99Hist->Draw("colz");
    MPAF99HistCanvas->SaveAs(("plots/MaxPeakAreaStudy/s1Raw_Drift_MPAF_0.99Quantile_" +
                            thisName + "_Hist.png").c_str());
    
  }

  // look at quantiles for 1-40 s1 with tritium, whole range with c14
  // look at each drift slice (to double check results
  // match at low s1)
  TCanvas* compareSlicesCanvas = new TCanvas();
  vector<float> c14BinCenters = MPAFVsS1RawsDrifts["C14"].getXBinCenters();
  vector< vector<float> > c14MPAF99Quants =
    MPAFVsS1RawsDrifts["C14"].getQuantile(.99);
  vector<float> ch3tBinCenters = MPAFVsS1RawsDrifts["CH3T"].getXBinCenters();
  vector< vector<float> > ch3tMPAF99Quants =
    MPAFVsS1RawsDrifts["CH3T"].getQuantile(.99);
  
  for(int aaSlice = 0; aaSlice < histParams2d["drift"].nBins; aaSlice++){
    TGraph* sliceGraphC14 = new TGraph(histParams2d["C14"].nBins);
    for(int s1Bin = 0;
        s1Bin < c14BinCenters.size();
        s1Bin++){
      sliceGraphC14->SetPoint(s1Bin, c14BinCenters[s1Bin],
                              aaSlice * .2 // to offset slices
                              + c14MPAF99Quants[s1Bin][aaSlice]);
    }
    sliceGraphC14->SetLineColor(1);
    sliceGraphC14->SetLineWidth(2);
    if(aaSlice == 0){
      sliceGraphC14->GetXaxis()->SetRangeUser(0, 300);
      sliceGraphC14->GetYaxis()->SetRangeUser(0, 3.0);
      sliceGraphC14->Draw("al");
    } else {sliceGraphC14->Draw("samel");}

    TGraph* sliceGraphCH3T = new TGraph(histParams2d["CH3T"].nBins);
    for(int s1Bin = 0;
        s1Bin < ch3tBinCenters.size();
        s1Bin++){
      sliceGraphCH3T->SetPoint(s1Bin, ch3tBinCenters[s1Bin],
                              aaSlice * .2 // to offset slices
                              + ch3tMPAF99Quants[s1Bin][aaSlice]);
    }
    sliceGraphCH3T->SetLineColor(2);
    sliceGraphCH3T->SetLineWidth(2);
    sliceGraphCH3T->Draw("samel");
  }
  
  compareSlicesCanvas->SaveAs("plots/MaxPeakAreaStudy/compareSlices_0.99Quantile.png");  


  // stitch the low-end CH3T (1-40) onto the high-end C14 (40-300) for each
  // slice
  TCanvas* stitchedCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["drift"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPAFs;
    for(int s1Bin = 0; s1Bin < ch3tBinCenters.size(); s1Bin++){
      if(ch3tBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tBinCenters[s1Bin]);
        stitchedMPAFs.push_back(ch3tMPAF99Quants[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14BinCenters.size(); s1Bin++){
      if(c14BinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14BinCenters[s1Bin]);
        stitchedMPAFs.push_back(c14MPAF99Quants[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPAFs[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      stitchedSliceGraph->GetYaxis()->SetRangeUser(0, 1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
        
  } // end slice loop
  stitchedCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_slices_0.99Quantile.png");




  // do the same bur for medians
  TCanvas* compareSlicesMedCanvas = new TCanvas();
  vector< vector<float> > c14MPAFMeds =
    MPAFVsS1RawsDrifts["C14"].getQuantile(.5);
  vector< vector<float> > ch3tMPAFMeds =
    MPAFVsS1RawsDrifts["CH3T"].getQuantile(.5);
  
  for(int aaSlice = 0; aaSlice < histParams2d["drift"].nBins; aaSlice++){
    TGraph* sliceGraphC14 = new TGraph(histParams2d["C14"].nBins);
    for(int s1Bin = 0;
        s1Bin < c14BinCenters.size();
        s1Bin++){
      sliceGraphC14->SetPoint(s1Bin, c14BinCenters[s1Bin],
                              aaSlice * .2 // to offset slices
                              + c14MPAFMeds[s1Bin][aaSlice]);
    }
    sliceGraphC14->SetLineColor(1);
    sliceGraphC14->SetLineWidth(2);
    if(aaSlice == 0){
      sliceGraphC14->GetXaxis()->SetRangeUser(0, 300);
      sliceGraphC14->GetYaxis()->SetRangeUser(0, 3.0);
      sliceGraphC14->Draw("al");
    } else {sliceGraphC14->Draw("samel");}

    TGraph* sliceGraphCH3T = new TGraph(histParams2d["CH3T"].nBins);
    for(int s1Bin = 0;
        s1Bin < ch3tBinCenters.size();
        s1Bin++){
      sliceGraphCH3T->SetPoint(s1Bin, ch3tBinCenters[s1Bin],
                              aaSlice * .2 // to offset slices
                              + ch3tMPAFMeds[s1Bin][aaSlice]);
    }
    sliceGraphCH3T->SetLineColor(2);
    sliceGraphCH3T->SetLineWidth(2);
    sliceGraphCH3T->Draw("samel");
  }
  
  compareSlicesMedCanvas->SaveAs("plots/MaxPeakAreaStudy/compareSlices_median.png");  


  // stitch the low-end CH3T (1-40) onto the high-end C14 (40-300) for each
  // slice
  TCanvas* stitchedMedCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["drift"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPAFs;
    for(int s1Bin = 0; s1Bin < ch3tBinCenters.size(); s1Bin++){
      if(ch3tBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tBinCenters[s1Bin]);
        stitchedMPAFs.push_back(ch3tMPAFMeds[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14BinCenters.size(); s1Bin++){
      if(c14BinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14BinCenters[s1Bin]);
        stitchedMPAFs.push_back(c14MPAFMeds[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPAFs[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      stitchedSliceGraph->GetYaxis()->SetRangeUser(0, 1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
        
  } // end slice loop
  stitchedMedCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_slices_medians.png");


  // one last ditch thing
  // plot the relative difference to the bottom slice

  // fit to the bottom slice
  vector<float> stitchedBotS1s;
  vector<float> stitchedBotMPAFs;
  for(int s1Bin = 0; s1Bin < ch3tBinCenters.size(); s1Bin++){
    if(ch3tBinCenters[s1Bin] <= 40){
      stitchedBotS1s.push_back(ch3tBinCenters[s1Bin]);
      stitchedBotMPAFs.push_back(ch3tMPAFMeds[s1Bin][histParams2d["drift"].nBins-1]);
    }
  }
  for(int s1Bin = 0; s1Bin < c14BinCenters.size(); s1Bin++){
    if(c14BinCenters[s1Bin] > 40){
      stitchedBotS1s.push_back(c14BinCenters[s1Bin]);
      stitchedBotMPAFs.push_back(c14MPAFMeds[s1Bin][histParams2d["drift"].nBins-1]);
    }
  } //end bin loop
  TGraph* stitchedBotSliceGraph = new TGraph(stitchedBotS1s.size(),
                                          &stitchedBotS1s[0],
                                          &stitchedBotMPAFs[0]);
  stitchedBotSliceGraph->SetLineWidth(2);
  stitchedBotSliceGraph->SetLineColor(1);
  stitchedBotSliceGraph->GetYaxis()->SetRangeUser(0, 1);
  

  // fit function to s1 median bottom slice
  TF1* fMPAFBotSlice = new TF1("fMPAFBotSlcie", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)+[6]*pow(x,-1)+[7]*pow(x,-2)+[8]*pow(x,-3)", 1, 300);
  stitchedBotSliceGraph->Fit(fMPAFBotSlice,"R0");

  TCanvas* botSliceFitCanvas = new TCanvas();
  stitchedBotSliceGraph->Draw("al");
  fMPAFBotSlice->SetLineColor(2);
  fMPAFBotSlice->SetLineWidth(2);
  fMPAFBotSlice->Draw("same");
  botSliceFitCanvas->SaveAs("plots/MaxPeakAreaStudy/botSliceFit.png");


  // make new rq that uses this as correction
  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;

    // actually do all events, not only kept, so in the future index will
    // line up
    vector<float> botS1AdjMPAFs;
    for(int aaEvent = 0; aaEvent < thisData.s1s.size(); aaEvent++){
      botS1AdjMPAFs.push_back(derivedData[thisName]["MPAF"][aaEvent] /
                              fMPAFBotSlice->Eval(thisData.s1Raws[aaEvent]));
    } // end looping over data points
    derivedData[thisName]["botS1AdjMPAF"] = botS1AdjMPAFs;
  } // end data type loop


  // plot all slices relative to that fit now
  // 2d characterization for this new adjusted rq
  unordered_map<string, HistInfo2d> botS1AdjVsS1RawsDrifts;
  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;

    HistInfo2d this2DInfo(histParams2d[thisName].min,
                          histParams2d[thisName].max,
                          histParams2d[thisName].nBins,
                          histParams2d["drift"].min,
                          histParams2d["drift"].max,
                          histParams2d["drift"].nBins);

    for(int aaEvent = 0; aaEvent < thisData.keepIndex.size(); aaEvent++){
      int index = thisData.keepIndex[aaEvent];

      this2DInfo.addPoint(thisData.s1Raws[index], thisData.drifts[index],
                          derivedData[thisName]["botS1AdjMPAF"][index]);
    }

    botS1AdjVsS1RawsDrifts[thisName] = this2DInfo;
  } // end data calib looop

  // make the slices and plot
  // stitch the low-end CH3T (1-40) onto the high-end C14 (40-300) for each
  // slice
  vector<float> c14BotAdjBinCenters = botS1AdjVsS1RawsDrifts["C14"].getXBinCenters();
  vector< vector<float> > c14BotAdjMPAFMeds =
    botS1AdjVsS1RawsDrifts["C14"].getQuantile(.5);
  vector<float> ch3tBotAdjBinCenters = botS1AdjVsS1RawsDrifts["CH3T"].getXBinCenters();
  vector< vector<float> > ch3tBotAdjMPAFMeds =
    botS1AdjVsS1RawsDrifts["CH3T"].getQuantile(.5);
  TCanvas* stitchedMedBotAdjCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["drift"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPAFs;
    for(int s1Bin = 0; s1Bin < ch3tBotAdjBinCenters.size(); s1Bin++){
      if(ch3tBotAdjBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tBotAdjBinCenters[s1Bin]);
        stitchedMPAFs.push_back(ch3tBotAdjMPAFMeds[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14BotAdjBinCenters.size(); s1Bin++){
      if(c14BotAdjBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14BotAdjBinCenters[s1Bin]);
        stitchedMPAFs.push_back(c14BotAdjMPAFMeds[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPAFs[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      stitchedSliceGraph->GetYaxis()->SetRangeUser(0.6, 1.1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
        
  } // end slice loop
  stitchedMedBotAdjCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_slices_medians_botAdjusted.png");

  
  // plot the same but for .99 quantiles (but still adjusted to top mean)
  vector< vector<float> > c14BotAdjMPAF99Quants =
    botS1AdjVsS1RawsDrifts["C14"].getQuantile(.99);
  vector< vector<float> > ch3tBotAdjMPAF99Quants =
    botS1AdjVsS1RawsDrifts["CH3T"].getQuantile(.99);
  TCanvas* stitched99QuantBotAdjCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["drift"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPAFs;
    for(int s1Bin = 0; s1Bin < ch3tBotAdjBinCenters.size(); s1Bin++){
      if(ch3tBotAdjBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tBotAdjBinCenters[s1Bin]);
        stitchedMPAFs.push_back(ch3tBotAdjMPAF99Quants[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14BotAdjBinCenters.size(); s1Bin++){
      if(c14BotAdjBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14BotAdjBinCenters[s1Bin]);
        stitchedMPAFs.push_back(c14BotAdjMPAF99Quants[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPAFs[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      stitchedSliceGraph->GetYaxis()->SetRangeUser(1.3, 2.5);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
        
  } // end slice loop
  stitched99QuantBotAdjCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_slices_0.99Quantiles_botAdjusted.png");


  // fit function to the maxPeakArea straight, not fraction (smoother)

  // first plot in the slices
  // plot all slices relative to that fit now
  // 2d characterization for this new adjusted rq
  unordered_map<string, HistInfo2d> mpaVsS1RawsDrifts;
  unordered_map<string, HistInfo2d> mpaVsS1csDrifts;
  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;

    HistInfo2d thisRaw2DInfo(histParams2d[thisName].min,
                          histParams2d[thisName].max,
                          histParams2d[thisName].nBins,
                          histParams2d["drift"].min,
                          histParams2d["drift"].max,
                          histParams2d["drift"].nBins);
    HistInfo2d thisCorr2DInfo(histParams2d[thisName].min,
                          histParams2d[thisName].max,
                          histParams2d[thisName].nBins,
                          histParams2d["drift"].min,
                          histParams2d["drift"].max,
                          histParams2d["drift"].nBins);

    for(int aaEvent = 0; aaEvent < thisData.keepIndex.size(); aaEvent++){
      int index = thisData.keepIndex[aaEvent];

      thisRaw2DInfo.addPoint(thisData.s1Raws[index], thisData.drifts[index],
                             thisData.maxPeakAreas[index]);
      thisCorr2DInfo.addPoint(thisData.s1s[index], thisData.drifts[index],
                              thisData.maxPeakAreas[index]);
    }

    mpaVsS1RawsDrifts[thisName] = thisRaw2DInfo;
    mpaVsS1csDrifts[thisName] = thisCorr2DInfo;
  } // end data calib looop

  // make the slices and plot
  // stitch the low-end CH3T (1-40) onto the high-end C14 (40-300) for each
  // slice
  vector<float> c14MPARawBinCenters =
    mpaVsS1RawsDrifts["C14"].getXBinCenters();
  vector< vector<float> > c14MPARawMeds =
    mpaVsS1RawsDrifts["C14"].getQuantile(.5);
  vector<float> ch3tMPARawBinCenters = mpaVsS1RawsDrifts["CH3T"].getXBinCenters();
  vector< vector<float> > ch3tMPARawMeds =
    mpaVsS1RawsDrifts["CH3T"].getQuantile(.5);
  TCanvas* stitchedMedMPARawCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["drift"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPARaws;
    for(int s1Bin = 0; s1Bin < ch3tMPARawBinCenters.size(); s1Bin++){
      if(ch3tMPARawBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tMPARawBinCenters[s1Bin]);
        stitchedMPARaws.push_back(ch3tMPARawMeds[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14MPARawBinCenters.size(); s1Bin++){
      if(c14MPARawBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14MPARawBinCenters[s1Bin]);
        stitchedMPARaws.push_back(c14MPARawMeds[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPARaws[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      //stitchedSliceGraph->GetYaxis()->SetRangeUser(0.6, 1.1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
        
  } // end slice loop
  stitchedMedMPARawCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_slices_medians_mpa.png");

  // 99% quantiles
  vector< vector<float> > c14MPARaw99s =
    mpaVsS1RawsDrifts["C14"].getQuantile(.99);
  vector< vector<float> > ch3tMPARaw99s =
    mpaVsS1RawsDrifts["CH3T"].getQuantile(.99);
  TCanvas* stitched99MPARawCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["drift"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPARaws;
    for(int s1Bin = 0; s1Bin < ch3tMPARawBinCenters.size(); s1Bin++){
      if(ch3tMPARawBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tMPARawBinCenters[s1Bin]);
        stitchedMPARaws.push_back(ch3tMPARaw99s[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14MPARawBinCenters.size(); s1Bin++){
      if(c14MPARawBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14MPARawBinCenters[s1Bin]);
        stitchedMPARaws.push_back(c14MPARaw99s[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPARaws[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      //stitchedSliceGraph->GetYaxis()->SetRangeUser(0.6, 1.1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
        
  } // end slice loop
  stitched99MPARawCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_slices_0.099Quantile_mpa.png");


 
  // make the slices and plot (mpa vs s1c)
  // stitch the low-end CH3T (1-40) onto the high-end C14 (40-300) for each
  // slice
  vector<float> c14MPACBinCenters =
    mpaVsS1csDrifts["C14"].getXBinCenters();
  vector< vector<float> > c14MPACMeds =
    mpaVsS1csDrifts["C14"].getQuantile(.5);
  vector<float> ch3tMPACBinCenters = mpaVsS1csDrifts["CH3T"].getXBinCenters();
  vector< vector<float> > ch3tMPACMeds =
    mpaVsS1csDrifts["CH3T"].getQuantile(.5);
  TCanvas* stitchedMedMPACCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["drift"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPACs;
    for(int s1Bin = 0; s1Bin < ch3tMPACBinCenters.size(); s1Bin++){
      if(ch3tMPACBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tMPACBinCenters[s1Bin]);
        stitchedMPACs.push_back(ch3tMPACMeds[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14MPACBinCenters.size(); s1Bin++){
      if(c14MPACBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14MPACBinCenters[s1Bin]);
        stitchedMPACs.push_back(c14MPACMeds[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPACs[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      //stitchedSliceGraph->GetYaxis()->SetRangeUser(0.6, 1.1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
        
  } // end slice loop
  stitchedMedMPARawCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_slices_medians_mpaVsS1c.png");

  // 99% quantiles
  vector< vector<float> > c14MPAC99s =
    mpaVsS1csDrifts["C14"].getQuantile(.99);
  vector< vector<float> > ch3tMPAC99s =
    mpaVsS1csDrifts["CH3T"].getQuantile(.99);
  TCanvas* stitched99MPACCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["drift"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPACs;
    for(int s1Bin = 0; s1Bin < ch3tMPACBinCenters.size(); s1Bin++){
      if(ch3tMPACBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tMPACBinCenters[s1Bin]);
        stitchedMPACs.push_back(ch3tMPAC99s[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14MPACBinCenters.size(); s1Bin++){
      if(c14MPACBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14MPACBinCenters[s1Bin]);
        stitchedMPACs.push_back(c14MPAC99s[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPACs[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      //stitchedSliceGraph->GetYaxis()->SetRangeUser(0.6, 1.1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
        
  } // end slice loop
  stitched99MPACCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_slices_0.099Quantile_mpaVsS1c.png");


  //************************
  // USE Z INSTEAD OF DRIFT
  //************************

  // make 2d histogram in s1_raw and z
  
  // first plot in the slices
  // plot all slices relative to that fit now
  // 2d characterization for this new adjusted rq
  unordered_map<string, HistInfo2d> mpaVsS1RawsZs;
  unordered_map<string, HistInfo2d> mpaVsS1csZs;
  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;

    HistInfo2d thisRaw2DInfo(histParams2d[thisName].min,
                          histParams2d[thisName].max,
                          histParams2d[thisName].nBins,
                          histParams2d["z"].min,
                          histParams2d["z"].max,
                          histParams2d["z"].nBins);
    HistInfo2d thisCorr2DInfo(histParams2d[thisName].min,
                          histParams2d[thisName].max,
                          histParams2d[thisName].nBins,
                          histParams2d["z"].min,
                          histParams2d["z"].max,
                          histParams2d["z"].nBins);

    for(int aaEvent = 0; aaEvent < thisData.keepIndex.size(); aaEvent++){
      int index = thisData.keepIndex[aaEvent];

      thisRaw2DInfo.addPoint(thisData.s1Raws[index], thisData.zs[index],
                             thisData.maxPeakAreas[index]);
      thisCorr2DInfo.addPoint(thisData.s1s[index], thisData.zs[index],
                              thisData.maxPeakAreas[index]);
    }

    mpaVsS1RawsZs[thisName] = thisRaw2DInfo;
    mpaVsS1csZs[thisName] = thisCorr2DInfo;
  } // end data calib looop
  
  // make the slices and plot (mpa vs s1c)
  // stitch the low-end CH3T (1-40) onto the high-end C14 (40-300) for each
  // slice
  vector<float> c14MPACZBinCenters =
    mpaVsS1RawsZs["C14"].getXBinCenters();
  vector< vector<float> > c14MPACZMeds =
    mpaVsS1RawsZs["C14"].getQuantile(.5);
  vector<float> ch3tMPACZBinCenters =
   mpaVsS1RawsZs ["CH3T"].getXBinCenters();
  vector< vector<float> > ch3tMPACZMeds =
    mpaVsS1RawsZs["CH3T"].getQuantile(.5);
  TCanvas* stitchedMedMPACZCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["z"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPACs;
    for(int s1Bin = 0; s1Bin < ch3tMPACZBinCenters.size(); s1Bin++){
      if(ch3tMPACZBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tMPACZBinCenters[s1Bin]);
        stitchedMPACs.push_back(ch3tMPACZMeds[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14MPACZBinCenters.size(); s1Bin++){
      if(c14MPACZBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14MPACZBinCenters[s1Bin]);
        stitchedMPACs.push_back(c14MPACZMeds[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPACs[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      //stitchedSliceGraph->GetYaxis()->SetRangeUser(0.6, 1.1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
        
  } // end slice loop
  stitchedMedMPARawCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_ZSlices_medians_mpaVsS1c.png");

  // 99% quantiles
  vector< vector<float> > c14MPACZ99s =
    mpaVsS1RawsZs["C14"].getQuantile(.99);
  vector< vector<float> > ch3tMPACZ99s =
    mpaVsS1RawsZs["CH3T"].getQuantile(.99);
  TCanvas* stitched99MPACZCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["z"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPACs;
    for(int s1Bin = 0; s1Bin < ch3tMPACZBinCenters.size(); s1Bin++){
      if(ch3tMPACZBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tMPACZBinCenters[s1Bin]);
        stitchedMPACs.push_back(ch3tMPACZ99s[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14MPACZBinCenters.size(); s1Bin++){
      if(c14MPACZBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14MPACZBinCenters[s1Bin]);
        stitchedMPACs.push_back(c14MPACZ99s[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPACs[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      //stitchedSliceGraph->GetYaxis()->SetRangeUser(0.6, 1.1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
        
  } // end slice loop
  stitched99MPACCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_ZSlices_0.099Quantile_mpaVsS1c.png");


  // look at the difference between the 99% quantile and the median to see
  // if it is consistent between slices.  Long shot, but hey.

  vector<float> c14MPARawZBinCenters =
    mpaVsS1RawsZs["C14"].getXBinCenters();
  vector< vector<float> > c14MPARawZMeds =
    mpaVsS1RawsZs["C14"].getQuantile(.5);
  vector<float> ch3tMPARawZBinCenters =
    mpaVsS1RawsZs["CH3T"].getXBinCenters();
  vector< vector<float> > ch3tMPARawZMeds =
    mpaVsS1RawsZs["CH3T"].getQuantile(.5);
  
  vector< vector<float> > c14MPARawZ99s =
    mpaVsS1RawsZs["C14"].getQuantile(.99);
  vector< vector<float> > ch3tMPARawZ99s =
    mpaVsS1RawsZs["CH3T"].getQuantile(.99);
  
  vector<float> combinedDiffs;
  vector<float> combinedS1s;
  TCanvas* MPAQuantDiffCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["z"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPADiffs;
    for(int s1Bin = 0; s1Bin < ch3tMPARawZBinCenters.size(); s1Bin++){
      if(ch3tMPACZBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tMPARawZBinCenters[s1Bin]);
        stitchedMPADiffs.push_back(ch3tMPARawZ99s[s1Bin][aaSlice] -
                                   ch3tMPARawZMeds[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14MPARawZBinCenters.size(); s1Bin++){
      if(c14MPARawZBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14MPARawZBinCenters[s1Bin]);
        stitchedMPADiffs.push_back(c14MPARawZ99s[s1Bin][aaSlice] -
                                c14MPARawZMeds[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPADiffs[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    if(aaSlice == 0){
      //stitchedSliceGraph->GetYaxis()->SetRangeUser(0.6, 1.1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
    combinedDiffs.insert(combinedDiffs.end(),
                         stitchedMPADiffs.begin(), stitchedMPADiffs.end());
    combinedS1s.insert(combinedS1s.begin(),
                       stitchedS1s.begin(), stitchedS1s.end());
  } // end slice loop
  MPAQuantDiffCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_ZSlices_QuantileDifference.png");


  // looks like there isn't a correlation?  Except maybe between 50 and 100
  // s1?
  // average the slices together and add that as an offset.
  TGraph* combinedSliceDiffsGraph = new TGraph(combinedS1s.size(),
                                               &combinedS1s[0],
                                               &combinedDiffs[0]);
  TF1* fQDiffLow = new TF1("fQDiffLow", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)", 0.5, 60);
  TF1* fQDiffMed = new TF1("fQDiffMed", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)", 30, 120);
  TF1* fQDiffHigh = new TF1("fQDiffHigh", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)", 100, 300);
  combinedSliceDiffsGraph->Fit(fQDiffLow, "R0");
  combinedSliceDiffsGraph->Fit(fQDiffMed, "R0");
  combinedSliceDiffsGraph->Fit(fQDiffHigh, "R0");
  TCanvas* diffFitCanvas = new TCanvas();
  combinedSliceDiffsGraph->SetMarkerStyle(8);
  combinedSliceDiffsGraph->SetMarkerSize(.25);
  combinedSliceDiffsGraph->Draw("ap");
  fQDiffLow->SetLineColor(2);
  fQDiffLow->Draw("samel");
  fQDiffMed->SetLineColor(3);
  fQDiffMed->Draw("samel");
  fQDiffHigh->SetLineColor(4);
  fQDiffHigh->Draw("samel");
  diffFitCanvas->SaveAs("plots/MaxPeakAreaStudy/QuantDiffFit.png");

  // print the parameters used for this fit
  // also add them to a vector while we're at it
  vector< vector< float > > diffFitParams;
  std::ofstream diffParamFile;
  diffParamFile.open("utility/MaxPeakAreaParams/diffFitParams.txt");
  int nDiffParamsLow = fQDiffLow->GetNpar();
  vector<float> lowParams;
  for(int aaParam = 0; aaParam < nDiffParamsLow; aaParam++){
    diffParamFile << fQDiffLow->GetParameter(aaParam) << " ";
    lowParams.push_back(fQDiffLow->GetParameter(aaParam));
  }
  diffParamFile << "\n";
  int nDiffParamsMed = fQDiffMed->GetNpar();
  vector<float> medParams;
  for(int aaParam = 0; aaParam < nDiffParamsMed; aaParam++){
    diffParamFile << fQDiffMed->GetParameter(aaParam) << " ";
    medParams.push_back(fQDiffMed->GetParameter(aaParam));
  }
  diffParamFile << "\n";
  int nDiffParamsHigh = fQDiffHigh->GetNpar();
  vector<float> highParams;
  for(int aaParam = 0; aaParam < nDiffParamsHigh; aaParam++){
    diffParamFile << fQDiffHigh->GetParameter(aaParam) << " ";
    highParams.push_back(fQDiffHigh->GetParameter(aaParam));
  }
  diffParamFile << "\n";
  diffParamFile.close();
  diffFitParams.push_back(lowParams);
  diffFitParams.push_back(medParams);
  diffFitParams.push_back(highParams);


  // now that we have offsets, fit to the medians
  vector< vector< vector< float > > >  mpaMedianFitParams;
  std::ofstream medParamsFile;
  medParamsFile.open("utility/MaxPeakAreaParams/medFitParams.txt");
  // make the slices and plot (mpa vs s1Raw) 
  // stitch the low-end CH3T (1-40) onto the high-end C14 (40-300) for each
  // slice
  TCanvas* stitchedMedFitCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["z"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPACs;
    for(int s1Bin = 0; s1Bin < ch3tMPARawZBinCenters.size(); s1Bin++){
      if(ch3tMPACZBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tMPARawZBinCenters[s1Bin]);
        stitchedMPACs.push_back(ch3tMPARawZMeds[s1Bin][aaSlice]);
      }
    }
    for(int s1Bin = 0; s1Bin < c14MPARawZBinCenters.size(); s1Bin++){
      if(c14MPARawZBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14MPARawZBinCenters[s1Bin]);
        stitchedMPACs.push_back(c14MPARawZMeds[s1Bin][aaSlice]);
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPACs[0]);
    TF1* medSliceFitLow = new TF1("medSliceFitLow", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)+[6]*pow(x,6)+[7]*pow(x,7)", 0.5, 30);
    TF1* medSliceFitHigh = new TF1("medSliceFitHigh", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)", 25, 300);
    stitchedSliceGraph->Fit(medSliceFitLow, "R0");
    stitchedSliceGraph->Fit(medSliceFitHigh, "R0");
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    medSliceFitLow->SetLineWidth(2);
    medSliceFitLow->SetLineStyle(2);
    medSliceFitLow->SetLineColor(4);
    medSliceFitHigh->SetLineWidth(2);
    medSliceFitHigh->SetLineStyle(2);
    medSliceFitHigh->SetLineColor(6);
    if(aaSlice == 0){
      stitchedSliceGraph->GetXaxis()->SetRangeUser(0, 50);
      stitchedSliceGraph->GetYaxis()->SetRangeUser(0, 4);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
    medSliceFitLow->Draw("samel");
    medSliceFitHigh->Draw("samel");

    // save slice params
    vector<float> thisSliceMedFitLowParams;
    vector<float> thisSliceMedFitHighParams;
    vector< vector<float> > thisSliceMedFitParams; 
    for(int aaParam = 0; aaParam < medSliceFitLow->GetNpar(); aaParam++){
      thisSliceMedFitLowParams.push_back(medSliceFitLow->GetParameter(aaParam));
      medParamsFile << medSliceFitLow->GetParameter(aaParam) << " ";
    }
    thisSliceMedFitParams.push_back(thisSliceMedFitLowParams);
    medParamsFile << "| ";
    for(int aaParam = 0; aaParam < medSliceFitHigh->GetNpar(); aaParam++){
      thisSliceMedFitHighParams.push_back(medSliceFitHigh->GetParameter(aaParam));
      medParamsFile << medSliceFitHigh->GetParameter(aaParam) << " ";
    }
    thisSliceMedFitParams.push_back(thisSliceMedFitHighParams);
    mpaMedianFitParams.push_back(thisSliceMedFitParams);
    medParamsFile << "\n";
  } // end slice loop
  //stitchedMedFitCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_ZSlices_medians_fit.png");
  medParamsFile.close();

  // same plot but zoomed in
  stitchedMedFitCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_ZSlices_medians_fit_zoomed.png");


  // now plot medians + difference vs the quantile slices, and hope it's close
  TCanvas* compareFitsCanvas = new TCanvas();
  for(int aaSlice = 0; aaSlice < histParams2d["z"].nBins; aaSlice++){
    vector<float> stitchedS1s;
    vector<float> stitchedMPAs;
    vector<float> fitMPAs;
    for(int s1Bin = 0; s1Bin < ch3tMPARawZBinCenters.size(); s1Bin++){
      if(ch3tMPARawZBinCenters[s1Bin] <= 40){
        stitchedS1s.push_back(ch3tMPARawZBinCenters[s1Bin]);
        stitchedMPAs.push_back(ch3tMPARawZ99s[s1Bin][aaSlice]);
        fitMPAs.push_back(medValue(mpaMedianFitParams, aaSlice,
                                   ch3tMPARawZBinCenters[s1Bin]) +
                          quantOffset(diffFitParams,
                                      ch3tMPARawZBinCenters[s1Bin]));
      }
    }
    for(int s1Bin = 0; s1Bin < c14MPARawZBinCenters.size(); s1Bin++){
      if(c14MPARawZBinCenters[s1Bin] > 40){
        stitchedS1s.push_back(c14MPARawZBinCenters[s1Bin]);
        stitchedMPAs.push_back(c14MPARawZ99s[s1Bin][aaSlice]);
        fitMPAs.push_back(medValue(mpaMedianFitParams, aaSlice,
                                   c14MPARawZBinCenters[s1Bin]) +
                          quantOffset(diffFitParams,
                                      c14MPARawZBinCenters[s1Bin]));
      }
    } //end bin loop
    TGraph* stitchedSliceGraph = new TGraph(stitchedS1s.size(),
                                            &stitchedS1s[0],
                                            &stitchedMPAs[0]);
    TGraph* fitSliceGraph = new TGraph(stitchedS1s.size(),
                                       &stitchedS1s[0],
                                       &fitMPAs[0]);
    stitchedSliceGraph->SetLineWidth(2);
    stitchedSliceGraph->SetLineColor(aaSlice + 40);
    fitSliceGraph->SetLineWidth(2);
    fitSliceGraph->SetLineColor(aaSlice + 40);
    fitSliceGraph->SetLineStyle(2);
    if(aaSlice == 0){
      //stitchedSliceGraph->GetYaxis()->SetRangeUser(0.6, 1.1);
      stitchedSliceGraph->Draw("al");
    } else {stitchedSliceGraph->Draw("samel");}
    fitSliceGraph->Draw("samel");
        
  } // end slice loop
  compareFitsCanvas->SaveAs("plots/MaxPeakAreaStudy/stitched_ZSlices_0.099Quantile_vs_fits.png");


  unordered_map<string,
                unordered_map<string, HistInfo>  > efficiencies_before;
  // loop over data sets
  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;
    
    // loop through observables
    for(string thisObs : observables){
      HistInfo thisInfo(histParams[thisObs].min,
                        histParams[thisObs].max,
                        histParams[thisObs].nBins);

      // add events to the histogram
      for(int aaEvent = 0; aaEvent < thisData.keepIndex.size(); aaEvent++){
        int index = thisData.keepIndex[aaEvent];
        if(thisObs == "s1"){
          thisInfo.addPoint(thisData.s1s[index],
                            thisData.maxPeakAreas[index]);
        } else if(thisObs == "s2"){
          thisInfo.addPoint(thisData.s2s[index],
                            thisData.maxPeakAreas[index]);
        } else if(thisObs == "r"){
          thisInfo.addPoint(thisData.rs[index],
                            thisData.maxPeakAreas[index]);
        } else if(thisObs == "drift"){
          thisInfo.addPoint(thisData.drifts[index],
                            thisData.maxPeakAreas[index]);
        } else if(thisObs == "phi"){
          thisInfo.addPoint(thisData.phis[index],
                            thisData.maxPeakAreas[index]);
        }
      }

      // add histogram to the map
      efficiencies_before[thisName][thisObs] = thisInfo;
    } // end observables loop 
  } // end data set loop

  cout << "c14 counts before: " << data["C14"].keepIndex.size() << endl;
  // perform cut on each data set
  //for(std::pair<string, dataContainer> thisPair : data){
  for(auto it = data.begin(); it != data.end(); it++){
  string thisName = it->first;
    dataContainer& thisData = it->second;

    MaxPeakAreaCut mpaCut("utility/MaxPeakAreaParams/");
    mpaCut.execute(thisData, true);
  }
  cout << "c14 counts after: " << data["C14"].keepIndex.size() << endl;

  // get histograms after cuts
  unordered_map<string,
                unordered_map<string, HistInfo>  > efficiencies_after;
  // loop over data sets
  for(std::pair<string, dataContainer> thisPair : data){
    string thisName = thisPair.first;
    dataContainer& thisData = thisPair.second;
    
    // loop through observables
    for(string thisObs : observables){
      HistInfo thisInfo(histParams[thisObs].min,
                        histParams[thisObs].max,
                        histParams[thisObs].nBins);

      // add events to the histogram
      for(int aaEvent = 0; aaEvent < thisData.keepIndex.size(); aaEvent++){
        int index = thisData.keepIndex[aaEvent];
        if(thisObs == "s1"){
          thisInfo.addPoint(thisData.s1s[index],
                            thisData.maxPeakAreas[index]);
        } else if(thisObs == "s2"){
          thisInfo.addPoint(thisData.s2s[index],
                            thisData.maxPeakAreas[index]);
        } else if(thisObs == "r"){
          thisInfo.addPoint(thisData.rs[index],
                            thisData.maxPeakAreas[index]);
        } else if(thisObs == "drift"){
          thisInfo.addPoint(thisData.drifts[index],
                            thisData.maxPeakAreas[index]);
        } else if(thisObs == "phi"){
          thisInfo.addPoint(thisData.phis[index],
                            thisData.maxPeakAreas[index]);
        }
      }

      // add histogram to the map
      efficiencies_after[thisName][thisObs] = thisInfo;
    } // end observables loop 
  } // end data set loop


  // plot each observable, multiple sources on same graph
  for(string thisObs : observables){
    TCanvas* thisObsCanvas = new TCanvas();
    int colorCounter = 1;
    for(std::pair<string, dataContainer> thisPair : data){
      string thisName = thisPair.first;

      vector<double> xs =
        efficiencies_before[thisName][thisObs].getBinCenters();
      vector<int> counts_before =
        efficiencies_before[thisName][thisObs].getCounts();
      vector<int> counts_after =
        efficiencies_after[thisName][thisObs].getCounts();
      vector<double> effs;
      for(int aaBin = 0; aaBin < counts_before.size(); aaBin++){
        if(counts_before[aaBin] == 0){effs.push_back(-1);}
        else{
          effs.push_back((double)counts_after[aaBin] /
                         (double)counts_before[aaBin]);
        }
      }
      TGraph* thisSourceEffGraph = new TGraph(xs.size(),
                                              &xs[0],
                                              &effs[0]);
      thisSourceEffGraph->SetLineWidth(2);
      thisSourceEffGraph->SetLineColor(colorCounter);
      colorCounter++;
      if(colorCounter == 2){
        thisSourceEffGraph->GetYaxis()->SetRangeUser(0.9, 1);
        thisSourceEffGraph->Draw("al");
      }
      else{thisSourceEffGraph->Draw("samel");}  
    } // end source loop
    thisObsCanvas->SaveAs(("plots/MaxPeakAreaStudy/efficiency_" + thisObs +
                           ".png").c_str());
  } // end observables loop
  
}
