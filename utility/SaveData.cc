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
  Int_t partner;
  Float_t promptFraction;
  Float_t maxPeakArea;

  Float_t s2;
  Float_t s2Raw;
  Float_t r;
  Float_t drift;
  Float_t rWall;
  Float_t s2GausSigma;
  Int_t s2AftT1;
  Int_t s2AftT0;

  ULong64_t luxstamp;
  Int_t close2End;
  Int_t excluded;

  Float_t goodArea;
  Float_t badArea;


  // create the tree itself
  string fileName = "data/EFTData_post_" + cutName + ".root";
  TFile* outFile = new TFile(fileName.c_str(), "RECREATE");
  TTree* outTree = new TTree("cutData", "cutData");

  outTree->Branch("luxstamp", &luxstamp, "luxstamp/l");
  outTree->Branch("close2End", &close2End, "close2End/I");
  outTree->Branch("excluded", &excluded, "excluded/I");

  outTree->Branch("s1", &s1, "s1/F");
  outTree->Branch("partner", &partner, "partner/I");
  outTree->Branch("promptFraction", &promptFraction, "promptFraction/F");
  outTree->Branch("maxPeakArea", &maxPeakArea, "maxPeakArea/F");

  outTree->Branch("s2", &s2, "s2/F");
  outTree->Branch("s2Raw", &s2Raw, "s2Raw/F");
  outTree->Branch("r", &r, "r/F");
  outTree->Branch("drift", &drift, "drift/F");
  outTree->Branch("rWall", &rWall, "rWall/F");
  outTree->Branch("s2GausSigma", &s2GausSigma, "s2GausSigma/F");
  outTree->Branch("s2AftT1", &s2AftT1, "s2AftT1/F");
  outTree->Branch("s2AftT0", &s2AftT0, "s2AftT0/F");
                                      
  outTree->Branch("goodArea", &goodArea, "goodArea/F");
  outTree->Branch("badArea", &badArea, "badArea/F");


  // fill the tree with the passing events
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    s1 = d.s1s[index];
    partner = d.partners[index];
    promptFraction = d.promptFractions[index];
    maxPeakArea = d.maxPeakAreas[index];

    s2 = d.s2s[index];
    s2Raw = d.s2Raws[index];
    r = d.rs[index];
    drift = d.drifts[index];
    rWall = d.rWalls[index];;
    s2GausSigma = d.s2GausSigmas[index];
    s2AftT1 = d.s2AftT1s[index];
    s2AftT0 = d.s2AftT0s[index];

    luxstamp = d.luxstamps[index];
    close2End = d.close2Ends[index];
    excluded = d.excludeds[index];

    goodArea = d.goodAreas[index];
    badArea = d.badAreas[index];

    outTree->Fill();
  }
  outTree->Write();
  outFile->Close();
}
