#ifndef DDSELECTOR_H
#define DDSELECTOR_H

#include "Cut.h"
#include "../Settings.h"

class DDSelectorCut : public Cut{
 public:
  DDSelectorCut();

  void performCut(dataContainer &d);
};


#endif
