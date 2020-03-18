#ifndef KREXCLUSION_H
#define KREXCLUSION_H

#include "Cut.h"
#include "../Settings.h"

#include <vector>

class KrExclusionCut : public Cut{
 public:

  std::vector<unsigned long long int> mInjections;
  std::vector<unsigned long long int> mEnds;
  std::vector<unsigned long long int> mNewEnds;
  float mHalflives = 1;
  unsigned long long int mHalflifeSamples = 6.588e11;
  
  KrExclusionCut();

  void performCut(dataContainer &d);

  void loadInjections(std::string filename);
  bool checkRange(unsigned long long int stamp);
  void setHalflives(float);
  double timeExcluded();
};


#endif
