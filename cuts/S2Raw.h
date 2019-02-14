#ifndef S2RAW_H
#define S2RAW_H

#include "Cut.h"
#include "../Settings.h"

class S2RawCut : public Cut{
 public:
  S2RawCut();

  void performCut(dataContainer &d);
};


#endif
