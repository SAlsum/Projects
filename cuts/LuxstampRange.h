#ifndef LUXSTAMPRANGE_H
#define LUXSTAMPRANGE_H

#include "Cut.h"
#include "../Settings.h"

class LuxstampRangeCut : public Cut{
 public:
  LuxstampRangeCut();

  void performCut(dataContainer &d);
  void plot(dataContainer &d, std::string plotPrefix);

};


#endif
