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


double meanDriftAdjust(double drift, double mpa){
  return mpa/(0.0546048
              + 7.94486e-05 * drift
              - 7.78426e-07 * pow(drift, 2)
              + 2.65714e-09 * pow(drift, 3));
}

double medianDriftAdjust(double drift, double mpa){
  return mpa/(0.0465434
              + 3.42106e-05 * drift
              - 1.06475e-07 * pow(drift, 2)
              - 2.54004e-10 * pow(drift, 3)
              + 9.47525e-13 * pow(drift, 4)
              + 1.11788e-14 * pow(drift, 5));
}


int main(int argc, char* argv[]){
  TStopwatch timer;
  timer.Start();

  gStyle->SetOptStat(0);

  /*
    Tritium
  */
  
  // get the list of files to process from passed in args
  string tritiumFilename = "data/EFTData_BasePlus_Tritium.root";
  vector<string> tritiumFilenames;
  tritiumFilenames.push_back(tritiumFilename);
 

  // load in some data to be cut
  dataContainer dt = loadData(tritiumFilenames);

  //float myFloat = d.keepIndex[0];
  
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
  
  s1MPACanvas->SaveAs("plots/MPATritiumPlot.png");

  
  //****************
  //   C14
  //****************
// get the list of files to process from passed in args
  string c14Filename = "data/EFTData_BasePlus_C14.root";
  vector<string> c14Filenames;
  c14Filenames.push_back(c14Filename);
 

  // load in some data to be cut
  dataContainer dc = loadData(c14Filenames);

  // get quantiles
  int cBins = 100;
  int cMin = 1;
  int cMax = 300;
  HistInfo cHistInfo(cMin, cMax, cBins);
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    cHistInfo.addPoint(dc.s1s[index], dc.maxPeakAreas[index]);
  }
  
  //plot some stuff
  // plot the events
  TGraph* cGraph = new TGraph(dc.keepIndex.size());
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    if(dc.s1s[index] < cMin or dc.s1s[index] > cMax){continue;}
    cGraph->SetPoint(aaEvent, dc.s1s[index], dc.maxPeakAreas[index]);
  }

  // quantiles
   vector<double> cCenters = cHistInfo.getBinCenters();
  vector<double> cMeans = cHistInfo.getQuantileSigma(0);
  TGraph* cMeanGraph = new TGraph(cBins, &cCenters[0], &cMeans[0]);
  cMeanGraph->SetLineColor(2);
  cMeanGraph->SetLineWidth(2);
  vector<double> c2Sigs = cHistInfo.getQuantileSigma(2);
  TGraph* c2SigsGraph = new TGraph(cBins, &cCenters[0], &c2Sigs[0]);
  c2SigsGraph->SetLineColor(3);
  c2SigsGraph->SetLineWidth(2);
  vector<double> c3Sigs = cHistInfo.getQuantileSigma(3);
  TGraph* c3SigsGraph = new TGraph(cBins, &cCenters[0], &c3Sigs[0]);
  c3SigsGraph->SetLineColor(4);
  c3SigsGraph->SetLineWidth(2);
  vector<double> c99s = cHistInfo.getQuantile(.99);
    TGraph* c99Graph = new TGraph(cBins, &cCenters[0], &c99s[0]);
  c99Graph->SetLineColor(5);
  c99Graph->SetLineWidth(2);

  //draw
  TCanvas* cCanv = new TCanvas();
  TLegend* cS1MPALegend = new TLegend(.2, .5, .4, .9);
  cS1MPALegend->AddEntry(cGraph, "data", "p");
  cS1MPALegend->AddEntry(cMeanGraph, "median", "l");
  cS1MPALegend->AddEntry(c2SigsGraph, "2 sigma quantile", "l");
  cS1MPALegend->AddEntry(c99Graph, "99% quantile", "l");
  cS1MPALegend->AddEntry(c3SigsGraph, "3 sigma quantile", "l");
  cGraph->SetTitle("Carbon 14 MPA vs S1c;S1c (phd);Max Peak Area");
  cGraph->Draw("AP");
  cMeanGraph->Draw("sameL");
  c2SigsGraph->Draw("sameL");
  c3SigsGraph->Draw("sameL");
  c99Graph->Draw("samel");
  cS1MPALegend->Draw("same");
  cCanv->SaveAs("plots/Run03/MPAC14.png");


  // combine
  TCanvas* combCanv = new TCanvas();
  
  TLegend* ctS1MPALegend = new TLegend(.15, .45, .45, .9);
  ctS1MPALegend->AddEntry(tGraph, "CH3T data", "p");
  ctS1MPALegend->AddEntry(meanGraph, "CH3T median", "l");
  ctS1MPALegend->AddEntry(twoSigGraph, "CH3T 2 sigma quantile", "l");
  //ctS1MPALegend->AddEntry(t99Graph, "CH3T 99% quantile", "l");
  ctS1MPALegend->AddEntry(threeSigGraph, "CH3T 3 sigma quantile", "l");
  ctS1MPALegend->AddEntry(cGraph, "C14 data", "p");
  ctS1MPALegend->AddEntry(cMeanGraph, "C14 median", "l");
  ctS1MPALegend->AddEntry(c2SigsGraph, "C14 2 sigma quantile", "l");
  //ctS1MPALegend->AddEntry(c99Graph, "C14 99% quantile", "l");
  ctS1MPALegend->AddEntry(c3SigsGraph, "C14 3 sigma quantile", "l");
  
  cGraph->SetTitle("Tritium and Carbon 14 MPA vs S1c;S1c (phd);Max Peak Area");
  cGraph->Draw("AP");
  tGraph->SetMarkerColor(8);
  tGraph->Draw("P");
  cMeanGraph->SetLineColor(kRed);
  cMeanGraph->Draw("samel");
  c2SigsGraph->SetLineColor(kRed+1);
  c2SigsGraph->Draw("samel");
  c99Graph->SetLineColor(kRed+2);
  //c99Graph->Draw("samel");
  c3SigsGraph->SetLineColor(kRed+3);
  c3SigsGraph->Draw("samel");
  meanGraph->SetLineColor(kBlue);
  meanGraph->Draw("sameL");
  twoSigGraph->SetLineColor(kBlue+1);
  twoSigGraph->Draw("sameL");
  t99Graph->SetLineColor(kBlue+2);
  //t99Graph->Draw("samel");
  threeSigGraph->SetLineColor(kBlue+3);
  threeSigGraph->Draw("sameL");
  ctS1MPALegend->Draw("same");
  combCanv->SaveAs("plots/MPACombined.png");


  // try hist
  TH2D* combHist = new TH2D("combHist", "combHist", cBins, cMin, cMax, 80, 0, 40);
  TH2D* cHist = new TH2D("cHist", "cHist", cBins, cMin, cMax, 80, 0, 40);
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    if(dt.s1s[index] < tMin or dt.s1s[index] > tMax){continue;}
    combHist->Fill(dt.s1s[index], dt.maxPeakAreas[index]);
  }
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    if(dc.s1s[index] < cMin or dc.s1s[index] > cMax){continue;}
    combHist->Fill(dc.s1s[index], dc.maxPeakAreas[index]);
    cHist->Fill(dc.s1s[index], dc.maxPeakAreas[index]);
  }
  TCanvas* histCanv = new TCanvas();
  combHist->Draw("colz");
  cMeanGraph->Draw("samel");
  c2SigsGraph->Draw("samel");
  c3SigsGraph->SetLineColor(8);
  c3SigsGraph->Draw("samel");
  meanGraph->SetLineColor(5);
  meanGraph->Draw("sameL");
  twoSigGraph->SetLineColor(6);
  twoSigGraph->Draw("sameL");
  threeSigGraph->SetLineColor(7);
  threeSigGraph->Draw("sameL");
  histCanv->SaveAs("plots/MPAHistCombined.png");


  // try to fit
  TF1* f = new TF1("f", "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*pow(x,4)+[5]*pow(x,5)", 1, 300);
  c2SigsGraph->Fit(f);
  TF1* f2 = new TF1("f2", "[0]+[1]*x+[2]*log(x)", 1, 300);
  c2SigsGraph->Fit(f2);
  TF1* f3 = new TF1("f3", "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[5]*(x+[4])+[6]*(x+[4])*(x+[4])", 1, 300);
  c2SigsGraph->Fit(f3);



  TCanvas* fitCanv = new TCanvas();
  cHist->Draw("colz");
  f->SetLineColor(6);
  f->Draw("same");
  f2->SetLineColor(5);
  f2->Draw("same");
  f3->SetLineColor(7);
  f3->Draw("same");
  c2SigsGraph->Draw("samel");
  fitCanv->SaveAs("plots/MPAFit.png");

 
  

  int cBinsLE = 40;
  TGraph* c2SigsGraphLE = new TGraph(cBinsLE, &cCenters[0], &c2Sigs[0]);
  TGraph* c2SigsGraphHE = new TGraph(cBins - cBinsLE, &cCenters[cBinsLE],
                                     &c2Sigs[cBinsLE]);
  TGraph* LEClone = new TGraph(cBinsLE, &cCenters[0], &c2Sigs[0]);
  TGraph* HEClone = new TGraph(cBins - cBinsLE, &cCenters[cBinsLE],
                                     &c2Sigs[cBinsLE]);

  // fit seperately to low energy and high
  double cStepSize = (cMax - cMin)/cBins;
  TF1* fLE = new TF1("fLE", "[0]+[1]*x+[2]*x*x+[3]*x*x*x", cMin, cMin+(cBinsLE*cStepSize));
  TF1* fHE = new TF1("fHE", "[0]+[1]*x+[2]*x*x+[3]*x*x*x", cMin+(cBinsLE*cStepSize), cMax);
  c2SigsGraphLE->Fit(fLE);
  c2SigsGraphHE->Fit(fHE);

  TCanvas* newFitCanvas = new TCanvas();
  cHist->Draw("colz");
  fLE->SetLineColor(6);
  fHE->SetLineColor(7);
  LEClone->SetLineColor(3);
  LEClone->SetLineWidth(2);
  HEClone->SetLineWidth(2);
  HEClone->SetLineColor(2);
  fLE->Draw("same");
  fHE->Draw("same");
  LEClone->Draw("samel");
  HEClone->Draw("samel");
  //c2SigsGraph->Draw("samel");
  newFitCanvas->SaveAs("plots/MPADoubleFit.png");
  


 TCanvas* sFitCanv = new TCanvas();
  cHist->GetYaxis()->SetRangeUser(0, 10);
  cHist->GetXaxis()->SetRangeUser(0, 50);
  cHist->Draw("colz");
  f->Draw("same");
  f2->Draw("same");
  c2SigsGraph->Draw("samel");
  sFitCanv->SaveAs("plots/MPAFitZoom.png");


  // other observable efficiencies
  HistInfo preS2Info(200, 30000, 100);
  HistInfo postS2Info(200, 30000, 100);
  HistInfo preRInfo(0, 24, 24);
  HistInfo postRInfo(0, 24, 24);
  HistInfo preDriftInfo(40, 300, 26);
  HistInfo postDriftInfo(40, 300, 26);
  HistInfo prePhiInfo(0, 3.15, 20);
  HistInfo postPhiInfo(0, 3.15, 20);
  

  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    preS2Info.addPoint(dc.s2s[index], dc.maxPeakAreas[index]);
    preRInfo.addPoint(dc.rs[index], dc.maxPeakAreas[index]);
    preDriftInfo.addPoint(dc.drifts[index], dc.maxPeakAreas[index]);
    prePhiInfo.addPoint(dc.phis[index], dc.maxPeakAreas[index]);
  }


  // try fraction instead
  vector<double> MPAFracs;
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    MPAFracs.push_back(dc.maxPeakAreas[index]/dc.s1s[index]);
  }
  TH2D* fracHist = new TH2D("fracHist", "", cBins, cMin, cMax, 100, 0, 1);
  TH2D* fracDriftHist = new TH2D("fdh", "", 26, 40, 300, 50, 0, .2);
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    fracHist->Fill(dc.s1s[index], MPAFracs[aaEvent]);
    fracDriftHist->Fill(dc.drifts[index], MPAFracs[aaEvent]);
  }
  fracHist->GetXaxis()->SetTitle("S1c (phd)");
  fracHist->GetYaxis()->SetTitle("MPA Fraction");
  fracDriftHist->GetXaxis()->SetTitle("drift (us)");
  fracDriftHist->GetYaxis()->SetTitle("MPA Fraction");
  TCanvas* fracCanvas = new TCanvas();
  fracHist->Draw("colz");
  fracCanvas->SaveAs("plots/MPAFracs.png");
  TCanvas* fdCanvas = new TCanvas();
  fracDriftHist->Draw("colz");
  fdCanvas->SaveAs("plots/MPADrift.png");


  // try median
  HistInfo fdInfo(40, 300, 26);
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    fdInfo.addPoint(dc.drifts[index], MPAFracs[aaEvent]);
  }
  vector<double> fdCenters = fdInfo.getBinCenters();
  vector<double> fdMedians = fdInfo.getQuantileSigma(0);

  TGraph* fdMedGraph = new TGraph(fdCenters.size(), &fdCenters[0], &fdMedians[0]);
  TF1* ff = new TF1("ff", "[0]+[1]*x+[2]*x*x+[3]*x*x*x", 40, 300);
  TF1* ffMed = new TF1("ffMed", "[0]+[1]*x+[2]*x*x+[3]*x*x*x", 40, 300);
  fracDriftHist->Fit(ff);
  fdMedGraph->Fit(ffMed);
  TCanvas* fDFitCanvas = new TCanvas();
  fracDriftHist->Draw("colz");
  ff->Draw("same");
  fdMedGraph->SetLineWidth(2);
  fdMedGraph->Draw("samel");
  fDFitCanvas->SaveAs("plots/MPADriftFit.png");


  // adjust mpa for drift
  vector<double> preMeanAdjC14;
  vector<double> preMedAdjC14;
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    preMeanAdjC14.push_back(meanDriftAdjust(dc.drifts[index],
                                            MPAFracs[aaEvent]));
    preMedAdjC14.push_back(medianDriftAdjust(dc.drifts[index],
                                            MPAFracs[aaEvent]));
  }

  TH2D* meanAdjMPAHist = new TH2D("meanAMPAHist", "meanAMPAHist", 26, 40, 300, 50, 0, 5);
  TH2D* medianAdjMPAHist = new TH2D("medianAMPAHist", "medianAMPAHist", 26, 40, 300, 50, 0, 5);
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    meanAdjMPAHist->Fill(dc.drifts[index], preMeanAdjC14[aaEvent]);
    medianAdjMPAHist->Fill(dc.drifts[index], preMedAdjC14[aaEvent]);
  }
  TCanvas* meanAdjCanvas = new TCanvas();
  meanAdjMPAHist->Draw("colz");
  meanAdjCanvas->SaveAs("plots/meanAdjustedMPA.png");
   TCanvas* medianAdjCanvas = new TCanvas();
  medianAdjMPAHist->Draw("colz");
  medianAdjCanvas->SaveAs("plots/medianAdjustedMPA.png");


  // plot main thing again now that it is adjusted
  TH2D* s1AdjMPA = new TH2D("s1AdjMPA", "s1AdjMPA", cBins, cMin, cMax,
                            100, 0, 20);
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    s1AdjMPA->Fill(dc.s1s[index], preMedAdjC14[aaEvent]);
  }

  HistInfo cAdjInfo(cMin, cMax, cBins);
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    cAdjInfo.addPoint(dc.s1s[index], preMedAdjC14[aaEvent]);
  }
  vector<double> cAdjMeds = cAdjInfo.getQuantileSigma(0);
  vector<double> cAdj2Sig = cAdjInfo.getQuantileSigma(2);
  vector<double> cAdj3Sig = cAdjInfo.getQuantileSigma(3);
  vector<double> cAdj99 = cAdjInfo.getQuantile(.99);

  TGraph* cAdjMedGraph = new TGraph(cBins, &cCenters[0], &cAdjMeds[0]);
  TGraph* cAdj2SigGraph = new TGraph(cBins, &cCenters[0], &cAdj2Sig[0]);
  TGraph* cAdj3SigGraph = new TGraph(cBins, &cCenters[0], &cAdj3Sig[0]);
  TGraph* cAdj99Graph = new TGraph(cBins, &cCenters[0], &cAdj99[0]);
  
  TCanvas* s1AdjMPACanvas = new TCanvas();
  s1AdjMPA->Draw("colz");
  cAdjMedGraph->SetLineWidth(2);
  cAdjMedGraph->Draw("samel");
  cAdj2SigGraph->SetLineWidth(2);
  cAdj2SigGraph->SetLineColor(3);
  cAdj2SigGraph->Draw("samel");
  cAdj3SigGraph->SetLineWidth(2);
  cAdj3SigGraph->SetLineColor(5);
  cAdj3SigGraph->Draw("samel");
  cAdj99Graph->SetLineWidth(2);
  cAdj99Graph->SetLineColor(6);
  cAdj99Graph->Draw("samel");
  s1AdjMPACanvas->SaveAs("plots/s1AdjMPA.png");



  // fit this
  //TF1* fAdj = new TF1("fAdj", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)", 1, 300);
  //TF1* fAdj2 = new TF1("fAdj2", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)+[6]*pow(x,-1)+[7]*pow(x,-2)", 1, 300);
  TF1* fAdj3 = new TF1("fAdj3", "[0]+[1]*x+[2]*x*x+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)+[6]*pow(x,-1)+[7]*pow(x,-2)+[8]*pow(x,-3)", 1, 300);
  //cAdj99Graph->Fit(fAdj);
  //cAdj99Graph->Fit(fAdj2);
  cAdj99Graph->Fit(fAdj3);
  TCanvas* AdjFitCanvas = new TCanvas();
  s1AdjMPA->SetTitle("Cut on Adjusted Max Peak Area Fraction;S1c (phd);AdjMPA (phd)");
  s1AdjMPA->Draw("colz");
  //fAdj->SetLineColor(3);
  //fAdj->Draw("same");
  //fAdj2->SetLineColor(5);
  //fAdj2->Draw("same");
  cAdj99Graph->Draw("samel");
  TLegend* adjMPALegend = new TLegend(.6, .7, .9, .9);
  adjMPALegend->AddEntry(cAdj99Graph, "0.99 Quantile", "l");
  adjMPALegend->AddEntry(fAdj3, "Fit to 0.99 Quantile", "l");
  adjMPALegend->Draw("same");
  AdjFitCanvas->SaveAs("plots/s1MPAFitAdjusted.png");
  
  
  // perform cuts
  MaxPeakAreaCut mpaCut;
  mpaCut.execute(dc, false, true);

  
  HistInfo cPostInfo(cMin, cMax, cBins);
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    cPostInfo.addPoint(dc.s1s[index], dc.maxPeakAreas[index]);
  }


  // efficiencies
  vector<int> preCounts = cHistInfo.getCounts();
  vector<int> postCounts = cPostInfo.getCounts();

  // plot stats
  TGraph* totalCounts = new TGraph(preCounts.size());
  for(int aaBin = 0; aaBin < cCenters.size(); aaBin++){
    totalCounts->SetPoint(aaBin, cCenters[aaBin], (double)preCounts[aaBin]);
  }
  TCanvas* countsCanvas = new TCanvas();
  totalCounts->Draw("al");
  countsCanvas->SaveAs("plots/C14Counts.png");

  vector<double> cutEffs;
  for(int aaBin = 0; aaBin < cBins; aaBin++){
    if(preCounts[aaBin] == 0){cutEffs.push_back(0); continue;} //bad 
    cutEffs.push_back(1 - ((double)((preCounts[aaBin] - postCounts[aaBin])) /
                      ((double)preCounts[aaBin])));
  }
  TGraph* cutEff = new TGraph(cBins, &cCenters[0], &cutEffs[0]);
  TCanvas* effCanvas = new TCanvas();
  cutEff->Draw("al");
  effCanvas->SaveAs("plots/MPAS1Eff.png");


for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    postS2Info.addPoint(dc.s2s[index], dc.maxPeakAreas[index]);
    postRInfo.addPoint(dc.rs[index], dc.maxPeakAreas[index]);
    postDriftInfo.addPoint(dc.drifts[index], dc.maxPeakAreas[index]);
    postPhiInfo.addPoint(dc.phis[index], dc.maxPeakAreas[index]);
  }


 vector<int> preS2s = preS2Info.getCounts();
 vector<int> postS2s = postS2Info.getCounts();
 vector<double> s2Effs;
 vector<int> preRs = preRInfo.getCounts();
 vector<int> postRs = postRInfo.getCounts();
 vector<double> rEffs;
 vector<int> preDrifts = preDriftInfo.getCounts();
 vector<int> postDrifts = postDriftInfo.getCounts();
 vector<double> driftEffs;
 vector<int> prePhis = prePhiInfo.getCounts();
 vector<int> postPhis = postPhiInfo.getCounts();
 vector<double> phiEffs;

 for(int aaBin = 0; aaBin < preS2s.size(); aaBin++){
   if(preS2s[aaBin] == 0){s2Effs.push_back(0); continue;}
   s2Effs.push_back(1 - ((double)((preS2s[aaBin] - postS2s[aaBin])) /
                      ((double)preS2s[aaBin])));
 }
for(int aaBin = 0; aaBin < preRs.size(); aaBin++){
   if(preRs[aaBin] == 0){s2Effs.push_back(0); continue;}
   rEffs.push_back(1 - ((double)((preRs[aaBin] - postRs[aaBin])) /
                      ((double)preRs[aaBin])));
 }
 for(int aaBin = 0; aaBin < preDrifts.size(); aaBin++){
   if(preDrifts[aaBin] == 0){s2Effs.push_back(0); continue;}
   driftEffs.push_back(1 - ((double)((preDrifts[aaBin] - postDrifts[aaBin])) /
                      ((double)preDrifts[aaBin])));
 }
 for(int aaBin = 0; aaBin < prePhis.size(); aaBin++){
   if(prePhis[aaBin] == 0){s2Effs.push_back(0); continue;}
   phiEffs.push_back(1 - ((double)((prePhis[aaBin] - postPhis[aaBin])) /
                      ((double)prePhis[aaBin])));
 }

 vector<double> s2Centers = preS2Info.getBinCenters();
 TGraph* s2Eff = new TGraph(s2Centers.size(), &s2Centers[0], &s2Effs[0]);
 TCanvas* s2EffCanvas = new TCanvas();
 s2Eff->Draw("al");
 s2EffCanvas->SaveAs("plots/MPAS2Eff.png");

  vector<double> rCenters = preRInfo.getBinCenters();
 TGraph* rEff = new TGraph(rCenters.size(), &rCenters[0], &rEffs[0]);
 TCanvas* rEffCanvas = new TCanvas();
 rEff->Draw("al");
 rEffCanvas->SaveAs("plots/MPAREff.png");
 
  vector<double> driftCenters = preDriftInfo.getBinCenters();
 TGraph* driftEff = new TGraph(driftCenters.size(), &driftCenters[0], &driftEffs[0]);
 TCanvas* driftEffCanvas = new TCanvas();
 driftEff->Draw("al");
 driftEffCanvas->SaveAs("plots/MPADriftEff.png");
 
 vector<double> phiCenters = prePhiInfo.getBinCenters();
 TGraph* phiEff = new TGraph(phiCenters.size(), &phiCenters[0], &phiEffs[0]);
 TCanvas* phiEffCanvas = new TCanvas();
 phiEff->Draw("al");
 phiEffCanvas->SaveAs("plots/MPAPhiEff.png");


 // fit to the S2 efficiencies
 TF1* fS2Eff = new TF1("fS2Eff", "[0]*pow(x,-1)+[1]*pow(x,-2)+[2]+[3]*x", 348, 4000);
 s2Eff->Fit(fS2Eff, "R0");
 
 TCanvas* s2EffFitCanvas = new TCanvas();
 s2Eff->SetTitle("S2 Efficiency;S2 (phd);Fraction of Events Preserved");
 s2Eff->SetLineWidth(2);
 s2Eff->Draw("al");
 fS2Eff->SetLineWidth(3);
 fS2Eff->Draw("same");
 s2EffFitCanvas->SaveAs("plots/MPAS2EffFit.png");
 cout << "first bin center: " << s2Centers[0] << endl;
 cout << "lowest S2 Eff: " << s2Effs[0] << endl;
  

  cout << "Time to run: " << timer.RealTime() << " seconds" << endl;
  
  return 0;
}
