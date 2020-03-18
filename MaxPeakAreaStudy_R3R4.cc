#include <string>
#include <vector>
#include <iostream>

#include "Settings.h"
#include "utility/LoadData.h"
#include "utility/HistInfo.h"
#include "cuts/Cut.h"
#include "cuts/PromptFraction.h"

#include "TStopwatch.h"

#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1I.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;


int main(int argc, char* argv[]){

  // load run04 tritium data
  string tritiumFilenameR4 = "data/EFTData_BasePlus_Run04_CH3T.root";
  //string tritiumFilenameR4 = "data/EFTData_diffPF_R4.root";
  vector<string> tritiumFilenamesR4;
  tritiumFilenamesR4.push_back(tritiumFilenameR4);
 
  dataContainer dt4 = loadData(tritiumFilenamesR4);

  // load run03 tritium data
  string tritiumFilenameR3 = "data/EFTData_BasePlus_Run03_CH3T.root";
  //string tritiumFilenameR3 = "data/EFTData_diffPF_R3.root";
  vector<string> tritiumFilenamesR3;
  tritiumFilenamesR3.push_back(tritiumFilenameR3);
 
  dataContainer dt3 = loadData(tritiumFilenamesR3);


    // plot them both
  int index;

  int tBins = 198;
  int tMin = 1;
  int tMax = 100;

  HistInfo tInfoR4(tMin, tMax, tBins);
  for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
    index = dt4.keepIndex[aaEvent];
    tInfoR4.addPoint(dt4.s1s[index], dt4.maxPeakAreas[index]);
  }

  HistInfo tInfoR3(tMin, tMax, tBins);
  for(int aaEvent = 0; aaEvent < dt3.keepIndex.size(); aaEvent++){
    index = dt3.keepIndex[aaEvent];
    tInfoR3.addPoint(dt3.s1s[index], dt3.maxPeakAreas[index]);
  }

  // just plot s1 vs pf, just median and .99 quantile to keep it clean
  TGraph* s1MPAR4Graph = new TGraph(dt4.keepIndex.size());
  for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
    index = dt4.keepIndex[aaEvent];
    if(dt4.s1s[index] < tMin or dt4.s1s[index] > tMax){continue;}
    s1MPAR4Graph->SetPoint(aaEvent, dt4.s1s[index], dt4.maxPeakAreas[index]);
  }
  s1MPAR4Graph->SetMarkerColor(3);
  TGraph* s1MPAR3Graph = new TGraph(dt3.keepIndex.size());
  for(int aaEvent = 0; aaEvent < dt3.keepIndex.size(); aaEvent++){
    index = dt3.keepIndex[aaEvent];
    if(dt3.s1s[index] < tMin or dt3.s1s[index] > tMax){continue;}
    s1MPAR3Graph->SetPoint(aaEvent, dt3.s1s[index], dt3.maxPeakAreas[index]);
  }
 s1MPAR3Graph->SetMarkerColor(1);

 vector<double> t4Meds = tInfoR4.getQuantileSigma(0);
 vector<double> t499s = tInfoR4.getQuantile(.99);
 vector<double> t3Meds = tInfoR3.getQuantileSigma(0);
 vector<double> t399s = tInfoR3.getQuantile(.99);
 vector<double> tCenters = tInfoR4.getBinCenters();
 
 TGraph* t4MedGraph = new TGraph(tBins, &tCenters[0], &t4Meds[0]);
 t4MedGraph->SetLineColor(2);
 t4MedGraph->SetLineWidth(3);
 TGraph* t499Graph = new TGraph(tBins, &tCenters[0], &t499s[0]);
 t499Graph->SetLineColor(2);
 t499Graph->SetLineWidth(3);
 TGraph* t3MedGraph = new TGraph(tBins, &tCenters[0], &t3Meds[0]);
 t3MedGraph->SetLineColor(4);
 t3MedGraph->SetLineWidth(3);
 TGraph* t399Graph = new TGraph(tBins, &tCenters[0], &t399s[0]);
 t399Graph->SetLineColor(4);
 t399Graph->SetLineWidth(3);


 TCanvas* s1MPACanvas = new TCanvas();
 s1MPAR4Graph->Draw("ap");
 s1MPAR3Graph->Draw("psame");
 t4MedGraph->Draw("samel");
 t499Graph->Draw("samel");
 t3MedGraph->Draw("samel");
 t399Graph->Draw("samel");
 s1MPACanvas->SaveAs("plots/runCompare/s1MPATritium.png");

 //MPA fractions
 vector<double> MPAR3Fracs;
 HistInfo tFracInfoR3(tMin, tMax, tBins);
 TGraph* tFracR3Graph = new TGraph(dt3.keepIndex.size());
 for(int aaEvent = 0; aaEvent < dt3.keepIndex.size(); aaEvent++){
   index = dt3.keepIndex[aaEvent];
   double mpaFrac = dt3.maxPeakAreas[index]/dt3.s1s[index];
   MPAR3Fracs.push_back(mpaFrac);
   tFracInfoR3.addPoint(dt3.s1s[index],
                        mpaFrac);
   tFracR3Graph->SetPoint(aaEvent, dt3.s1s[index], mpaFrac);
 }
 HistInfo tFracInfoR4(tMin, tMax, tBins);
 TGraph* tFracR4Graph = new TGraph(dt4.keepIndex.size());
 vector<double> MPAR4Fracs;
 for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
   index = dt4.keepIndex[aaEvent];
   double mpaFrac = dt4.maxPeakAreas[index]/dt4.s1s[index];
   MPAR4Fracs.push_back(mpaFrac);
   tFracInfoR4.addPoint(dt4.s1s[index],
                        mpaFrac);
   tFracR4Graph->SetPoint(aaEvent, dt4.s1s[index], mpaFrac);
 }

 vector<double> tFrac4Meds = tFracInfoR4.getQuantileSigma(0);
 vector<double> tFrac499s = tFracInfoR4.getQuantile(.99);
 vector<double> tFrac3Meds = tFracInfoR3.getQuantileSigma(0);
 vector<double> tFrac399s = tFracInfoR3.getQuantile(.99);

 
 TGraph* tFrac4MedGraph = new TGraph(tBins, &tCenters[0], &tFrac4Meds[0]);
 tFrac4MedGraph->SetLineColor(2);
 tFrac4MedGraph->SetLineWidth(3);
 TGraph* tFrac499Graph = new TGraph(tBins, &tCenters[0], &tFrac499s[0]);
 tFrac499Graph->SetLineColor(2);
 tFrac499Graph->SetLineWidth(3);
 TGraph* tFrac3MedGraph = new TGraph(tBins, &tCenters[0], &tFrac3Meds[0]);
 tFrac3MedGraph->SetLineColor(4);
 tFrac3MedGraph->SetLineWidth(3);
 TGraph* tFrac399Graph = new TGraph(tBins, &tCenters[0], &tFrac399s[0]);
 tFrac399Graph->SetLineColor(4);
 tFrac399Graph->SetLineWidth(3);

 tFracR3Graph->SetMarkerColor(3);
 tFracR4Graph->SetMarkerColor(1);

 TCanvas* s1MPAFracCanvas = new TCanvas();
 tFracR4Graph->Draw("ap");
 tFracR3Graph->Draw("psame");
 tFrac4MedGraph->Draw("samel");
 tFrac499Graph->Draw("samel");
 tFrac3MedGraph->Draw("samel");
 tFrac399Graph->Draw("samel");
 s1MPAFracCanvas->SaveAs("plots/runCompare/s1MPAFTritium.png");


 // plot mpa vs drift with and without cutting out kr83m
 // think that may have been my previous problem
 TH2F* mpafDriftHist =
   new TH2F("mpafDrift", "mpafDrift", 26, 40, 300, 50, 0, .2);
 for(int aaEvent = 0; aaEvent < dt3.keepIndex.size(); aaEvent++){
   index = dt3.keepIndex[aaEvent];
   mpafDriftHist->Fill(dt3.drifts[index], MPAR3Fracs[index]);
 }
 mpafDriftHist->GetXaxis()->SetTitle("drift (us)");
 mpafDriftHist->GetYaxis()->SetTitle("MPA fraction");
 TCanvas* mpafDriftCanvas = new TCanvas();
 mpafDriftHist->Draw("colz");
 mpafDriftCanvas->SaveAs("plots/runCompare/Run03_MPAFDrift_WithKr.png");

 TH2F* mpafDriftCutHist =
   new TH2F("mpafDriftCut", "mpafDriftCut", 26, 40, 300, 50, 0, .2);
 for(int aaEvent = 0; aaEvent < dt3.keepIndex.size(); aaEvent++){
   index = dt3.keepIndex[aaEvent];
   if(dt3.s1s[index] > 100){continue;}
   mpafDriftCutHist->Fill(dt3.drifts[index], MPAR3Fracs[index]);
 }
 mpafDriftCutHist->GetXaxis()->SetTitle("drift (us)");
 mpafDriftCutHist->GetYaxis()->SetTitle("MPA fraction");
 TCanvas* mpafDriftCutCanvas = new TCanvas();
 mpafDriftCutHist->Draw("colz");
 mpafDriftCutCanvas->SaveAs("plots/runCompare/Run03_MPAFDrift_WithoutKr.png");

 // run04
 TH2F* mpafDriftR4Hist =
   new TH2F("mpafDrift", "mpafDrift", 26, 40, 300, 50, 0, .2);
 for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
   index = dt4.keepIndex[aaEvent];
   mpafDriftR4Hist->Fill(dt4.drifts[index], MPAR4Fracs[index]);
 }
 mpafDriftR4Hist->GetXaxis()->SetTitle("drift (us)");
 mpafDriftR4Hist->GetYaxis()->SetTitle("MPA fraction");
 TCanvas* mpafDriftR4Canvas = new TCanvas();
 mpafDriftR4Hist->Draw("colz");
 mpafDriftR4Canvas->SaveAs("plots/runCompare/Run04_MPAFDrift_WithKr.png");

 TH2F* mpafDriftCutR4Hist =
   new TH2F("mpafDriftCut", "mpafDriftCut", 26, 40, 300, 50, 0, .2);
 for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
   index = dt4.keepIndex[aaEvent];
   if(dt4.s1s[index] > 100){continue;}
   mpafDriftCutR4Hist->Fill(dt4.drifts[index], MPAR4Fracs[index]);
 }
 mpafDriftCutR4Hist->GetXaxis()->SetTitle("drift (us)");
 mpafDriftCutR4Hist->GetYaxis()->SetTitle("MPA fraction");
 TCanvas* mpafDriftCutR4Canvas = new TCanvas();
 mpafDriftCutR4Hist->Draw("colz");
 mpafDriftCutR4Canvas->SaveAs("plots/runCompare/Run04_MPAFDrift_WithoutKr.png");

 // run03 with only kr
 TH2F* mpafDriftCutKrHist =
   new TH2F("mpafDriftCut", "mpafDriftCut", 26, 40, 300, 50, 0, .2);
 for(int aaEvent = 0; aaEvent < dt3.keepIndex.size(); aaEvent++){
   index = dt3.keepIndex[aaEvent];
   if(dt3.s1s[index] < 100){continue;}
   mpafDriftCutKrHist->Fill(dt3.drifts[index], MPAR3Fracs[index]);
 }
 mpafDriftCutKrHist->GetXaxis()->SetTitle("drift (us)");
 mpafDriftCutKrHist->GetYaxis()->SetTitle("MPA fraction");
 TCanvas* mpafDriftCutKrCanvas = new TCanvas();
 mpafDriftCutKrHist->Draw("colz");
 mpafDriftCutKrCanvas->SaveAs("plots/runCompare/Run03_MPAFDrift_OnlyKr.png");

}
