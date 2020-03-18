#ifndef SELECTION_H
#define SELECTION_H

#include "Cut.h"
#include "../Settings.h"

class SelectionCut : public Cut{
 public:
  SelectionCut();

  void performCut(dataContainer &d);
};


#endif
