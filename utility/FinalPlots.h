#ifndef FINALPLOTS_H
#define FINALPLOTS_H

#include "../Settings.h"

#include "TGraph.h"

#include <string>
#include <map>
#include <vector>

struct bandBin{
  float binCenter;
  float binActual;
  float gausMean;
  float meanError;
  float gausSigma;
  float sigError;
  float chi2;
};

struct calcBandBin{
  float binCenter;
  float meanMean;
  float meanLower;
  float meanUpper;
  float minLower;
  float maxUpper;
};

struct bandCollection{
  std::vector< std::vector< std::map<int, bandBin> > > ER;
  std::vector< std::vector< std::map<int, bandBin> > > NR;
};


void plotFinals(dataContainer &d, std::string plotLabel,
                std::string bandsFile);

std::map<int, bandBin> loadBand(bool nr, int timeBin, int zSlice,
                                std::string filepath);
bandCollection loadBands(std::string filepath);
void createBands(std::vector<calcBandBin>& ERBand,
                 std::vector<calcBandBin>& NRBand,
                 bandCollection& theBands);
float extractS2(float logS2S1, float s1);
void setGraphStyle(TGraph* theGraph, int style);

#endif
