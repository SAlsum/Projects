#ifndef BADAREA_H
#define BADAREA_H

#include "Cut.h"
#include "../Settings.h"

class BadAreaCut : public Cut{
 public:
  BadAreaCut();

  void performCut(dataContainer &d);
  void plot(dataContainer &d, std::string plotPrefix);

  void plotUniques(dataContainer &d, std::string plotPrefix);
};


#endif
