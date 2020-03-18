#include "SaveData.h"
#include "../Settings.h"

#include <string>
#include <vector>


#include "TFile.h"
#include "TTree.h"

using std::string;

void saveData(dataContainer &d, string cutName){

  // create variables to write to tree (abbreviated tree, only relevant data)
  Float_t s1;
  Float_t s1Raw;
  Int_t partner;
  Float_t promptFraction;
  Float_t maxPeakArea;
  Int_t aftT01;
  Int_t aftT05;
  Int_t aftT25;
  Int_t aftT50;
  Int_t aftT75;
  Int_t aftT95;
  Int_t aftT99;
  Int_t aftTlx;
  Int_t aftTrx;
  //  Int_t hftTmax;

  Float_t s2;
  Float_t s2Raw;
  Float_t r;
  Float_t phi;
  Float_t drift;
  Float_t z;
  Float_t rWall;
  Float_t s2GausSigma;
  Int_t s2AftT1;
  Int_t s2AftT0;
  Float_t chi2;
  Float_t eField;

  ULong64_t luxstamp;
  Int_t fileNum;
  Int_t close2End;
  Int_t excluded;

  Float_t goodArea;
  Float_t badArea;

  Int_t timeBin;
  Int_t zSlice;


  // create the tree itself
  string fileName = "data/EFTData_post_" + cutName + ".root";
  TFile* outFile = new TFile(fileName.c_str(), "RECREATE");
  TTree* outTree = new TTree("cutData", "cutData");

  outTree->Branch("luxstamp", &luxstamp, "luxstamp/l");
  outTree->Branch("fileNum", &fileNum, "fileNum/I");
  outTree->Branch("close2End", &close2End, "close2End/I");
  outTree->Branch("excluded", &excluded, "excluded/I");


  outTree->Branch("s1", &s1, "s1/F");
  outTree->Branch("s1Raw", &s1Raw, "s1Raw/F");
  outTree->Branch("partner", &partner, "partner/I");
  outTree->Branch("promptFraction", &promptFraction, "promptFraction/F");
  outTree->Branch("maxPeakArea", &maxPeakArea, "maxPeakArea/F");
  outTree->Branch("aftT01", &aftT01, "aftT01/I");
  outTree->Branch("aftT05", &aftT05, "aftT05/I");
  outTree->Branch("aftT25", &aftT25, "aftT25/I");
  outTree->Branch("aftT50", &aftT50, "aftT50/I");
  outTree->Branch("aftT75", &aftT75, "aftT75/I");
  outTree->Branch("aftT95", &aftT95, "aftT95/I");
  outTree->Branch("aftT99", &aftT99, "aftT99/I");
  outTree->Branch("aftTlx", &aftTlx, "aftTlx/I");
  outTree->Branch("aftTrx", &aftTrx, "aftTrx/I");
  //  outTree->Branch("hftTmax", &hftTmax, "hftTmax/I");
  
  outTree->Branch("s2", &s2, "s2/F");
  outTree->Branch("s2Raw", &s2Raw, "s2Raw/F");
  outTree->Branch("r", &r, "r/F");
  outTree->Branch("phi", &phi, "phi/F");
  outTree->Branch("drift", &drift, "drift/F");
  outTree->Branch("z", &z, "z/F");
  //outTree->Branch("rWall", &rWall, "rWall/F");
  outTree->Branch("s2GausSigma", &s2GausSigma, "s2GausSigma/F");
  outTree->Branch("s2AftT1", &s2AftT1, "s2AftT1/I");
  outTree->Branch("s2AftT0", &s2AftT0, "s2AftT0/I");
  outTree->Branch("chi2", &chi2, "chi2/F");
  outTree->Branch("eField", &eField, "eField/F");
                                      
  outTree->Branch("goodArea", &goodArea, "goodArea/F");
  outTree->Branch("badArea", &badArea, "badArea/F");

  outTree->Branch("timeBin", &timeBin, "timeBin/I");
  outTree->Branch("zSlice", &zSlice, "zSlice/I");


  // fill the tree with the passing events
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    
    s1 = d.s1s[index];
    s1Raw = d.s1Raws[index];
    partner = d.partners[index];
    promptFraction = d.promptFractions[index];
    maxPeakArea = d.maxPeakAreas[index];
    aftT01 = d.aft_t01s[index];
    aftT05 = d.aft_t05s[index];
    aftT25 = d.aft_t25s[index];
    aftT50 = d.aft_t50s[index];
    aftT75 = d.aft_t75s[index];
    aftT95 = d.aft_t95s[index];
    aftT99 = d.aft_t99s[index];
    aftTlx = d.aft_tlxs[index];
    aftTrx = d.aft_trxs[index];
    //    hftTmax = d.hft_tmaxs[index];

    s2 = d.s2s[index];
    s2Raw = d.s2Raws[index];
    r = d.rs[index];
    phi = d.phis[index];
    drift = d.drifts[index];
    z = d.zs[index];
    //rWall = d.rWalls[index];;
    s2GausSigma = d.s2GausSigmas[index];
    s2AftT1 = d.s2AftT1s[index];
    s2AftT0 = d.s2AftT0s[index];
    chi2 = d.chi2s[index];
    eField = d.eFields[index];

    luxstamp = d.luxstamps[index];
    fileNum = d.fileNums[index];
    close2End = d.close2Ends[index];
    excluded = d.excludeds[index];

    goodArea = d.goodAreas[index];
    badArea = d.badAreas[index];

    timeBin = d.timeBins[index];
    zSlice = d.zSlices[index];

    outTree->Fill();
  }
  outTree->Write();
  outFile->Close();
}
