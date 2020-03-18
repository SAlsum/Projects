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
  string tritiumFilename = "data/EFTData_BasePlus_Tritium.root";
  vector<string> tritiumFilenames;
  tritiumFilenames.push_back(tritiumFilename);
 
  dataContainer dt = loadData(tritiumFilenames);

  // load c14 data
  string c14Filename = "data/EFTData_BasePlus_C14.root";
  vector<string> c14Filenames;
  c14Filenames.push_back(c14Filename);
 
  dataContainer dc = loadData(c14Filenames);

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
  tS1PFCanvas->SaveAs("plots/s1PFTritium.png");


// basic c14 plots

  int cBins = 100;
  int cMin = 1;
  int cMax = 300;

  // create object for easy quantile calculation
  HistInfo cInfo(cMin, cMax, cBins);
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    cInfo.addPoint(dc.s1s[index], dc.promptFractions[index]);
  }
  
  // c14 statistics
  vector<double> cCenters = cInfo.getBinCenters();
  vector<int> cPreCounts = cInfo.getCounts();
  TGraph* cCountGraph = new TGraph(cBins);
  for(int aaBin = 0; aaBin < cCenters.size(); aaBin++){
    cCountGraph->SetPoint(aaBin, cCenters[aaBin], (double)cPreCounts[aaBin]);
  }

  TCanvas* cCountsCanvas = new TCanvas();
  cCountGraph->Draw("al");
  cCountsCanvas->SaveAs("plots/pfCarbon14Counts.png");

  // s1 vs pf
  TGraph* cGraph = new TGraph(dc.keepIndex.size());
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    if(dc.s1s[index] < cMin or dc.s1s[index] > cMax){continue;}
    cGraph->SetPoint(aaEvent, dc.s1s[index], dc.promptFractions[index]);
  }

  vector<double> cMeans = cInfo.getQuantileSigma(0);
  vector<double> c2Sigs = cInfo.getQuantileSigma(-2);
  vector<double> c3Sigs = cInfo.getQuantileSigma(-3);
  vector<double> c01s = cInfo.getQuantile(.01);

  TGraph* cMeanGraph = new TGraph(cBins, &cCenters[0], &cMeans[0]);
  cMeanGraph->SetLineColor(2);
  cMeanGraph->SetLineWidth(3);
  TGraph* c2SigGraph = new TGraph(cBins, &cCenters[0], &c2Sigs[0]);
  c2SigGraph->SetLineColor(3);
  c2SigGraph->SetLineWidth(3);
  TGraph* c3SigGraph = new TGraph(cBins, &cCenters[0], &c3Sigs[0]);
  c3SigGraph->SetLineColor(4);
  c3SigGraph->SetLineWidth(3);
  TGraph* c01Graph = new TGraph(cBins, &cCenters[0], &c01s[0]);
  c01Graph->SetLineColor(5);
  c01Graph->SetLineWidth(3);

  TCanvas* cS1PFCanvas = new TCanvas();
  cGraph->Draw("ap");
  cMeanGraph->Draw("samel");
  c2SigGraph->Draw("samel");
  c3SigGraph->Draw("samel");
  c01Graph->Draw("samel");
  cS1PFCanvas->SaveAs("plots/s1PFCarbon14.png");


  // both together
  TCanvas* ctS1PFCanvas = new TCanvas();
  cGraph->Draw("ap");
  tGraph->SetMarkerColor(8);
  tGraph->Draw("samep");
  cMeanGraph->SetLineColor(4);
  cMeanGraph->Draw("samel");
  tMeanGraph->SetLineColor(2);
  tMeanGraph->Draw("samel");
  c2SigGraph->SetLineColor(4);
  c2SigGraph->SetLineStyle(2);
  c2SigGraph->Draw("samel");
  t2SigGraph->SetLineColor(2);
  t2SigGraph->SetLineStyle(2);
  t2SigGraph->Draw("samel");
  c01Graph->SetLineColor(4);
  c01Graph->SetLineStyle(4);
  c01Graph->Draw("samel");
  t01Graph->SetLineColor(2);
  t01Graph->SetLineStyle(4);
  t01Graph->Draw("samel");
  ctS1PFCanvas->SaveAs("plots/s1PFCombined.png");

   // same but looking at lower s1
  TCanvas* ctS1PFZoomedCanvas = new TCanvas();
  cGraph->Draw("ap");
  cGraph->GetXaxis()->SetRangeUser(1, 20);
  cGraph->Draw("ap");
  ctS1PFZoomedCanvas->Update();
  tGraph->SetMarkerColor(8);
  tGraph->Draw("samep");
  cMeanGraph->SetLineColor(4);
  cMeanGraph->Draw("samel");
  tMeanGraph->SetLineColor(2);
  tMeanGraph->Draw("samel");
  c2SigGraph->SetLineColor(4);
  c2SigGraph->SetLineStyle(2);
  c2SigGraph->Draw("samel");
  t2SigGraph->SetLineColor(2);
  t2SigGraph->SetLineStyle(2);
  t2SigGraph->Draw("samel");
  c01Graph->SetLineColor(4);
  c01Graph->SetLineStyle(4);
  c01Graph->Draw("samel");
  t01Graph->SetLineColor(2);
  t01Graph->SetLineStyle(4);
  t01Graph->Draw("samel");
  ctS1PFZoomedCanvas->SaveAs("plots/s1PFCombinedZoomed.png");


  //***********************
  // look for correlations
  //***********************
  
  // plot pf vs other observables to see if there is a correlation
  TH2D* rPFHist = new TH2D("rPFHist", "rPFHist", 24, 0, 24, 50, .2, 1.6);
  TH2D* driftPFHist = new TH2D("driftPFHist", "driftPFHist", 26, 40, 300,
                               50, .2, 1.6);
  TH2D* phiPFHist = new TH2D("phiPFHist", "phiPFHist", 20, 0, 3.15,
                             50, .2, 1.6);
  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    rPFHist->Fill(dc.rs[index], dc.promptFractions[index]);
    driftPFHist->Fill(dc.drifts[index], dc.promptFractions[index]);
    phiPFHist->Fill(dc.phis[index], dc.promptFractions[index]);
  }

  TCanvas* rPFCanvas = new TCanvas();
  rPFHist->Draw("colz");
  rPFCanvas->SaveAs("plots/rPF.png");
  TCanvas* driftPFCanvas = new TCanvas();
  driftPFHist->Draw("colz");
  driftPFCanvas->SaveAs("plots/driftPF.png");
  TCanvas* phiPFCanvas = new TCanvas();
  phiPFHist->Draw("colz");
  phiPFCanvas->SaveAs("plots/phiPF.png");


  // no obvious correlation, so continue with just the pf


  //**********
  // fit cuts
  //**********
  
  // calibrations differ slightly at low energy, suspect due to low stats.
  // use the .01 for tritium at low energy until stats get low ~40 s1
  // then use the .01 for c14 from there out, should match fairly well

  // fit funcitons to the calibrations
  // tritium first
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
  tFitCanvas->SaveAs("plots/PFTritiumFit.png");

  // now c14
  TF1* fC14 = new TF1("fC14","[0]*pow(x,-1)+[1]+[2]*x+[3]*pow(x,2)+[4]*pow(x,3)",40,300);
  c01Graph->Fit(fC14, "R0");

  TCanvas* cFitCanvas = new TCanvas();
  cGraph->Draw("ap");
  cGraph->GetXaxis()->SetRangeUser(35, 300);
  cGraph->Draw("ap");
  cFitCanvas->Update();
  c01Graph->SetLineColor(7);
  c01Graph->SetLineStyle(1);
  c01Graph->Draw("samel");
  fC14->Draw("same");
  cFitCanvas->SaveAs("plots/PFC14Fit.png");


  // plot them together
  TCanvas* combinedFitCanvas = new TCanvas();
  cGraph->Draw("ap");
  cGraph->GetXaxis()->SetRangeUser(1, 300);
  cGraph->Draw("ap");
  combinedFitCanvas->Update();
  tGraph->Draw("samep");
  c01Graph->Draw("samel");
  t01Graph->Draw("samel");
  fC14->Draw("same");
  fTrit->Draw("same");
  combinedFitCanvas->SaveAs("plots/PFCombinedFit.png");


  //**********************************
  // calculate and check efficiencies
  //**********************************

  // bin data in each observable
  HistInfo preS2TInfo(200, 10000, 100);
  HistInfo postS2TInfo(200, 10000, 100);
  HistInfo preRTInfo(0, 24, 24);
  HistInfo postRTInfo(0, 24, 24);
  HistInfo preDriftTInfo(40, 300, 26);
  HistInfo postDriftTInfo(40, 300, 26);
  HistInfo prePhiTInfo(0, 3.15, 20);
  HistInfo postPhiTInfo(0, 3.15, 20);
  HistInfo preS2CInfo(200, 30000, 100);
  HistInfo postS2CInfo(200, 30000, 100);
  HistInfo preRCInfo(0, 24, 24);
  HistInfo postRCInfo(0, 24, 24);
  HistInfo preDriftCInfo(40, 300, 26);
  HistInfo postDriftCInfo(40, 300, 26);
  HistInfo prePhiCInfo(0, 3.15, 20);
  HistInfo postPhiCInfo(0, 3.15, 20);

  for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    preS2CInfo.addPoint(dc.s2s[index], dc.promptFractions[index]);
    preRCInfo.addPoint(dc.rs[index], dc.promptFractions[index]);
    preDriftCInfo.addPoint(dc.drifts[index], dc.promptFractions[index]);
    prePhiCInfo.addPoint(dc.phis[index], dc.promptFractions[index]);
  }
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    // don't worry about higher energy for tritium, some krypton slipped in
    // and dominates the higher energies
    if(dt.s1s[index] > 100){continue;}
    preS2TInfo.addPoint(dt.s2s[index], dt.promptFractions[index]);
    preRTInfo.addPoint(dt.rs[index], dt.promptFractions[index]);
    preDriftTInfo.addPoint(dt.drifts[index], dt.promptFractions[index]);
    prePhiTInfo.addPoint(dt.phis[index], dt.promptFractions[index]);
  }
  
  
  // perform cuts
  PromptFractionCut pfCut;
  pfCut.execute(dt, false, true);
  pfCut.execute(dc, false, true);

  
  // bin remaining data
  HistInfo tPostInfo(tMin, tMax, tBins);
  HistInfo cPostInfo(cMin, cMax, cBins);

    for(int aaEvent = 0; aaEvent < dc.keepIndex.size(); aaEvent++){
    index = dc.keepIndex[aaEvent];
    cPostInfo.addPoint(dc.s1s[index], dc.promptFractions[index]);
    postS2CInfo.addPoint(dc.s2s[index], dc.promptFractions[index]);
    postRCInfo.addPoint(dc.rs[index], dc.promptFractions[index]);
    postDriftCInfo.addPoint(dc.drifts[index], dc.promptFractions[index]);
    postPhiCInfo.addPoint(dc.phis[index], dc.promptFractions[index]);
  }
  for(int aaEvent = 0; aaEvent < dt.keepIndex.size(); aaEvent++){
    index = dt.keepIndex[aaEvent];
    // don't worry about higher energy for tritium, some krypton slipped in
    // and dominates the higher energies
    if(dt.s1s[index] > 100){continue;}
    tPostInfo.addPoint(dt.s1s[index], dt.promptFractions[index]);
    postS2TInfo.addPoint(dt.s2s[index], dt.promptFractions[index]);
    postRTInfo.addPoint(dt.rs[index], dt.promptFractions[index]);
    postDriftTInfo.addPoint(dt.drifts[index], dt.promptFractions[index]);
    postPhiTInfo.addPoint(dt.phis[index], dt.promptFractions[index]);
  }

  // get counts for each bin before and after
  vector<int> preS1TCounts = tInfo.getCounts();
  vector<int> postS1TCounts = tPostInfo.getCounts();
  vector<double> s1TEffs;
  vector<int> preS1CCounts = cInfo.getCounts();
  vector<int> postS1CCounts = cPostInfo.getCounts();
  vector<double> s1CEffs;

  vector<int> preS2TCounts = preS2TInfo.getCounts();
  vector<int> postS2TCounts = postS2TInfo.getCounts();
  vector<double> s2TEffs;
  vector<int> preS2CCounts = preS2CInfo.getCounts();
  vector<int> postS2CCounts = postS2CInfo.getCounts();
  vector<double> s2CEffs;

  vector<int> preRTCounts = preRTInfo.getCounts();
  vector<int> postRTCounts = postRTInfo.getCounts();
  vector<double> rTEffs;
  vector<int> preRCCounts = preRCInfo.getCounts();
  vector<int> postRCCounts = postRCInfo.getCounts();
  vector<double> rCEffs;

  vector<int> preDriftTCounts = preDriftTInfo.getCounts();
  vector<int> postDriftTCounts = postDriftTInfo.getCounts();
  vector<double> driftTEffs;
  vector<int> preDriftCCounts = preDriftCInfo.getCounts();
  vector<int> postDriftCCounts = postDriftCInfo.getCounts();
  vector<double> driftCEffs;

  vector<int> prePhiTCounts = prePhiTInfo.getCounts();
  vector<int> postPhiTCounts = postPhiTInfo.getCounts();
  vector<double> phiTEffs;
  vector<int> prePhiCCounts = prePhiCInfo.getCounts();
  vector<int> postPhiCCounts = postPhiCInfo.getCounts();
  vector<double> phiCEffs;

  // calculate the efficiencies for each
  // s1
  for(int aaBin = 0; aaBin < preS1TCounts.size(); aaBin++){
    if(preS1TCounts[aaBin] == 0){s1TEffs.push_back(0); continue;}
    s1TEffs.push_back(1 - (((double)(preS1TCounts[aaBin] -
                                    postS1TCounts[aaBin])) /
                           ((double)preS1TCounts[aaBin])));
  }
  for(int aaBin = 0; aaBin < preS1CCounts.size(); aaBin++){
    if(preS1CCounts[aaBin] == 0){s1CEffs.push_back(0); continue;}
    s1CEffs.push_back(1 - (((double)(preS1CCounts[aaBin] -
                                    postS1CCounts[aaBin])) /
                           ((double)preS1CCounts[aaBin])));
  }
  // s2
  for(int aaBin = 0; aaBin < preS2TCounts.size(); aaBin++){
    if(preS2TCounts[aaBin] == 0){s2TEffs.push_back(0); continue;}
    s2TEffs.push_back(1 - (((double)(preS2TCounts[aaBin] -
                                    postS2TCounts[aaBin])) /
                           ((double)preS2TCounts[aaBin])));
  }
  for(int aaBin = 0; aaBin < preS2CCounts.size(); aaBin++){
    if(preS2CCounts[aaBin] == 0){s2CEffs.push_back(0); continue;}
    s2CEffs.push_back(1 - (((double)(preS2CCounts[aaBin] -
                                    postS2CCounts[aaBin])) /
                           ((double)preS2CCounts[aaBin])));
  }
  // r
  for(int aaBin = 0; aaBin < preRTCounts.size(); aaBin++){
    if(preRTCounts[aaBin] == 0){rTEffs.push_back(0); continue;}
    rTEffs.push_back(1 - (((double)(preRTCounts[aaBin] -
                                    postRTCounts[aaBin])) /
                           ((double)preRTCounts[aaBin])));
  }
  for(int aaBin = 0; aaBin < preRCCounts.size(); aaBin++){
    if(preRCCounts[aaBin] == 0){rCEffs.push_back(0); continue;}
    rCEffs.push_back(1 - (((double)(preRCCounts[aaBin] -
                                    postRCCounts[aaBin])) /
                           ((double)preRCCounts[aaBin])));
  }
  // drift
  for(int aaBin = 0; aaBin < preDriftTCounts.size(); aaBin++){
    if(preDriftTCounts[aaBin] == 0){driftTEffs.push_back(0); continue;}
    driftTEffs.push_back(1 - (((double)(preDriftTCounts[aaBin] -
                                    postDriftTCounts[aaBin])) /
                           ((double)preDriftTCounts[aaBin])));
  }
  for(int aaBin = 0; aaBin < preDriftCCounts.size(); aaBin++){
    if(preDriftCCounts[aaBin] == 0){driftCEffs.push_back(0); continue;}
    driftCEffs.push_back(1 - (((double)(preDriftCCounts[aaBin] -
                                    postDriftCCounts[aaBin])) /
                           ((double)preDriftCCounts[aaBin])));
  }
  // phi
  for(int aaBin = 0; aaBin < prePhiTCounts.size(); aaBin++){
    if(prePhiTCounts[aaBin] == 0){phiTEffs.push_back(0); continue;}
    phiTEffs.push_back(1 - (((double)(prePhiTCounts[aaBin] -
                                    postPhiTCounts[aaBin])) /
                           ((double)prePhiTCounts[aaBin])));
  }
  for(int aaBin = 0; aaBin < prePhiCCounts.size(); aaBin++){
    if(prePhiCCounts[aaBin] == 0){phiCEffs.push_back(0); continue;}
    phiCEffs.push_back(1 - (((double)(prePhiCCounts[aaBin] -
                                    postPhiCCounts[aaBin])) /
                           ((double)prePhiCCounts[aaBin])));
  }

  // plot them
  // s1
  TGraph* s1TEff = new TGraph(tCenters.size(), &tCenters[0], &s1TEffs[0]);
  TGraph* s1CEff = new TGraph(cCenters.size(), &cCenters[0], &s1CEffs[0]);
  TCanvas* s1EffCanvas = new TCanvas();
  s1CEff->SetLineColor(7);
  s1CEff->Draw("al");
  s1TEff->SetLineColor(4);
  s1TEff->Draw("samel");
  s1EffCanvas->SaveAs("plots/PFS1Efficiency.png");

  // s2
  vector<double> s2TCenters = preS2TInfo.getBinCenters();
  vector<double> s2CCenters = preS2CInfo.getBinCenters();
  TGraph* s2TEff = new TGraph(s2TCenters.size(), &s2TCenters[0], &s2TEffs[0]);
  TGraph* s2CEff = new TGraph(s2CCenters.size(), &s2CCenters[0], &s2CEffs[0]);
  TCanvas* s2EffCanvas = new TCanvas();
  s2CEff->SetLineColor(7);
  s2CEff->SetLineWidth(2);
  s2CEff->Draw("al");
  s2TEff->SetLineColor(4);
  s2TEff->SetLineWidth(2);
  s2TEff->Draw("samel");
  s2EffCanvas->SaveAs("plots/PFS2Efficiency.png");

  // drift
    vector<double> driftTCenters = preDriftTInfo.getBinCenters();
  vector<double> driftCCenters = preDriftCInfo.getBinCenters();
  TGraph* driftTEff = new TGraph(driftTCenters.size(), &driftTCenters[0], &driftTEffs[0]);
  TGraph* driftCEff = new TGraph(driftCCenters.size(), &driftCCenters[0], &driftCEffs[0]);
  TCanvas* driftEffCanvas = new TCanvas();
  driftCEff->SetLineColor(7);
  driftCEff->Draw("al");
  driftTEff->SetLineColor(4);
  driftTEff->Draw("samel");
  driftEffCanvas->SaveAs("plots/PFDriftEfficiency.png");

   // r
    vector<double> rTCenters = preRTInfo.getBinCenters();
  vector<double> rCCenters = preRCInfo.getBinCenters();
  TGraph* rTEff = new TGraph(rTCenters.size(), &rTCenters[0], &rTEffs[0]);
  TGraph* rCEff = new TGraph(rCCenters.size(), &rCCenters[0], &rCEffs[0]);
  TCanvas* rEffCanvas = new TCanvas();
  rCEff->SetLineColor(7);
  rCEff->Draw("al");
  rTEff->SetLineColor(4);
  rTEff->Draw("samel");
  rEffCanvas->SaveAs("plots/PFREfficiency.png");

   // phi
    vector<double> phiTCenters = prePhiTInfo.getBinCenters();
  vector<double> phiCCenters = prePhiCInfo.getBinCenters();
  TGraph* phiTEff = new TGraph(phiTCenters.size(), &phiTCenters[0], &phiTEffs[0]);
  TGraph* phiCEff = new TGraph(phiCCenters.size(), &phiCCenters[0], &phiCEffs[0]);
  TCanvas* phiEffCanvas = new TCanvas();
  phiCEff->SetLineColor(7);
  phiCEff->Draw("al");
  phiTEff->SetLineColor(4);
  phiTEff->Draw("samel");
  phiEffCanvas->SaveAs("plots/PFPhiEfficiency.png");


  //S2 efficiencies
  cout << "first bin center: " << s2TCenters[0] << " " << s2CCenters[0]
       << endl;
  cout << "lowest S2 Eff: " << s2TEffs[0] << " " << s2CEffs[0] << endl;

 TF1* fS2Eff = new TF1("fS2Eff", "[0]*pow(x,-2)+[1]*pow(x,-1)+[2]+[3]*x", 248, 2000);
 s2TEff->Fit(fS2Eff, "R0");
  
  TCanvas* s2EffFitCanvas = new TCanvas();
  s2CEff->SetLineColor(7);
  s2CEff->SetLineWidth(2);
  s2TEff->SetLineColor(4);
  s2TEff->SetLineWidth(2);
  s2TEff->SetTitle("S2 Efficiency;S2 (phd);Fraction of Events Preserved");
  s2TEff->Draw("al");
  s2CEff->Draw("samel");
  fS2Eff->Draw("same");
  s2EffFitCanvas->SaveAs("plots/PFS2FitEfficiency.png");

  
  
  cout << "Time to run: " << timer.RealTime() << " seconds" << endl;
  
  return 0;
}
