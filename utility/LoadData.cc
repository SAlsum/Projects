#include "LoadData.h"
#include "../Settings.h"

#include <string>
#include <vector>
#include <iostream>

#include "TFile.h"
#include "TTree.h"


using std::string;
using std::vector;
using std::cout;
using std::endl;


dataContainer loadData(std::vector<string> fileList){
  // determine (just based on file names for now) what type of input we have
  // only allow 1 cut file
  if(fileList.size() == 1){
    // get the base name for the first file assuming it is well formed
    string baseName0 = fileList[0];
    if(baseName0.rfind("/") != baseName0.npos){
      baseName0 = baseName0.substr(baseName0.rfind("/")+1,
                                   baseName0.rfind(".")
                                   - (baseName0.rfind("/") + 1));
    }
    string f7 = baseName0.substr(0, 7);
    // cut files will start with 'EFTData'
    if(f7 == "EFTData"){
      return loadCutData(fileList[0]);
    }
  } 
  for(int aaFile = 0; aaFile < fileList.size(); aaFile++){
    // check to ensure files follow naming convention for rq files.  If not,
    // throw an error.
  }
  return loadRQData(fileList);
}


dataContainer loadRQData(std::vector<string> fileList){
  // load data from list of rq files into one dataContainer object
  dataContainer d;

  // set up variables to load data into
  Float_t inS1;
  Int_t inPartners;
  Float_t inPromptFraction;
  Float_t inMaxPeakArea;

  Float_t inS2;
  Float_t inS2Raw;
  Float_t inR;
  Float_t inDrift;
  Float_t inRWall;
  Float_t inS2GausSigma;
  Int_t inS2AftT1s;
  Int_t inS2AftT0s;

  ULong64_t inLuxstamp;
  Int_t inClose2End;
  Int_t inExcluded;

  Float_t inGoodArea;
  Float_t inBadArea;

  // keep track of total events saved
  int evtCount = 0;
  
  // loop through input files
  for(int aaFile = 0; aaFile < fileList.size(); aaFile++){
    // load the file
    TFile* inFile = new TFile(fileList[aaFile].c_str());
    cout << "loading from file: " << fileList[aaFile] << endl; 

    // get info from trees
    TTree* s1Tree = (TTree*)inFile->Get("1S1");
    TTree* s2Tree = (TTree*)inFile->Get("1S2");
    TTree* eventTree = (TTree*)inFile->Get("event");
    TTree* pulseTree = (TTree*)inFile->Get("true_pulse");
    const Int_t nEntries = (Int_t)s1Tree->GetEntries(); //should be same for all trees

    // set addresses
    s1Tree->SetBranchAddress("area_z_cor", &inS1);
    s1Tree->SetBranchAddress("partner", &inPartners);
    s1Tree->SetBranchAddress("prompt_fraction_tlx", &inPromptFraction);
    s1Tree->SetBranchAddress("max_peak_area", &inMaxPeakArea);

    s2Tree->SetBranchAddress("area_z_cor", &inS2);
    s2Tree->SetBranchAddress("area", &inS2Raw);
    s2Tree->SetBranchAddress("r_raw", &inR);
    s2Tree->SetBranchAddress("drift", &inDrift);
    s2Tree->SetBranchAddress("r_raw_wall", &inRWall);
    s2Tree->SetBranchAddress("gaus_fit_sigma", &inS2GausSigma);
    s2Tree->SetBranchAddress("aft_t1_samples", &inS2AftT1s);
    s2Tree->SetBranchAddress("aft_t0_samples", &inS2AftT0s);

    eventTree->SetBranchAddress("luxstamp", &inLuxstamp);
    eventTree->SetBranchAddress("close2End", &inClose2End);
    eventTree->SetBranchAddress("excluded", &inExcluded);

    pulseTree->SetBranchAddress("goodarea", &inGoodArea);
    pulseTree->SetBranchAddress("badarea", &inBadArea);

    // store in the data object's vectors
    for(Int_t bbEvent = 0; bbEvent < nEntries; bbEvent++){
    s1Tree->GetEntry(bbEvent);
    s2Tree->GetEntry(bbEvent);
    eventTree->GetEntry(bbEvent);
    pulseTree->GetEntry(bbEvent);

    d.s1s.push_back(inS1);
    d.partners.push_back(inPartners);
    d.promptFractions.push_back(inPromptFraction);
    d.maxPeakAreas.push_back(inMaxPeakArea);
    
    d.s2s.push_back(inS2);
    d.s2Raws.push_back(inS2Raw);
    d.rs.push_back(inR);
    d.drifts.push_back(inDrift);
    d.rWalls.push_back(inRWall);
    d.s2GausSigmas.push_back(inS2GausSigma);
    d.s2AftT1s.push_back(inS2AftT1s);
    d.s2AftT0s.push_back(inS2AftT0s);

    d.luxstamps.push_back(inLuxstamp);
    d.close2Ends.push_back(inClose2End);
    d.excludeds.push_back(inExcluded);

    d.goodAreas.push_back(inGoodArea);
    d.badAreas.push_back(inBadArea);
    
    d.cutMask.push_back(false);

    // record index of each event
    d.keepIndex.push_back(evtCount);
    evtCount++;

    if(bbEvent % 1000000 == 0){cout << "loading event " << bbEvent << endl;}
    } // end event loop
  } // end file loop
  
  return d;
}


dataContainer loadCutData(string inFile){
  // load data from cut file into a dataContainer object
  dataContainer d;

  // set up variables to load data into
  Float_t inS1;
  Int_t inPartners;
  Float_t inPromptFraction;
  Float_t inMaxPeakArea;

  Float_t inS2;
  Float_t inS2Raw;
  Float_t inR;
  Float_t inDrift;
  Float_t inRWall;
  Float_t inS2GausSigma;
  Float_t inS2AftT1s;
  Float_t inS2AftT0s;

  ULong64_t inLuxstamp;
  Int_t inClose2End;
  Int_t inExcluded;

  Float_t inGoodArea;
  Float_t inBadArea;

  // load the input files
  TFile* inTFile = new TFile(inFile.c_str());
  cout << "loading from file: " << inFile << endl;
  TTree* dataTree = (TTree*)inTFile->Get("cutData");
  const Int_t nEntries = (Int_t)dataTree->GetEntries();

  // set addresses
  dataTree->SetBranchAddress("s1", &inS1);
  dataTree->SetBranchAddress("partner", &inPartners);
  dataTree->SetBranchAddress("promptFraction", &inPromptFraction);
  dataTree->SetBranchAddress("maxPeakArea", &inMaxPeakArea);

  dataTree->SetBranchAddress("s2", &inS2);
  dataTree->SetBranchAddress("s2Raw", &inS2Raw);
  dataTree->SetBranchAddress("r", &inR);
  dataTree->SetBranchAddress("drift", &inDrift);
  dataTree->SetBranchAddress("rWall", &inRWall);
  dataTree->SetBranchAddress("s2GausSigma", &inS2GausSigma);
  dataTree->SetBranchAddress("s2AftT1", &inS2AftT1s);
  dataTree->SetBranchAddress("s2AftT0", &inS2AftT0s);

  dataTree->SetBranchAddress("luxstamp", &inLuxstamp);
  dataTree->SetBranchAddress("close2End", &inClose2End);
  dataTree->SetBranchAddress("excluded", &inExcluded);

  dataTree->SetBranchAddress("goodArea", &inGoodArea);
  dataTree->SetBranchAddress("badArea", &inBadArea);

  // store data in object's vectors
  for(Int_t aaEvent = 0; aaEvent < nEntries; aaEvent++){
    dataTree->GetEntry(aaEvent);

    d.s1s.push_back(inS1);
    d.partners.push_back(inPartners);
    d.promptFractions.push_back(inPromptFraction);
    d.maxPeakAreas.push_back(inMaxPeakArea);
    
    d.s2s.push_back(inS2);
    d.s2Raws.push_back(inS2Raw);
    d.rs.push_back(inR);
    d.drifts.push_back(inDrift);
    d.rWalls.push_back(inRWall);
    d.s2GausSigmas.push_back(inS2GausSigma);
    d.s2AftT1s.push_back(inS2AftT1s);
    d.s2AftT0s.push_back(inS2AftT0s);

    d.luxstamps.push_back(inLuxstamp);
    d.close2Ends.push_back(inClose2End);
    d.excludeds.push_back(inExcluded);

    d.goodAreas.push_back(inGoodArea);
    d.badAreas.push_back(inBadArea);
    
    d.cutMask.push_back(false);

    d.keepIndex.push_back(aaEvent);

    if(aaEvent % 1000000 == 0){cout << "loading event " << aaEvent << endl;}
  } // end event loop
  
  return d;
}
