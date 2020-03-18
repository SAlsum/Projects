#ifndef FIDUCIALADJACENT_H
#define FIDUCIALADJACENT_H

#ifndef MY_PI
#define MY_PI 3.14159
#endif

#include "Cut.h"
#include "../Settings.h"

#include <vector>
#include <string>

class FiducialAdjacentCut : public Cut{
 public:

  float mDriftMin;
  float mDriftStepSize;
  float mPhiMin;
  float mPhiStepSize;
  
  FiducialAdjacentCut();
  FiducialAdjacentCut(std::string wallDirectory);

  void performCut(dataContainer &d);

  // load wall position information
  std::vector< std::vector< std::vector <float> > > mWallInfo;
  void loadWallInfo(std::string wallDirectory);

  // get wall position for individual event
  float getRWall(int timeBin, float drift, float phi);


  
};


#endif
