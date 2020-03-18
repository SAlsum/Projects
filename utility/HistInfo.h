#ifndef HISTINFO_H
#define HISTINFO_H

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <unordered_map>



class HistInfo{
 private:
  double mMin;
  double mStepSize;
  int mNBins;
  bool mSorted;
  //int mQuantMax;
  std::vector< std::vector< double > > mBins;
  //Quantiles mQuantiles;
  std::unordered_map< double, std::vector<double> > mQuantiles;
  std::vector<double> mBinCenters;
  
 public:
  HistInfo();
  HistInfo(double min, double max, int nBins);
  void initialize();

  void addPoint(double x, double y);
  void sort();
  //void calculateQuantiles();
  void calculateQuantile(double quantile);
  void calculateQuantileSigma(double sigma);
  void calculateBinCenters();
  std::vector<double> getQuantile(double quantile);
  std::vector<double> getQuantileSigma(double sigma);
  std::vector<double> getBinCenters();
  std::vector<int> getCounts();

  
};




#endif
