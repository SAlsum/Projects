#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"

#define S1BINSIZE 5
#define S2BINSIZE 1000

using std::string;
using std::vector;
using std::cout;
using std::endl;


vector< vector<int> > loadS1S2Hist(string filename, int& total);


int main(int argc, char* argv[]){

  string preFile = argv[1];
  string postFile = argv[2];

  int preTotal = 0;
  vector< vector<int> > preCuts = loadS1S2Hist(preFile, preTotal);
  int postTotal = 0;
  vector< vector<int> > postCuts = loadS1S2Hist(postFile, postTotal);

  cout << "overall efficiency: "
       << (float)(preTotal - postTotal)/(float)preTotal << endl;

  gStyle->SetOptStat(0);
  
  TH2F* efficiencyHist = new TH2F("effHist", "effHist",
                                  preCuts.size(), 0,
                                  preCuts.size() * S1BINSIZE,
                                  preCuts[0].size(), 0,
                                  preCuts[0].size() * S2BINSIZE);
  for(int aaS1 = 0; aaS1 < preCuts.size(); aaS1++){
    for(int bbS2 = 0; bbS2 < preCuts[aaS1].size(); bbS2++){
      efficiencyHist->SetBinContent(aaS1, bbS2,
                                    (float)(preCuts[aaS1][bbS2] -
                                            postCuts[aaS1][bbS2]) /
                                    (float)preCuts[aaS1][bbS2]);
    }
  }

  TCanvas* effCanvas = new TCanvas();
  efficiencyHist->Draw("colz");
  effCanvas->SaveAs("efficiency.png");
  
}


vector< vector<int> > loadS1S2Hist(string filename, int& total){

  // set up array
  Float_t maxS1 = 300;
  Float_t s1BinSize = S1BINSIZE;
  Float_t maxS2 = 31600;
  Float_t s2BinSize = S2BINSIZE;
  
  // load in pre-cut data set
  vector<int> s2Setup(ceil(maxS2/s2BinSize), 0);
  vector< vector<int> > Cuts(ceil(maxS1/s1BinSize), s2Setup);

  // load data into these variables
  Float_t S1;
  Float_t S2;

  TFile* File = new TFile(filename.c_str());
  TTree* Tree = (TTree*)File->Get("cutData");
  const Int_t Entries = (Int_t)Tree->GetEntries();

  // set addresses
  Tree->SetBranchAddress("s1", &S1);
  Tree->SetBranchAddress("s2", &S2);

  // incriment bin if point lands inside
  for(int aaEvent = 0; aaEvent < Entries; aaEvent++){
    Tree->GetEntry(aaEvent);

    if(S1 < maxS1 and S2 < maxS2){
      Cuts[floor(S1/s1BinSize)][floor(S2/s2BinSize)]++;
      total++;
    }
  }

  return Cuts;
}
