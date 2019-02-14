#include <string>
#include <vector>
#include <iostream>

#include "Settings.h"
#include "utility/LoadData.h"
#include "cuts/Cut.h"
#include "cuts/ExclusionWKr.h"
#include "cuts/Close2End.h"
#include "cuts/Partner.h"
#include "cuts/S1Range.h"
#include "cuts/S2Range.h"
#include "cuts/S2Raw.h"
#include "cuts/Fiducial.h"
#include "cuts/BadArea.h"
#include "cuts/PromptFraction.h"
#include "cuts/S2Shape.h"


using std::vector;
using std::string;
using std::cout;
using std::endl;


int main(int argc, char* argv[]){
  // get the list of files to process from passed in args
  string tmpFilename;
  vector<string> filenames;
  if(argc < 1){cout << "need a file to cut" << endl; return 1;}
  for(int aaArg = 1; aaArg < argc; aaArg++){
    tmpFilename = argv[aaArg];
    filenames.push_back(tmpFilename);
  }

  // load in some data to be cut
  dataContainer d = loadData(filenames);

  // perform cuts
  ExclusionCut exCut;
  exCut.execute(d, true);
  Close2EndCut closeCut;
  closeCut.execute(d, true);
  PartnerCut parCut;
  parCut.execute(d);

  S1RangeCut s1RangeCut;
  s1RangeCut.execute(d);
  S2RangeCut s2RangeCut;
  s2RangeCut.execute(d);
  S2RawCut s2RawThresholdCut;
  s2RawThresholdCut.execute(d);
  

  FiducialCut fidCut;
  fidCut.execute(d);

  BadAreaCut baCut;
  baCut.execute(d);
  PromptFractionCut pfCut;
  pfCut.execute(d);
  S2ShapeCut s2ShapeCut;
  s2ShapeCut.execute(d);

  return 0;
}
