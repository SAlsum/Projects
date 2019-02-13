#ifndef EXCLUSIONWKR_H
#define EXCLUSIONWKR_H

#include "Cut.h"
#include "../Settings.h"

class ExclusionCut : public Cut{
 public:
  ExclusionCut();

  void performCut(dataContainer &d);
};

#endif
