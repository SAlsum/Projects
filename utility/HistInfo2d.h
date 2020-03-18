#ifndef HISTINFO2D_H
#define HISTINFO2D_H

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <unordered_map>


class HistInfo2d{
private:
  float mXMin;
  float mXStepSize;
  float mXBins;
  float mYMin;
  float mYStepSize;
  float mYBins;
  bool mSorted;

  std::vector< std::vector< std::vector<float> > > mBins;
  std::unordered_map<float, std::vector< std::vector<float> > > mQuantiles;
  std::vector<float> mXBinCenters;
  std::vector<float> mYBinCenters;

public:
  HistInfo2d();
  HistInfo2d(float xMin, float xMax, int xBins,
           float yMin, float yMax, int yBins);
  void initialize();

  void addPoint(float x, float y, float z);
  void sort();

  void calculateQuantile(float quantile);
  void calculateBinCenters();

  std::vector< std::vector<float> > getQuantile(float quantile);
  std::vector< std::vector<float> > getQuantileSigma(float sigma);
  std::vector<float> getXBinCenters();
  std::vector<float> getYBinCenters();
  std::vector< std::vector<int> > getCounts();
};







#endif
