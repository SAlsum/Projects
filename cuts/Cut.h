#ifndef CUT_H
#define CUT_H

#include "../Settings.h"

#include <string>

// define the abstract cut class
class Cut{
 public:
  std::string thisCutName;
  
  Cut();
  Cut(std::string cutName);

  virtual void performCut(dataContainer &d) = 0;
  void plot(dataContainer &d, std::string plotPrefix);
  void save(dataContainer &d, std::string cutName);
  void execute(dataContainer &d, bool cutOnly=false, bool prePlots=false);
};


//check again

#endif
