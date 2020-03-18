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

  // print luxstamp, aft_t1_samples, and luxstamp + hft_t1_samples
  std::ofstream outfile;
  outfile.open("interestingEvents/JingkeEvents.txt");
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    int index = d.keepIndex[aaEvent];
    outfile << d.luxstamps[index] << " " << d.aft_t50s[index] << " "
            << (d.luxstamps[index] + (ULong64_t)(d.aft_t50s[index])) << " "
            << d.s1Raws[index] << "\n";
  }
  outfile.close();


}
