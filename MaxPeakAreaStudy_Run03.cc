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
#include "TStyle.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;


double medianDriftAdjust(double drift, double mpa){
  return mpa/(0.0912065
              + 0.00032629 * drift
              - 1.94849e-06 * pow(drift, 2)
              + 4.59999e-09 * pow(drift, 3));
}


int main(int argc, char* argv[]){

  gStyle->SetOptStat(0);

  // get files to process
  string tritiumFilename = "data/EFTData_post_BadAreaCut_Run03_CH3T.root";
  vector<string> tritiumFilenames;
  tritiumFilenames.push_back(tritiumFilename);

  // load in the data
  dataContainer dt = loadData(tritiumFilenames);

int tBins = 25;
  int tMin = 1;
  int tMax = 100;
  int index;
  HistInfo tHistInfo(1, 100, tBins);
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    tHistInfo.addPoint(dt.s1s[index], dt.maxPeakAreas[index]);
  }

 //plot some stuff
  // plot the events
  TGraph* tGraph = new TGraph(dt.keepIndex.size());
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    if(dt.s1s[index] < tMin or dt.s1s[index] > tMax){continue;}
    tGraph->SetPoint(aaEvent, dt.s1s[index], dt.maxPeakAreas[index]);
  }

  // plot the quantiles
  vector<double> binCenters = tHistInfo.getBinCenters();
  vector<double> means = tHistInfo.getQuantileSigma(0);
  TGraph* meanGraph = new TGraph(tBins, &binCenters[0], &means[0]);
  meanGraph->SetLineColor(2);
  meanGraph->SetLineWidth(2);
  vector<double> twoSigs = tHistInfo.getQuantileSigma(2);
  TGraph* twoSigGraph = new TGraph(tBins, &binCenters[0], &twoSigs[0]);
  twoSigGraph->SetLineColor(3);
  twoSigGraph->SetLineWidth(2);
  vector<double> threeSigs = tHistInfo.getQuantileSigma(3);
  TGraph* threeSigGraph = new TGraph(tBins, &binCenters[0], &threeSigs[0]);
  threeSigGraph->SetLineColor(4);
  threeSigGraph->SetLineWidth(2);
  vector<double> t99s = tHistInfo.getQuantile(.99);
  TGraph* t99Graph = new TGraph(tBins, &binCenters[0], &t99s[0]);
  t99Graph->SetLineColor(5);
  t99Graph->SetLineWidth(2);

 //plot them
  TCanvas* s1MPACanvas = new TCanvas();
  TLegend* tS1MPALegend = new TLegend(.2, .5, .4, .9);
  tS1MPALegend->AddEntry(tGraph, "data", "p");
  tS1MPALegend->AddEntry(meanGraph, "median", "l");
  tS1MPALegend->AddEntry(twoSigGraph, "2 sigma quantile", "l");
  tS1MPALegend->AddEntry(t99Graph, "99% quantile", "l");
  tS1MPALegend->AddEntry(threeSigGraph, "3 sigma quantile", "l");

  tGraph->SetTitle("CH3T MPA vs S1c;S1c (phd);Max Peak Area");
  tGraph->Draw("AP");
  meanGraph->Draw("sameL");
  twoSigGraph->Draw("sameL");
  threeSigGraph->Draw("sameL");
  t99Graph->Draw("sameL");
  tS1MPALegend->Draw("same");
  
  s1MPACanvas->SaveAs("plots/Run03/MPATritiumPlot.png");


  //look at mpa fraction
  vector<double> MPAFracs;
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    MPAFracs.push_back(dt.maxPeakAreas[index]/dt.s1s[index]);
  }
  TH2D* fracHist = new TH2D("fracHist", "", tBins, tMin, tMax, 100, 0, 1);
  TH2D* fracDriftHist = new TH2D("fdh", "", 26, 40, 300, 50, 0, .2);
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    fracHist->Fill(dt.s1s[index], MPAFracs[aaEvent]);
    fracDriftHist->Fill(dt.drifts[index], MPAFracs[aaEvent]);
  }
  fracHist->GetXaxis()->SetTitle("S1c (phd)");
  fracHist->GetYaxis()->SetTitle("MPA Fraction");
  fracDriftHist->GetXaxis()->SetTitle("drift (us)");
  fracDriftHist->GetYaxis()->SetTitle("MPA Fraction");
  TCanvas* fracCanvas = new TCanvas();
  fracHist->Draw("colz");
  fracCanvas->SaveAs("plots/Run03/MPAFracs.png");
  TCanvas* fdCanvas = new TCanvas();
  fracDriftHist->Draw("colz");
  fdCanvas->SaveAs("plots/Run03/MPADrift.png");


  // try median
  HistInfo fdInfo(40, 300, 26);
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    fdInfo.addPoint(dt.drifts[index], MPAFracs[aaEvent]);
  }
  vector<double> fdCenters = fdInfo.getBinCenters();
  vector<double> fdMedians = fdInfo.getQuantileSigma(0);

  TGraph* fdMedGraph = new TGraph(fdCenters.size(), &fdCenters[0], &fdMedians[0]);
  //TF1* ff = new TF1("ff", "[0]+[1]*x+[2]*x*x+[3]*x*x*x", 40, 300);
  TF1* ffMed = new TF1("ffMed", "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x+[5]*x*x*x*x*x", 40, 300);
  TF1* ffMed2 = new TF1("ffMed", "[0]+[1]*x+[2]*x*x+[3]*x*x*x", 40, 300);
  // include run04 for comparison
  TF1* ffRun04 = new TF1("ffRun04",".0465434+3.42106e-05*x-1.06475e-07*x*x-2.54004e-10*x*x*x+9.47525e-13*x*x*x*x+1.11788e-14*x*x*x*x*x", 40, 300);
  //fracDriftHist->Fit(ff);
  fdMedGraph->Fit(ffMed, "R0");
  fdMedGraph->Fit(ffMed2, "R0");
  TCanvas* fDFitCanvas = new TCanvas();
  fracDriftHist->Draw("colz");
  //ff->Draw("same");
  ffRun04->SetLineColor(3);
  ffRun04->SetLineWidth(2);
  ffRun04->Draw("same");
  ffMed->SetLineColor(2);
  ffMed->SetLineWidth(2);
  ffMed->Draw("same");
  ffMed2->SetLineColor(5);
  ffMed2->SetLineWidth(2);
  ffMed2->Draw("same");
  fdMedGraph->SetLineWidth(2);
  fdMedGraph->Draw("samel0");
  fDFitCanvas->SaveAs("plots/Run03/MPADriftFit.png");


  // adjust mpa for drift
  vector<double> preMedAdj;
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    preMedAdj.push_back(medianDriftAdjust(dt.drifts[index],
                                          MPAFracs[aaEvent]));
  }

  TH2D* medAdjMPAHist = new TH2D("medAMPAHist", "medAMPAHist", 26, 40, 300, 50, 0, 5);
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    medAdjMPAHist->Fill(dt.drifts[index], preMedAdj[aaEvent]);
  }
  TCanvas* medAdjCanvas = new TCanvas();
  medAdjMPAHist->Draw("colz");
  medAdjCanvas->SaveAs("plots/Run03/medianAdjustedMPA.png");



  // plot adjusted mpaf vs s1 now
  TH2D* s1AdjMPA = new TH2D("s1AdjMPA", "s1AdjMPA", tBins, tMin, tMax,
                            100, 0, 20);
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    s1AdjMPA->Fill(dt.s1s[index], preMedAdj[aaEvent]);
  }

  HistInfo tAdjInfo(tMin, tMax, tBins);
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    tAdjInfo.addPoint(dt.s1s[index], preMedAdj[aaEvent]);
  }
  
  vector<double> tAdjMeds = tAdjInfo.getQuantileSigma(0);
  vector<double> tAdj2Sig = tAdjInfo.getQuantileSigma(2);
  vector<double> tAdj3Sig = tAdjInfo.getQuantileSigma(3);
  vector<double> tAdj99 = tAdjInfo.getQuantile(.99);
  vector<double> tCenters = tAdjInfo.getBinCenters();

  TGraph* tAdjMedGraph = new TGraph(tBins, &tCenters[0], &tAdjMeds[0]);
  TGraph* tAdj2SigGraph = new TGraph(tBins, &tCenters[0], &tAdj2Sig[0]);
  TGraph* tAdj3SigGraph = new TGraph(tBins, &tCenters[0], &tAdj3Sig[0]);
  TGraph* tAdj99Graph = new TGraph(tBins, &tCenters[0], &tAdj99[0]);
  
  TCanvas* s1AdjMPACanvas = new TCanvas();
  s1AdjMPA->Draw("colz");
  tAdjMedGraph->SetLineWidth(2);
  tAdjMedGraph->Draw("samel");
  tAdj2SigGraph->SetLineWidth(2);
  tAdj2SigGraph->SetLineColor(3);
  tAdj2SigGraph->Draw("samel");
  tAdj3SigGraph->SetLineWidth(2);
  tAdj3SigGraph->SetLineColor(5);
  tAdj3SigGraph->Draw("samel");
  tAdj99Graph->SetLineWidth(2);
  tAdj99Graph->SetLineColor(6);
  tAdj99Graph->Draw("samel");
  s1AdjMPACanvas->SaveAs("plots/Run03/s1AdjMPA.png");


  // make a fit
  TF1* fAdj = new TF1("fAdj3", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)+[6]*pow(x,-1)+[7]*pow(x,-2)+[8]*pow(x,-3)", 1, 100);
  tAdj99Graph->Fit(fAdj,"R0");

  TCanvas* AdjFitCanvas = new TCanvas();
  s1AdjMPA->SetTitle("Cut on Adjusted Max Peak Area Fraction;S1c (phd);AdjMPA (phd)");
  s1AdjMPA->Draw("colz");
  tAdj99Graph->Draw("samel");
  fAdj->SetLineWidth(2);
  fAdj->SetLineColor(3);
  fAdj->Draw("same");
  AdjFitCanvas->SaveAs("plots/Run03/s1MPAFitAdjusted.png");



  //**********
  // fit cuts
  //**********

  TF1* fTrit = new TF1("fTrit", "[0]*pow(x,-3)+[1]*pow(x,-2)+[2]*pow(x,-1)+[3]+[4]*x+[5]*pow(x,2)+[6]*pow(x,3)",1,40); //ok, lowest couple bins sketchy
  t01Graph->Fit(fTrit,"R0");

  TCanvas* tFitCanvas = new TCanvas();
  tGraph->Draw("ap");
  tGraph->GetXaxis()->SetRangeUser(1, 45);
  tGraph->Draw("ap");
  tFitCanvas->Update();
  t01Graph->SetLineColor(4);
  t01Graph->SetLineStyle(1);
  t01Graph->Draw("samel");
  fTrit->Draw("same");
  tFitCanvas->SaveAs("plots/Run03/PFTritiumFit.png");
  

  
}
