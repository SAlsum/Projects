#include "Settings.h"
#include "utility/LoadData.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>


using std::string;
using std::to_string;
using std::cout;
using std::endl;
using std::vector;


bool selector(dataContainer& d, int index){
  // edit this function to change event selection.  Return true for events to
  // select
  
  return //d.s1s[index] < 10 and d.s2s[index] > 5000;
    true;
}


int main(int argc, char* argv[]){

 // get the list of files to process from passed in args
  string tmpFilename;
  vector<string> filenames;
  if(argc < 1){cout << "need a file to cut" << endl; return 1;}
  for(int aaArg = 1; aaArg < argc; aaArg++){
    tmpFilename = argv[aaArg];
    filenames.push_back(tmpFilename);
  }

  // load in the data
  dataContainer d = loadData(filenames);

  // print luxstamp of events that pass the selection
  std::ofstream outfile;
  outfile.open("interestingEvents/selectedEvents.txt");
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    //index = d.keepIndex[aaEvent];
    if(selector(d, aaEvent)){
      outfile << to_string(d.luxstamps[aaEvent]) << "\n";
    }
  }
  outfile.close();
  
}
