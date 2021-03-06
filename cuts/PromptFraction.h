#ifndef PROMPTFRACTION_H
#define PROMPTFRACTION_H

#include "Cut.h"
#include "../Settings.h"

class PromptFractionCut : public Cut{
 public:
  PromptFractionCut();

  void performCut(dataContainer &d);
  void plot(dataContainer &d, std::string plotPrefix);

  void plotUniques(dataContainer &d, std::string plotPrefix);
};


#endif
