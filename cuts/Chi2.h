#ifndef CHI2_H
#define CHI2_H

#include "Cut.h"
#include "../Settings.h"

class Chi2Cut : public Cut{
 public:
  Chi2Cut();

  void performCut(dataContainer &d);
  void plot(dataContainer &d, std::string plotPrefix);

  void plotUniques(dataContainer &d, std::string plotPrefix);
};


#endif
