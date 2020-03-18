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

using std::vector;
using std::string;
using std::cout;
using std::endl;


// this is horribly coded, but i'm in a hurry :(
// probably will cost me time later on

int main(int argc, char* argv[]){
  TStopwatch timer;
  timer.Start();
  
  // load tritium data
  string tritiumFilename = "data/EFTData_post_BadAreaCut_Run03_CH3T.root";
  vector<string> tritiumFilenames;
  tritiumFilenames.push_back(tritiumFilename);
 
  dataContainer dt = loadData(tritiumFilenames);

  //********************
  // some initial plots
  //********************
  int index;

  int tBins = 198;
  int tMin = 1;
  int tMax = 100;

  // create object for easy quantile calculation
  HistInfo tInfo(tMin, tMax, tBins);
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    tInfo.addPoint(dt.s1s[index], dt.promptFractions[index]);
  }

  // basic tritium plots

  // tritium statistics
  vector<double> tCenters = tInfo.getBinCenters();
  vector<int> tPreCounts = tInfo.getCounts();
  TGraph* tCountGraph = new TGraph(tBins);
  for(int aaBin = 0; aaBin < tCenters.size(); aaBin++){
    tCountGraph->SetPoint(aaBin, tCenters[aaBin], (double)tPreCounts[aaBin]);
  }

  TCanvas* tCountsCanvas = new TCanvas();
  tCountGraph->Draw("al");
  tCountsCanvas->SaveAs("plots/pfTritiumCounts.png");

  // s1 vs pf
  TGraph* tGraph = new TGraph(dt.keepIndex.size());
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    if(dt.s1s[index] < tMin or dt.s1s[index] > tMax){continue;}
    tGraph->SetPoint(aaEvent, dt.s1s[index], dt.promptFractions[index]);
  }

  vector<double> tMeans = tInfo.getQuantileSigma(0);
  vector<double> t2Sigs = tInfo.getQuantileSigma(-2);
  vector<double> t3Sigs = tInfo.getQuantileSigma(-3);
  vector<double> t01s = tInfo.getQuantile(.01);

  TGraph* tMeanGraph = new TGraph(tBins, &tCenters[0], &tMeans[0]);
  tMeanGraph->SetLineColor(2);
  tMeanGraph->SetLineWidth(3);
  TGraph* t2SigGraph = new TGraph(tBins, &tCenters[0], &t2Sigs[0]);
  t2SigGraph->SetLineColor(3);
  t2SigGraph->SetLineWidth(3);
  TGraph* t3SigGraph = new TGraph(tBins, &tCenters[0], &t3Sigs[0]);
  t3SigGraph->SetLineColor(4);
  t3SigGraph->SetLineWidth(3);
  TGraph* t01Graph = new TGraph(tBins, &tCenters[0], &t01s[0]);
  t01Graph->SetLineColor(5);
  t01Graph->SetLineWidth(3);

  TCanvas* tS1PFCanvas = new TCanvas();
  tGraph->Draw("ap");
  tMeanGraph->Draw("samel");
  t2SigGraph->Draw("samel");
  t3SigGraph->Draw("samel");
  t01Graph->Draw("samel");
  tS1PFCanvas->SaveAs("plots/Run03/s1PFTritium.png");



  //**********
  // fit cuts
  //**********

  TF1* fTrit = new TF1("fTrit", "[0]*pow(x,-3)+[1]*pow(x,-2)+[2]*pow(x,-1)+[3]+[4]*x+[5]*pow(x,2)+[6]*pow(x,3)",1,100); //ok, lowest couple bins sketchy
  t01Graph->Fit(fTrit,"R0");
  // function from run04
  TF1* fTritRun04 = new TF1("fTritRun04","-3.21552*pow(x,-3)+5.13049*pow(x,-2)-2.28076*pow(x,-1)+.867027-0.00249864*x+5.56275e-5*pow(x,2)-4.28088e-7*pow(x,3)",1,40);
  TF1* fTritRun04HE = new TF1("fTritRun04HE","-4.68072*pow(x,-1)+.96818-0.00222693*x+1.13698e-5*pow(x,2)-1.83202e-8*pow(x,3)",40,100);

  TCanvas* tFitCanvas = new TCanvas();
  tGraph->Draw("ap");
  tGraph->GetXaxis()->SetRangeUser(1, 100);
  tGraph->Draw("ap");
  tFitCanvas->Update();
  t01Graph->SetLineColor(5);
  t01Graph->SetLineStyle(1);
  t01Graph->Draw("samel");
  fTrit->Draw("same");
  fTritRun04->SetLineColor(3);
  fTritRun04->Draw("same");
  fTritRun04HE->SetLineColor(3);
  fTritRun04HE->Draw("same");
  tFitCanvas->SaveAs("plots/Run03/PFTritiumFit.png");
  
  
  return 0;
}
