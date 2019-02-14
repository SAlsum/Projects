#ifndef CLOSE2END_H
#define CLOSE2END_H

#include "Cut.h"
#include "../Settings.h"

class Close2EndCut : public Cut{
 public:
  Close2EndCut();

  void performCut(dataContainer &d);
};


#endif
