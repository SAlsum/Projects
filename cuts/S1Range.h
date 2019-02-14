#ifndef S1RANGE_H
#define S1RANGE_H

#include "Cut.h"
#include "../Settings.h"

class S1RangeCut : public Cut{
 public:
  S1RangeCut();

  void performCut(dataContainer &d);
};


#endif
