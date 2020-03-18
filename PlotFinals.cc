#include <string>
#include <vector>
#include <iostream>

#include "Settings.h"
#include "utility/LoadData.h"
#include "utility/FinalPlots.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;


int main(int argc, char* argv[]){
  
 // get the list of files to process from passed in args
  string tmpFilename;
  vector<string> filenames;
  if(argc < 1){cout << "need a file to plot" << endl; return 1;}
  for(int aaArg = 1; aaArg < argc - 1; aaArg++){
    tmpFilename = argv[aaArg];
    filenames.push_back(tmpFilename);
  }

  // load in some data to be cut plotted
  dataContainer d = loadData(filenames);

  // make plots
  plotFinals(d, argv[argc-1], "bands/bandFiles/");
  
}
