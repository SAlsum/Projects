#ifndef S2SHAPE_H
#define S2SHAPE_H

#include "Cut.h"
#include "../Settings.h"

class S2ShapeCut : public Cut{
 public:
  S2ShapeCut();

  void performCut(dataContainer &d);
  void plot(dataContainer &d, std::string plotPrefix);

  void plotUniques(dataContainer &d, std::string plotPrefix);
};


#endif
