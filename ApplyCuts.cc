#include <string>
#include <vector>
#include <iostream>

#include "Settings.h"
#include "utility/LoadData.h"
#include "cuts/Cut.h"
#include "cuts/ExclusionWKr.h"


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



  return 0;
}
