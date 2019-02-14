#ifndef FIDUCIAL_H
#define FIDUCIAL_H

#include "Cut.h"
#include "../Settings.h"

class FiducialCut : public Cut{
 public:
  FiducialCut();

  void performCut(dataContainer &d);
};


#endif
