#ifndef S2RANGEEXTENDED_H
#define S2RANGEEXTENDED_H

#include "Cut.h"
#include "../Settings.h"

class S2RangeExtendedCut : public Cut{
 public:
  S2RangeExtendedCut();

  void performCut(dataContainer &d);
};


#endif
