#ifndef S2RANGE_H
#define S2RANGE_H

#include "Cut.h"
#include "../Settings.h"

class S2RangeCut : public Cut{
 public:
  S2RangeCut();

  void performCut(dataContainer &d);
};


#endif
