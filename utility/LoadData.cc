#include "LoadData.h"
#include "../Settings.h"

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

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
  Float_t inS1Raw;
  Int_t inPartners;
  Float_t inPromptFraction;
  Float_t inMaxPeakArea;
  Int_t inAftT01;
  Int_t inAftT05;
  Int_t inAftT25;
  Int_t inAftT50;
  Int_t inAftT75;
  Int_t inAftT95;
  Int_t inAftT99;
  Int_t inAftTlx;
  Int_t inAftTrx;
  //  Int_t inHftTmax;

  Float_t inS2;
  Float_t inS2Raw;
  Float_t inR;
  Float_t inPhi;
  Float_t inX;
  Float_t inY;
  Float_t inDrift;
  //Float_t inXJPM;
  //Float_t inYJPM;
  Float_t inZ;
  bool calcZFlag = false;
  //Float_t inRWall;
  Float_t inS2GausSigma;
  Int_t inS2AftT1s;
  Int_t inS2AftT0s;
  Float_t inChi2;
  Float_t inEField;

  ULong64_t inLuxstamp;
  Int_t inFileNum;
  Int_t inClose2End;
  Int_t inExcluded;

  Float_t inGoodArea;
  Float_t inBadArea;

  int inTimeBin;
  int inZSlice;

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
    s1Tree->SetBranchAddress("area", &inS1Raw);
    s1Tree->SetBranchAddress("partner", &inPartners);
    s1Tree->SetBranchAddress("prompt_fraction_tlx", &inPromptFraction);
    //s1Tree->SetBranchAddress("prompt_fraction", &inPromptFraction);
    s1Tree->SetBranchAddress("max_peak_area", &inMaxPeakArea);
    s1Tree->SetBranchAddress("aft_t0_samples", &inAftT01);
    s1Tree->SetBranchAddress("aft_t05_samples", &inAftT05);
    s1Tree->SetBranchAddress("aft_t25_samples", &inAftT25);
    s1Tree->SetBranchAddress("aft_t1_samples", &inAftT50);
    s1Tree->SetBranchAddress("aft_t75_samples", &inAftT75);
    s1Tree->SetBranchAddress("aft_t95_samples", &inAftT95);
    s1Tree->SetBranchAddress("aft_t2_samples", &inAftT99);
    s1Tree->SetBranchAddress("aft_tlx_samples", &inAftTlx);
    s1Tree->SetBranchAddress("aft_trx_samples", &inAftTrx);
    //    s1Tree->SetBranchAddress("hft_t1_samples", &inHftTmax);

    s2Tree->SetBranchAddress("area_z_cor", &inS2);
    s2Tree->SetBranchAddress("area", &inS2Raw);
    s2Tree->SetBranchAddress("r_raw", &inR);

    // set branch if claudio's new position exists, otherwise use old
    if(s2Tree->GetListOfBranches()->FindObject("x_raw_new")){
    s2Tree->SetBranchAddress("x_raw_new", &inX);
    } else{
      cout << "x_raw_new doesn't exist, falling back to x_raw" << endl;
      s2Tree->SetBranchAddress("x_raw", &inX);
    }
    if(s2Tree->GetListOfBranches()->FindObject("y_raw_new")){
    s2Tree->SetBranchAddress("y_raw_new", &inY);
    } else{
      cout << "y_raw_new doesn't exist, falling back to y_raw" << endl;
      s2Tree->SetBranchAddress("y_raw", &inY);
    }
    s2Tree->SetBranchAddress("drift", &inDrift);
    if(s2Tree->GetListOfBranches()->FindObject("z_jpm")){
      s2Tree->SetBranchAddress("z_jpm", &inZ);
    } else{
      cout
        << "z_jpm doesn't exist, going to have to calculate based on field"
        << endl;
      calcZFlag = true;
    }
    //s2Tree->SetBranchAddress("r_raw_wall", &inRWall);
    s2Tree->SetBranchAddress("gaus_fit_sigma", &inS2GausSigma);
    s2Tree->SetBranchAddress("aft_t1_samples", &inS2AftT1s);
    s2Tree->SetBranchAddress("aft_t0_samples", &inS2AftT0s);
    if(s2Tree->GetListOfBranches()->FindObject("chi_raw_new")){
      s2Tree->SetBranchAddress("chi2_raw_new", &inChi2);
    } else{
      cout << "chi2_raw_new doesn't exist, falling back to chi2_rec for now, though I think that's wrong..." << endl;
      s2Tree->SetBranchAddress("chi2_rec", &inChi2);
    }
    if(s2Tree->GetListOfBranches()->FindObject("e_field")){
      s2Tree->SetBranchAddress("e_field", &inEField);
    } else{
      cout << "no e_field, setting to run03 default of 18000" << endl;
      inEField = 18000;
    }

    eventTree->SetBranchAddress("luxstamp", &inLuxstamp);
    eventTree->SetBranchAddress("fileNum", &inFileNum);
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
    d.s1Raws.push_back(inS1Raw);
    d.partners.push_back(inPartners);
    d.promptFractions.push_back(inPromptFraction);
    d.maxPeakAreas.push_back(inMaxPeakArea);
    d.aft_t01s.push_back(inAftT01);
    d.aft_t05s.push_back(inAftT05);
    d.aft_t25s.push_back(inAftT25);
    d.aft_t50s.push_back(inAftT50);
    d.aft_t75s.push_back(inAftT75);
    d.aft_t95s.push_back(inAftT95);
    d.aft_t99s.push_back(inAftT99);
    d.aft_tlxs.push_back(inAftTlx);
    d.aft_trxs.push_back(inAftTrx);
    //    d.hft_tmaxs.push_back(inHftTmax);
    
    d.s2s.push_back(inS2);
    d.s2Raws.push_back(inS2Raw);
    d.rs.push_back(pow(pow(inX, 2) + pow(inY, 2) ,.5));
    d.phis.push_back(std::fmod(atan2(inY, inX) + 3.14159*2, 3.14159*2));
    d.drifts.push_back(inDrift);
    if(calcZFlag){
      inZ = 54. - inDrift * 0.154;
    }
    d.zs.push_back(inZ);
    //d.rWalls.push_back(inRWall);
    d.s2GausSigmas.push_back(inS2GausSigma);
    d.s2AftT1s.push_back(inS2AftT1s);
    d.s2AftT0s.push_back(inS2AftT0s);
    d.chi2s.push_back(inChi2);
    d.eFields.push_back(inEField);

    d.luxstamps.push_back(inLuxstamp);
    d.fileNums.push_back(inFileNum);
    d.close2Ends.push_back(inClose2End);
    d.excludeds.push_back(inExcluded);

    d.goodAreas.push_back(inGoodArea);
    d.badAreas.push_back(inBadArea);

    calculateBin(inLuxstamp, inDrift, inTimeBin, inZSlice);
    d.timeBins.push_back(inTimeBin);
    d.zSlices.push_back(inZSlice);
    
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
  Float_t inS1Raw;
  Int_t inPartners;
  Float_t inPromptFraction;
  Float_t inMaxPeakArea;
  Int_t inAftT01;
  Int_t inAftT05;
  Int_t inAftT25;
  Int_t inAftT50;
  Int_t inAftT75;
  Int_t inAftT95;
  Int_t inAftT99;
  Int_t inAftTlx;
  Int_t inAftTrx;
  //  Int_t inHftTmax;
  
  Float_t inS2;
  Float_t inS2Raw;
  Float_t inR;
  Float_t inPhi;
  Float_t inDrift;
  Float_t inZ;
  //Float_t inRWall;
  Float_t inS2GausSigma;
  Int_t inS2AftT1s;
  Int_t inS2AftT0s;
  Float_t inChi2;
  Float_t inEField = 180;

  ULong64_t inLuxstamp;
  Int_t inFileNum;
  Int_t inClose2End;
  Int_t inExcluded;

  Float_t inGoodArea;
  Float_t inBadArea;

  Int_t inTimeBin;
  Int_t inZSlice;
  
  // load the input files
  TFile* inTFile = new TFile(inFile.c_str());
  cout << "loading from file: " << inFile << endl;
  TTree* dataTree = (TTree*)inTFile->Get("cutData");
  const Int_t nEntries = (Int_t)dataTree->GetEntries();

  // set addresses
  dataTree->SetBranchAddress("s1", &inS1);
  dataTree->SetBranchAddress("s1Raw", &inS1Raw);
  dataTree->SetBranchAddress("partner", &inPartners);
  dataTree->SetBranchAddress("promptFraction", &inPromptFraction);
  dataTree->SetBranchAddress("maxPeakArea", &inMaxPeakArea);
  dataTree->SetBranchAddress("aftT01", &inAftT01);
  dataTree->SetBranchAddress("aftT05", &inAftT05);
  dataTree->SetBranchAddress("aftT25", &inAftT25);
  dataTree->SetBranchAddress("aftT50", &inAftT50);
  dataTree->SetBranchAddress("aftT75", &inAftT75);
  dataTree->SetBranchAddress("aftT95", &inAftT95);
  dataTree->SetBranchAddress("aftT99", &inAftT99);
  dataTree->SetBranchAddress("aftTlx", &inAftTlx);
  dataTree->SetBranchAddress("aftTrx", &inAftTrx);
  //  dataTree->SetBranchAddress("hftTmax", &inHftTmax);

  dataTree->SetBranchAddress("s2", &inS2);
  dataTree->SetBranchAddress("s2Raw", &inS2Raw);
  dataTree->SetBranchAddress("r", &inR);
  dataTree->SetBranchAddress("phi", &inPhi);
  dataTree->SetBranchAddress("drift", &inDrift);
  dataTree->SetBranchAddress("z", &inZ);
  //dataTree->SetBranchAddress("rWall", &inRWall);
  dataTree->SetBranchAddress("s2GausSigma", &inS2GausSigma);
  dataTree->SetBranchAddress("s2AftT1", &inS2AftT1s);
  dataTree->SetBranchAddress("s2AftT0", &inS2AftT0s);
  dataTree->SetBranchAddress("chi2", &inChi2);
  dataTree->SetBranchAddress("eField", &inEField);

  dataTree->SetBranchAddress("luxstamp", &inLuxstamp);
  dataTree->SetBranchAddress("fileNum", &inFileNum);
  dataTree->SetBranchAddress("close2End", &inClose2End);
  dataTree->SetBranchAddress("excluded", &inExcluded);

  dataTree->SetBranchAddress("goodArea", &inGoodArea);
  dataTree->SetBranchAddress("badArea", &inBadArea);

  dataTree->SetBranchAddress("timeBin", &inTimeBin);
  dataTree->SetBranchAddress("zSlice", &inZSlice);

  // store data in object's vectors
  for(Int_t aaEvent = 0; aaEvent < nEntries; aaEvent++){

    dataTree->GetEntry(aaEvent);

    d.s1s.push_back(inS1);
    d.s1Raws.push_back(inS1Raw);
    d.partners.push_back(inPartners);
    d.promptFractions.push_back(inPromptFraction);
    d.maxPeakAreas.push_back(inMaxPeakArea);
    d.aft_t01s.push_back(inAftT01);
    d.aft_t05s.push_back(inAftT05);
    d.aft_t25s.push_back(inAftT25);
    d.aft_t50s.push_back(inAftT50);
    d.aft_t75s.push_back(inAftT75);
    d.aft_t95s.push_back(inAftT95);
    d.aft_t99s.push_back(inAftT99);
    d.aft_tlxs.push_back(inAftTlx);
    d.aft_trxs.push_back(inAftTrx);
    //    d.hft_tmaxs.push_back(inHftTmax);
    
    d.s2s.push_back(inS2);
    d.s2Raws.push_back(inS2Raw);
    d.rs.push_back(inR);
    d.phis.push_back(inPhi);
    d.drifts.push_back(inDrift);
    d.zs.push_back(inZ);
    //    d.rWalls.push_back(inRWall);
    d.s2GausSigmas.push_back(inS2GausSigma);
    d.s2AftT1s.push_back(inS2AftT1s);
    d.s2AftT0s.push_back(inS2AftT0s);
    d.chi2s.push_back(inChi2);
    d.eFields.push_back(inEField);

    d.luxstamps.push_back(inLuxstamp);
    d.fileNums.push_back(inFileNum);
    d.close2Ends.push_back(inClose2End);
    d.excludeds.push_back(inExcluded);

    d.goodAreas.push_back(inGoodArea);
    d.badAreas.push_back(inBadArea);

    d.timeBins.push_back(inTimeBin);
    d.zSlices.push_back(inZSlice);
    
    d.cutMask.push_back(false);

    d.keepIndex.push_back(aaEvent);

    if(aaEvent % 1000000 == 0){cout << "loading event " << aaEvent << endl;}
  } // end event loop
  
  return d;
}

void calculateBin(ULong64_t LUXStamp, Float_t drift,
                  int& timeBin, int& zSlice){
  // time bin
  if(LUXStamp <= 9358507485294003){timeBin = 0;} // run03
  else if(LUXStamp < 11660490100773156){timeBin = -1;} // between 3 and 4
  else if(LUXStamp < 12637830127330394){timeBin = 1;} // tb1
  else if(LUXStamp < 13413042129941973){timeBin = 2;} // tb2
  else if(LUXStamp < 14996142211013142){timeBin = 3;} // tb3
  else if(LUXStamp <= 16835364707187219){timeBin = 4;} // tb4
  else{timeBin = 5;} //beyond run04

  // z-slice
  if(drift < 40){/*cout << "drift lower than 40" << endl;*/}
  else if(drift < 105){zSlice = 1;}
  else if(drift < 170){zSlice = 2;}
  else if(drift < 235){zSlice = 3;}
  else if(drift <= 300){zSlice = 4;}
  else{/*cout << "drift higher than 300" << endl;*/}
}


