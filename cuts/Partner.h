#ifndef PARTNER_H
#define PARTNER_H

#include "Cut.h"
#include "../Settings.h"

class PartnerCut : public Cut{
 public:
  PartnerCut();

  void performCut(dataContainer &d);
};


#endif
