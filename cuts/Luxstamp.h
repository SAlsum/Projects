#ifndef LUXSTAMP_H
#define LUXSTAMP_H

#include "Cut.h"
#include "../Settings.h"
#include <unordered_set>
#include <string>

class LuxstampCut : public Cut{
 private:
  
  std::unordered_set<unsigned long long int> mStamps; 
  
 public:

  LuxstampCut();

  void performCut(dataContainer &d);
  void plot(dataContainer &d, std::string plotPrefix);

  void loadStamps(std::string filename);
  
};


#endif
