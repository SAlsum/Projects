#ifndef FIDUCIAL_H
#define FIDUCIAL_H

#define MY_PI 3.14159

#include "Cut.h"
#include "../Settings.h"

#include <vector>
#include <string>

class FiducialCut : public Cut{
 public:

  float mDriftMin;
  float mDriftStepSize;
  float mPhiMin;
  float mPhiStepSize;
  
  FiducialCut();
  FiducialCut(std::string wallDirectory);

  void performCut(dataContainer &d);

  // load wall position information
  std::vector< std::vector< std::vector <float> > > mWallInfo;
  void loadWallInfo(std::string wallDirectory);

  // get wall position for individual event
  float getRWall(int timeBin, float drift, float phi);


  
};


#endif
