#ifndef S1RANGEEXTENDED_H
#define S1RANGEEXTENDED_H

#include "Cut.h"
#include "../Settings.h"

class S1RangeExtendedCut : public Cut{
 public:
  S1RangeExtendedCut();

  void performCut(dataContainer &d);
};


#endif
