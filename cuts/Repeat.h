#ifndef REPEAT_H
#define REPEAT_H

#include "Cut.h"
#include "../Settings.h"
#include <unordered_set>
#include <string>

class RepeatCut : public Cut{
 private:
  
  std::unordered_set<unsigned long long int> mStamps; 
  
 public:

  RepeatCut();

  void performCut(dataContainer &d);
  void plot(dataContainer &d, std::string plotPrefix);

  void loadStamps(std::string filename);
  
};


#endif
