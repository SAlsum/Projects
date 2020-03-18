#ifndef S2WIDTH_H
#define S2WIDTH_H

#include "Cut.h"
#include "../Settings.h"

class S2WidthCut : public Cut{
 public:
  S2WidthCut();

  void performCut(dataContainer &d);
  void plot(dataContainer &d, std::string plotPrefix);

  void plotUniques(dataContainer &d, std::string plotPrefix);
};


#endif
