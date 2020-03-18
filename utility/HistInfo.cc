#include "HistInfo.h"

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <unordered_map>
#include <cmath>


using std::vector;
using std::endl;
using std::cout;


HistInfo::HistInfo()
  : mMin(0), mStepSize(1), mNBins(10)
{
  initialize();
}


HistInfo::HistInfo(double min, double max, int nBins)
  :mMin(min), mStepSize((max-min)/nBins), mNBins(nBins)
{
  initialize();
}


void HistInfo::initialize(){
  vector<double> tmpVec;
  for(int aaBin = 0; aaBin < mNBins; aaBin++){
    mBins.push_back(tmpVec);
  }
}


void HistInfo::addPoint(double x, double y){
  if(x < mMin or x >= mMin + mStepSize * mNBins){return;}
  mBins[floor((x - mMin)/mStepSize)].push_back(y);
  mSorted = false;
}


void HistInfo::sort(){
  // cout << "Sorting... " << endl;
  for(int aaBin = 0; aaBin < mNBins; aaBin++){
    std::sort(mBins[aaBin].begin(), mBins[aaBin].end());
  }
  mSorted = true;
}


/*
void HistInfo::calculateQuantiles(){
  vector<double> emptyVec;
  for(int aaQuant = -mQuantMax; aaQuant <= mQuantMax; aaQuant++){
    mQuantiles[aaQuant] = emptyVec;    
    
    double fraction = .5*(1 + erf(aaQuant/sqrt(2.)));
    for(int bbBin = 0; bbBin < mNBins; bbBin++){
      int nEvts = mBins[bbBin].size();

      double quantVal = mBins[bbBin][floor(fraction * nEvts)];
      mQuantiles[aaQuant].push_back(quantVal);
    } // end bin loop
  } // end quant loop
}
*/


void HistInfo::calculateQuantile(double fraction){
  cout << "calculating quantile " << fraction << endl;
  vector<double> emptyVec;
  mQuantiles[fraction] = emptyVec;


    for(int aaBin = 0; aaBin < mNBins; aaBin++){
      int nEvts = mBins[aaBin].size();
      //cout << "nEvts: " << nEvts << endl;
      if(nEvts > 0){
        // just see what would happen if we tried to round up
        double quantVal;
        if(floor(fraction * nEvts)+1 <= mBins[aaBin].size()){
          quantVal = mBins[aaBin][floor(fraction * nEvts)+1];
        }else{
            quantVal = mBins[aaBin][floor(fraction * nEvts)];
          }
          mQuantiles[fraction].push_back(quantVal);
        } else {
          if(mQuantiles[fraction].size() > 0){
            mQuantiles[fraction].push_back(mQuantiles[fraction].back());
          } else {
            mQuantiles[fraction].push_back(0);
          }
        }
    } // end bin loop
}


void HistInfo::calculateQuantileSigma(double sigma){
  double fraction = .5*(1 + erf(sigma/sqrt(2.)));
  calculateQuantile(fraction);
}


void HistInfo::calculateBinCenters(){
  for(int aaBin = 0; aaBin < mNBins; aaBin++){
    mBinCenters.push_back(mMin + (aaBin + 0.5) * mStepSize);
  }
}


vector<double> HistInfo::getQuantile(double quantile){
  // calculate quantiles if it has not been done
  if(mQuantiles.count(quantile) == 0){
    if(! mSorted){sort();}
    calculateQuantile(quantile);
  }

  // retrieve desired quantile
  return mQuantiles.at(quantile);
}


vector<double> HistInfo::getQuantileSigma(double sigma){
  double fraction = .5*(1 + erf(sigma/sqrt(2.)));
  return getQuantile(fraction);
}



vector<double> HistInfo::getBinCenters(){
  // calculate bin centers if not done
  if(mBinCenters.begin() == mBinCenters.end()){
    calculateBinCenters();
  }

  return mBinCenters;
};


vector<int> HistInfo::getCounts(){
  // just say how many points are in each bin
  vector<int> counts;
  for(int aaBin = 0; aaBin < mBins.size(); aaBin++){
    counts.push_back(mBins[aaBin].size());
  }
  return counts;
}
