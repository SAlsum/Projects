#include "HistInfo2d.h"

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


HistInfo2d::HistInfo2d()
: mXMin(0), mXStepSize(1), mXBins(10),
  mYMin(0), mYStepSize(1), mYBins(10)
{
  initialize();
}


HistInfo2d::HistInfo2d(float xMin, float xMax, int xBins,
                       float yMin, float yMax, int yBins)
  : mXMin(xMin), mXStepSize((xMax-xMin)/xBins), mXBins(xBins),
    mYMin(yMin), mYStepSize((yMax-yMin)/yBins), mYBins(yBins)
{
  initialize();
}


void HistInfo2d::initialize(){
  vector< vector<float> > tmpVec;
  vector<float> tmpVec2;
  // empty vector with enough bins in y direction
  for(int aaBin = 0; aaBin < mYBins; aaBin++){
    tmpVec.push_back(tmpVec2);
  }
  // enough rows in x of these vectors
  for(int aaBin = 0; aaBin < mXBins; aaBin++){
    mBins.push_back(tmpVec);
  }
}


void HistInfo2d::addPoint(float x, float y, float z){
  if(x < mXMin or x >= mXMin + mXStepSize * mXBins or
     y < mYMin or y >= mYMin + mYStepSize * mYBins){return;}
  mBins[floor((x - mXMin)/mXStepSize)][floor((y - mYMin)/mYStepSize)]
    .push_back(z);
  mSorted = false;
}


void HistInfo2d::sort(){
  for(int xxBin = 0; xxBin < mXBins; xxBin++){
    for(int yyBin = 0; yyBin < mYBins; yyBin++){
      std::sort(mBins[xxBin][yyBin].begin(), mBins[xxBin][yyBin].end());
    }
  }
  mSorted = true;
}


void HistInfo2d::calculateQuantile(float fraction){
  cout << "calculating quantile " << fraction << endl;
  
  vector< vector<float> > quantMatrix;
  for(int xxBin = 0; xxBin < mXBins; xxBin++){
    vector<float> thisXQuantVals;
    for(int yyBin = 0; yyBin < mYBins; yyBin++){
      int nEvts = mBins[xxBin][yyBin].size();
      if(nEvts > 0){ //doesn't make sense to find a quantile if no evts in bin
        float quantVal = mBins[xxBin][yyBin][floor(fraction * nEvts)];
        thisXQuantVals.push_back(quantVal);
      } else {
        if(thisXQuantVals.size() > 0){// just return the last one calculated 
        thisXQuantVals.push_back(thisXQuantVals.back());
        } else { // if first one, just push 0
          thisXQuantVals.push_back(0);
        }
      }
    } // end y bin loop
    quantMatrix.push_back(thisXQuantVals);
  } // end bin loops

  //record this
  mQuantiles[fraction] = quantMatrix;
}


vector< vector<float> > HistInfo2d::getQuantile(float quantile){
  // check to see if any new events have been added to see if we have to
  // recalcualte the quantile regardless of whether it exists
  if(! mSorted){
    sort();
    calculateQuantile(quantile);
  }

  // check to see if quantile has already been calculated, if not, do.
  if(mQuantiles.count(quantile) == 0){
    calculateQuantile(quantile);
  }

  // retrieve the desired quantile
  return mQuantiles.at(quantile);
}


vector< vector<float> > HistInfo2d::getQuantileSigma(float sigma){
  float fraction = .5*(1 + erf(sigma/sqrt(2.)));
  return getQuantile(fraction);
}


void HistInfo2d::calculateBinCenters(){
  for(int xxBin = 0; xxBin < mXBins; xxBin++){
    mXBinCenters.push_back(mXMin + (xxBin + 0.5) * mXStepSize);
  }
  for(int yyBin = 0; yyBin < mYBins; yyBin++){
    mYBinCenters.push_back(mYMin + (yyBin + 0.5) * mYStepSize);
  }
}


vector<float> HistInfo2d::getXBinCenters(){
  // calculate the bin centers if it hasn't been done
  if(mXBinCenters.begin() == mXBinCenters.end()){
    calculateBinCenters();
  }
  return mXBinCenters;
}


vector<float> HistInfo2d::getYBinCenters(){
  // calculate the bin centers if it hasn't been done
  if(mYBinCenters.begin() == mYBinCenters.end()){
    calculateBinCenters();
  }
  return mYBinCenters;
}


vector< vector<int> > HistInfo2d::getCounts(){
  vector< vector<int> > counts;
  for(int xxBin = 0; xxBin < mXBins; xxBin++){
    vector<int> thisXCounts;
    for(int yyBin = 0; yyBin < mYBins; yyBin++){
      thisXCounts.push_back(mBins[xxBin][yyBin].size());
    }
    counts.push_back(thisXCounts);
  }
  return counts;
}
