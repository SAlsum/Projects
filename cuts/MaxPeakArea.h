#ifndef MAXPEAKAREA_H
#define MAXPEAKAREA_H

#include "Cut.h"
#include "../Settings.h"

#include <vector>
#include <string>

class MaxPeakAreaCut : public Cut{
 public:

  std::vector< std::vector<float> > mQuantDiffParams;
  std::vector< std::vector< std::vector<float> > > mMedianParams;
  
  MaxPeakAreaCut();
  MaxPeakAreaCut(std::string MPADir);

  void performCut(dataContainer &d);
  void plot(dataContainer &d, std::string plotPrefix);

  void plotUniques(dataContainer &d, std::string plotPrefix);

  //float driftAdjust(float drift, float mpaFraction);

  void loadQuantDiffParams(std::string MPADir);
  void loadMedianParams(std::string MPADir);
  int getZSlice(float z);
  float getDiffValue(float s1Raw);
  float getMedianValue(float s1Raw, float z);
};



#endif
