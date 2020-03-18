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
#include "cuts/MaxPeakArea.h"
#include "cuts/PromptFraction.h"
#include "cuts/BadArea.h"
#include "cuts/S2Shape.h"
#include "cuts/S2Width.h"
#include "cuts/Chi2.h"


#include "TStopwatch.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;


int main(int argc, char* argv[]){
  TStopwatch timer;
  timer.Start();
  
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
  BadAreaCut baCut;
  baCut.execute(d, false, true);
  
  PromptFractionCut pfCut;
  pfCut.execute(d, false, true);
  MaxPeakAreaCut mpaCut;
  mpaCut.execute(d, false, true);
  
  S2ShapeCut s2ShapeCut;
  s2ShapeCut.execute(d, false, true);
  S2WidthCut s2WidthCut;
  s2WidthCut.execute(d, false, true);
  Chi2Cut c2Cut;
  c2Cut.execute(d, false, true);


  




  cout << "Time to run: " << timer.RealTime() << " seconds" << endl;
  
  return 0;
}
