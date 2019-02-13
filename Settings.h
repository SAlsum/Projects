#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

#include "Rtypes.h"


// data structure containing all needed rqs
  struct dataContainer{
    std::vector<Float_t> s1s;
    std::vector<Int_t> partners;
    std::vector<Float_t> promptFractions;
    std::vector<Float_t> maxPeakAreas;
  
    std::vector<Float_t> s2s;
    std::vector<Float_t> s2Raws;
    std::vector<Float_t> rs;
    //std::vector<Float_t> phis; don't actually need phi for cuts
    std::vector<Float_t> drifts;
    std::vector<Float_t> rWalls;
    std::vector<Float_t> s2GausSigmas;
    std::vector<Int_t> s2AftT1s;
    std::vector<Int_t> s2AftT0s;

    std::vector<ULong64_t> luxstamps;
    std::vector<Int_t> close2Ends;
    std::vector<Int_t> excludeds;

    std::vector<Float_t> goodAreas;
    std::vector<Float_t> badAreas;

    std::vector<bool> cutMask;
    
    std::vector<int> keepIndex; // which events haven't been cut
  };
// in my opinion it would be slightly more elegant to have a struct that
// records a single data point with a single value for each of the above
// and a second that contains a vector of those points and a record of which
// are cut, but I can't figure that it would be any faster and this is
// fine for now...




#endif
