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
    tInfoR4.addPoint(dt4.s1s[index], dt4.promptFractions[index]);
  }

  HistInfo tInfoR3(tMin, tMax, tBins);
  for(int aaEvent = 0; aaEvent < dt3.keepIndex.size(); aaEvent++){
    index = dt3.keepIndex[aaEvent];
    tInfoR3.addPoint(dt3.s1s[index], dt3.promptFractions[index]);
  }

  // just plot s1 vs pf, just median and .01 quantile to keep it clean
  TGraph* s1PFR4Graph = new TGraph(dt4.keepIndex.size());
  for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
    index = dt4.keepIndex[aaEvent];
    if(dt4.s1s[index] < tMin or dt4.s1s[index] > tMax){continue;}
    s1PFR4Graph->SetPoint(aaEvent, dt4.s1s[index], dt4.promptFractions[index]);
  }
  s1PFR4Graph->SetMarkerColor(3);
  TGraph* s1PFR3Graph = new TGraph(dt3.keepIndex.size());
  for(int aaEvent = 0; aaEvent < dt3.keepIndex.size(); aaEvent++){
    index = dt3.keepIndex[aaEvent];
    if(dt3.s1s[index] < tMin or dt3.s1s[index] > tMax){continue;}
    s1PFR3Graph->SetPoint(aaEvent, dt3.s1s[index], dt3.promptFractions[index]);
  }
 s1PFR3Graph->SetMarkerColor(1);

 vector<double> t4Meds = tInfoR4.getQuantileSigma(0);
 vector<double> t401s = tInfoR4.getQuantile(.01);
 vector<double> t3Meds = tInfoR3.getQuantileSigma(0);
 vector<double> t301s = tInfoR3.getQuantile(.01);
 vector<double> tCenters = tInfoR4.getBinCenters();
 
 TGraph* t4MedGraph = new TGraph(tBins, &tCenters[0], &t4Meds[0]);
 t4MedGraph->SetLineColor(2);
 t4MedGraph->SetLineWidth(3);
 TGraph* t401Graph = new TGraph(tBins, &tCenters[0], &t401s[0]);
 t401Graph->SetLineColor(2);
 t401Graph->SetLineWidth(3);
 TGraph* t3MedGraph = new TGraph(tBins, &tCenters[0], &t3Meds[0]);
 t3MedGraph->SetLineColor(4);
 t3MedGraph->SetLineWidth(3);
 TGraph* t301Graph = new TGraph(tBins, &tCenters[0], &t301s[0]);
 t301Graph->SetLineColor(4);
 t301Graph->SetLineWidth(3);


 TCanvas* s1PFCanvas = new TCanvas();
 s1PFR4Graph->Draw("ap");
 s1PFR3Graph->Draw("psame");
 t4MedGraph->Draw("samel");
 t401Graph->Draw("samel");
 t3MedGraph->Draw("samel");
 t301Graph->Draw("samel");
 s1PFCanvas->SaveAs("plots/runCompare/s1PFTritium.png");



 // only take the tritium data from run04 that is close in energy
 vector<int> newKeepIndex;
 for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
   index = dt4.keepIndex[aaEvent];
   if(dt4.eFields[index] < 16000 or dt4.eFields[index] > 20000){
     dt4.cutMask[index] = true;
   } else {newKeepIndex.push_back(index);}
 }
 dt4.keepIndex = newKeepIndex;


 // plot again with only the close in e-field events
 TGraph* s1PFR4EFGraph = new TGraph(dt4.keepIndex.size());
 for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
   index = dt4.keepIndex[aaEvent];
   if(dt4.s1s[index] < tMin or dt4.s1s[index] > tMax){continue;}
   s1PFR4EFGraph->SetPoint(aaEvent, dt4.s1s[index], dt4.promptFractions[index]);
 }
 s1PFR4EFGraph->SetMarkerColor(3);
 HistInfo tInfoR4EF(tMin, tMax, tBins);
 for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
   index = dt4.keepIndex[aaEvent];
   tInfoR4EF.addPoint(dt4.s1s[index], dt4.promptFractions[index]);
 }
 
 vector<double> t4EFMeds = tInfoR4EF.getQuantileSigma(0);
 vector<double> t4EF01s = tInfoR4EF.getQuantile(.01);
 TGraph* t4EFMedGraph = new TGraph(tBins, &tCenters[0], &t4EFMeds[0]);
 t4EFMedGraph->SetLineColor(2);
 t4EFMedGraph->SetLineWidth(3);
 TGraph* t4EF01Graph = new TGraph(tBins, &tCenters[0], &t4EF01s[0]);
 t4EF01Graph->SetLineColor(2);
 t4EF01Graph->SetLineWidth(3);

 TCanvas* s1PFEFAdjCanvas = new TCanvas();
 s1PFR4EFGraph->Draw("ap");
 s1PFR3Graph->Draw("psame");
 t4EFMedGraph->Draw("samel");
 t4EF01Graph->Draw("samel");
 t3MedGraph->Draw("samel");
 t301Graph->Draw("samel");
 s1PFEFAdjCanvas->SaveAs("plots/runCompare/s1PFTritium_similarEField.png");


 // compare before and after cutting to similar field tritium
 TCanvas* cutCompareCanvas = new TCanvas();
 s1PFR4Graph->SetMarkerColor(1);
 s1PFR4Graph->Draw("ap");
 s1PFR4EFGraph->Draw("psame");
 t4MedGraph->SetLineColor(4);
 t4MedGraph->Draw("samel");
 t401Graph->SetLineColor(4);
 t401Graph->Draw("samel");
 t4EFMedGraph->Draw("samel");
 t4EF01Graph->Draw("samel");
 cutCompareCanvas->SaveAs("plots/runCompare/r4_beforeAfter_similarFields.png");

 
 // look spatially at what events remain for run04
 TH2F* remSpatial = new TH2F("remSpatial", "remSpatial", 46, 0, 23, 26, 40, 300);
 for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
   index = dt4.keepIndex[aaEvent];
   remSpatial->Fill(dt4.rs[index], dt4.drifts[index]);
 }

 TCanvas* remSpatialCanvas = new TCanvas();
 remSpatial->Draw("colz");
 remSpatialCanvas->SaveAs("plots/runCompare/similarFieldSpatial.png");


 // try offset to see if it lines up after that
 double offset = .05;
 TGraph* t4MedOffsetGraph = new TGraph(tBins, &tCenters[0], &t4Meds[0]);
 t4MedOffsetGraph->SetLineColor(2);
 t4MedOffsetGraph->SetLineWidth(3);
 TGraph* t401OffsetGraph = new TGraph(tBins, &tCenters[0], &t401s[0]);
 t401OffsetGraph->SetLineColor(2);
 t401OffsetGraph->SetLineWidth(3);
 for(int aaPt = 0; aaPt < tBins; aaPt++){
   Double_t x, y;
   t4MedOffsetGraph->GetPoint(aaPt, x, y);
   t4MedOffsetGraph->SetPoint(aaPt, x, y - offset);
   t401OffsetGraph->GetPoint(aaPt, x, y);
   t401OffsetGraph->SetPoint(aaPt, x, y - offset);
 }
 
 TCanvas* s1PFOffsetCanvas = new TCanvas();
 s1PFR4Graph->Draw("ap");
 s1PFR3Graph->Draw("psame");
 t4MedOffsetGraph->Draw("samel");
 t401OffsetGraph->Draw("samel");
 t3MedGraph->Draw("samel");
 t301Graph->Draw("samel");
 s1PFOffsetCanvas->SaveAs("plots/runCompare/s1PFTritium_Offset.png");




 
 // plot the difference between aft_tlx and aft_t05 for each
 TH1I* r4Tlx = new TH1I("r4Tlx", "r4Tlx", 80, -30000, 50000);
 TH1I* r4T05 = new TH1I("r4T05", "r4T05", 80, -30000, 50000);
 TH1I* r4tDiff = new TH1I("r4tDiff", "r4tDiff", 30, -5, 5);
 for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
   index = dt4.keepIndex[aaEvent];
   r4Tlx->Fill(dt4.aft_tlxs[index]);
   r4T05->Fill(dt4.aft_t05s[index]);
   r4tDiff->Fill(dt4.aft_tlxs[index] - dt4.aft_t05s[index]);
 }

 TH1I* r3Tlx = new TH1I("r4Tlx", "r4Tlx", 80, -30000, 50000);
 TH1I* r3T05 = new TH1I("r4T05", "r4T05", 80, -30000, 50000);
 TH1I* r3tDiff = new TH1I("r3tDiff", "r3tDiff", 30, -5, 5);
 for(int aaEvent = 0; aaEvent < dt3.keepIndex.size(); aaEvent++){
   index = dt3.keepIndex[aaEvent];
   r3Tlx->Fill(dt3.aft_tlxs[index]);
   r3T05->Fill(dt3.aft_t05s[index]);
   r3tDiff->Fill(dt3.aft_tlxs[index] - dt3.aft_t05s[index]);
 }
 
 TCanvas* tlsR4Canvas = new TCanvas();
 r4Tlx->SetLineColor(2);
 r4Tlx->Draw();
 r4T05->Draw("same");
 tlsR4Canvas->SaveAs("plots/runCompare/tlsR4.png");
 
 TCanvas* tlsCompCanvas = new TCanvas();
 r4Tlx->SetLineColor(1);
 r4Tlx->Draw();
 r4T05->SetLineColor(2);
 r4T05->Draw("same");
 r3Tlx->SetLineColor(3);
 r3Tlx->Draw("same");
 r3T05->Draw("same");
 tlsCompCanvas->SaveAs("plots/runCompare/tlsComp.png");

 TCanvas* tlDiffCanvas = new TCanvas();
 r4tDiff->SetLineColor(2);
 r4tDiff->Draw();
 r3tDiff->Draw("same");
 tlDiffCanvas->SaveAs("plots/runCompare/tDiffCompare.png");


 // compare tlx and trx to each benchmark for each run
 TH1I* r301Diff = new TH1I("r301Diff", "r301Diff", 20, -5, 5);
 TH1I* r305Diff = new TH1I("r305Diff", "r305Diff", 20, -5, 5);
 TH1I* r325Diff = new TH1I("r325Diff", "r325Diff", 20, -5, 5);
 TH1I* r350Diff = new TH1I("r350Diff", "r350Diff", 20, -5, 5);
 TH1I* r375Diff = new TH1I("r375Diff", "r375Diff", 20, -5, 5);
 TH1I* r395Diff = new TH1I("r395Diff", "r395Diff", 20, -5, 5);
 TH1I* r399Diff = new TH1I("r399Diff", "r399Diff", 20, -5, 5);

 TH1I* r301rDiff = new TH1I("r301rDiff", "r301rDiff", 20, -5, 5);
 TH1I* r305rDiff = new TH1I("r305rDiff", "r305rDiff", 20, -5, 5);
 TH1I* r325rDiff = new TH1I("r325rDiff", "r325rDiff", 20, -5, 5);
 TH1I* r350rDiff = new TH1I("r350rDiff", "r350rDiff", 20, -5, 5);
 TH1I* r375rDiff = new TH1I("r375rDiff", "r375rDiff", 20, -5, 5);
 TH1I* r395rDiff = new TH1I("r395rDiff", "r395rDiff", 20, -5, 5);
 TH1I* r399rDiff = new TH1I("r399rDiff", "r399rDiff", 20, -5, 5);

for(int aaEvent = 0; aaEvent < dt3.keepIndex.size(); aaEvent++){
   index = dt3.keepIndex[aaEvent];
   r301Diff->Fill(dt3.aft_tlxs[index] - dt3.aft_t01s[index]);
   r305Diff->Fill(dt3.aft_tlxs[index] - dt3.aft_t05s[index]);
   r325Diff->Fill(dt3.aft_tlxs[index] - dt3.aft_t25s[index]);
   r350Diff->Fill(dt3.aft_tlxs[index] - dt3.aft_t50s[index]);
   r375Diff->Fill(dt3.aft_tlxs[index] - dt3.aft_t75s[index]);
   r395Diff->Fill(dt3.aft_tlxs[index] - dt3.aft_t95s[index]);
   r399Diff->Fill(dt3.aft_tlxs[index] - dt3.aft_t99s[index]);

   r301rDiff->Fill(dt3.aft_trxs[index] - dt3.aft_t01s[index]);
   r305rDiff->Fill(dt3.aft_trxs[index] - dt3.aft_t05s[index]);
   r325rDiff->Fill(dt3.aft_trxs[index] - dt3.aft_t25s[index]);
   r350rDiff->Fill(dt3.aft_trxs[index] - dt3.aft_t50s[index]);
   r375rDiff->Fill(dt3.aft_trxs[index] - dt3.aft_t75s[index]);
   r395rDiff->Fill(dt3.aft_trxs[index] - dt3.aft_t95s[index]);
   r399rDiff->Fill(dt3.aft_trxs[index] - dt3.aft_t99s[index]);
 }

 TCanvas* r3tlsCompareCanvas = new TCanvas();
 r301Diff->SetLineColor(1);
 r301Diff->Draw();
 r305Diff->SetLineColor(2);
 r305Diff->Draw("same");
 r325Diff->SetLineColor(3);
 r325Diff->Draw("same");
 r350Diff->SetLineColor(4);
 r350Diff->Draw("same");
 r375Diff->SetLineColor(5);
 r375Diff->Draw("same");
 r395Diff->SetLineColor(6);
 r395Diff->Draw("same");
 r399Diff->SetLineColor(7);
 r399Diff->Draw("same");
 r3tlsCompareCanvas->SaveAs("plots/runCompare/Run03_tlx_comparison.png");

TCanvas* r3trsCompareCanvas = new TCanvas();
 r301rDiff->SetLineColor(1);
 r301rDiff->Draw();
 r305rDiff->SetLineColor(2);
 r305rDiff->Draw("same");
 r325rDiff->SetLineColor(3);
 r325rDiff->Draw("same");
 r350rDiff->SetLineColor(4);
 r350rDiff->Draw("same");
 r375rDiff->SetLineColor(5);
 r375rDiff->Draw("same");
 r395rDiff->SetLineColor(6);
 r395rDiff->Draw("same");
 r399rDiff->SetLineColor(7);
 r399rDiff->Draw("same");
 r3trsCompareCanvas->SaveAs("plots/runCompare/Run03_trx_comparison.png");


 TH1I* r401Diff = new TH1I("r401Diff", "r401Diff", 20, -5, 5);
 TH1I* r405Diff = new TH1I("r405Diff", "r405Diff", 20, -5, 5);
 TH1I* r425Diff = new TH1I("r425Diff", "r425Diff", 20, -5, 5);
 TH1I* r450Diff = new TH1I("r450Diff", "r450Diff", 20, -5, 5);
 TH1I* r475Diff = new TH1I("r475Diff", "r475Diff", 20, -5, 5);
 TH1I* r495Diff = new TH1I("r495Diff", "r495Diff", 20, -5, 5);
 TH1I* r499Diff = new TH1I("r499Diff", "r499Diff", 20, -5, 5);

 TH1I* r401rDiff = new TH1I("r401rDiff", "r401rDiff", 20, -5, 5);
 TH1I* r405rDiff = new TH1I("r405rDiff", "r405rDiff", 20, -5, 5);
 TH1I* r425rDiff = new TH1I("r425rDiff", "r425rDiff", 20, -5, 5);
 TH1I* r450rDiff = new TH1I("r450rDiff", "r450rDiff", 20, -5, 5);
 TH1I* r475rDiff = new TH1I("r475rDiff", "r475rDiff", 20, -5, 5);
 TH1I* r495rDiff = new TH1I("r495rDiff", "r495rDiff", 20, -5, 5);
 TH1I* r499rDiff = new TH1I("r499rDiff", "r499rDiff", 20, -5, 5);

for(int aaEvent = 0; aaEvent < dt4.keepIndex.size(); aaEvent++){
   index = dt4.keepIndex[aaEvent];
   r401Diff->Fill(dt4.aft_tlxs[index] - dt4.aft_t01s[index]);
   r405Diff->Fill(dt4.aft_tlxs[index] - dt4.aft_t05s[index]);
   r425Diff->Fill(dt4.aft_tlxs[index] - dt4.aft_t25s[index]);
   r450Diff->Fill(dt4.aft_tlxs[index] - dt4.aft_t50s[index]);
   r475Diff->Fill(dt4.aft_tlxs[index] - dt4.aft_t75s[index]);
   r495Diff->Fill(dt4.aft_tlxs[index] - dt4.aft_t95s[index]);
   r499Diff->Fill(dt4.aft_tlxs[index] - dt4.aft_t99s[index]);

   r401rDiff->Fill(dt4.aft_trxs[index] - dt4.aft_t01s[index]);
   r405rDiff->Fill(dt4.aft_trxs[index] - dt4.aft_t05s[index]);
   r425rDiff->Fill(dt4.aft_trxs[index] - dt4.aft_t25s[index]);
   r450rDiff->Fill(dt4.aft_trxs[index] - dt4.aft_t50s[index]);
   r475rDiff->Fill(dt4.aft_trxs[index] - dt4.aft_t75s[index]);
   r495rDiff->Fill(dt4.aft_trxs[index] - dt4.aft_t95s[index]);
   r499rDiff->Fill(dt4.aft_trxs[index] - dt4.aft_t99s[index]);
 }

 TCanvas* r4tlsCompareCanvas = new TCanvas();
 r401Diff->SetLineColor(1);
 r401Diff->Draw();
 r405Diff->SetLineColor(2);
 r405Diff->Draw("same");
 r425Diff->SetLineColor(3);
 r425Diff->Draw("same");
 r450Diff->SetLineColor(4);
 r450Diff->Draw("same");
 r475Diff->SetLineColor(5);
 r475Diff->Draw("same");
 r495Diff->SetLineColor(6);
 r495Diff->Draw("same");
 r499Diff->SetLineColor(7);
 r499Diff->Draw("same");
 r4tlsCompareCanvas->SaveAs("plots/runCompare/Run04_tlx_comparison.png");

TCanvas* r4trsCompareCanvas = new TCanvas();
 r401rDiff->SetLineColor(1);
 r401rDiff->Draw();
 r405rDiff->SetLineColor(2);
 r405rDiff->Draw("same");
 r425rDiff->SetLineColor(3);
 r425rDiff->Draw("same");
 r450rDiff->SetLineColor(4);
 r450rDiff->Draw("same");
 r475rDiff->SetLineColor(5);
 r475rDiff->Draw("same");
 r495rDiff->SetLineColor(6);
 r495rDiff->Draw("same");
 r499rDiff->SetLineColor(7);
 r499rDiff->Draw("same");
 r4trsCompareCanvas->SaveAs("plots/runCompare/Run04_trx_comparison.png");

 
}
