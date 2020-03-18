#include "Fiducial.h"
#include "../Settings.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>


using std::vector;
using std::string;
using std::to_string;
using std::cout;
using std::endl;
using std::min;


FiducialCut::FiducialCut()
  : Cut("FiducialCut"), mDriftMin(0), mPhiMin(-MY_PI)
{}

FiducialCut::FiducialCut(string wallDir)
  : Cut("FiducialCut"), mDriftMin(0), mPhiMin(-MY_PI)
{
  loadWallInfo(wallDir);
}

void FiducialCut::performCut(dataContainer &d){
  vector<int> newKeepIndex;
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    if(d.drifts[index] < 40 or d.drifts[index] > 300
       or d.rs[index] > getRWall(std::max(-1, std::min(4, d.timeBins[index])),
                                 d.drifts[index],
                                 d.phis[index]) - 3){
      d.cutMask[index] = true;
      d.recentlyCutIndex.push_back(index);
    } else {newKeepIndex.push_back(index);}
  }
  d.keepIndex = newKeepIndex;
}


void FiducialCut::loadWallInfo(string wallDir){
  vector< vector<float> > thisTimeBinInfo;
  vector<float> thisPhiInfo;
  
  // 4 time bins, labeled 1-4
  for(int aaTimeBin = 1; aaTimeBin <= 4; aaTimeBin++){
    string thisFilename = wallDir + "/WallPositions_MedianTB" +
      to_string(aaTimeBin) + ".txt";
    std::ifstream wallFile(thisFilename);
    string line;

    thisTimeBinInfo.clear();
    while(std::getline(wallFile, line)){
      thisPhiInfo.clear();
      std::istringstream iss(line);
      string item;
      float entry;
      while(iss >> item){
        if(item == "nan"){entry = 0;} // not sure what to do with nans atm
        else{entry = std::stof(item);}
        thisPhiInfo.push_back(entry);
      } // end drift time loop
      thisTimeBinInfo.push_back(thisPhiInfo);
     
    } // end phi loop

    mWallInfo.push_back(thisTimeBinInfo);
  } // end time bin loop

  // set some other variables now that we can
  mDriftStepSize = (380 - mDriftMin)/(float)mWallInfo[0][0].size();
  mPhiStepSize = MY_PI*2/(float)mWallInfo[0].size();
}


float FiducialCut::getRWall(int timeBin, float drift, float phi){
  if(timeBin == 0){return 20;} //no walls for r3 yet
  int driftBin = floor((drift - mDriftMin)/mDriftStepSize);
  float phiAdjust = phi > MY_PI ? phi - 2 * MY_PI : phi;
  int phiBin = min((int)floor((phiAdjust - mPhiMin)/mPhiStepSize), 359); // 359 phi bins. ugly..
   // cout << "timeBin: " << timeBin << endl;
   // cout << "drift: " << drift << endl;
   // cout << "phi: " << phi << endl;
   // cout << "driftBin: " << driftBin << endl;
   // cout << "phiBin: " << phiBin << endl;
  // if occurs between r3 and r4, use tb1 walls
  if(timeBin == -1){return mWallInfo[0][phiBin][driftBin];}
  return mWallInfo[timeBin - 1][phiBin][driftBin]; // currently only 4 tb
}
