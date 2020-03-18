#include <string>
#include <vector>
#include <iostream>

#include "Settings.h"
#include "utility/LoadData.h"
#include "utility/HistInfo.h"
#include "cuts/Cut.h"
#include "cuts/MaxPeakArea.h"

#include "TStopwatch.h"

#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TColor.h"
#include "TLegend.h"

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
  if(argc < 1){cout << "need a file to plot" << endl; return 1;}
  for(int aaArg = 1; aaArg < argc; aaArg++){
    tmpFilename = argv[aaArg];
    filenames.push_back(tmpFilename);
  }

    // load in some data to be cut plotted
  dataContainer d = loadData(filenames);

  
  TH2F* posHist = new TH2F("posHist", "posHist", 200, -26, 26, 200, -26, 26);
  int index;
  cout << d.keepIndex.size() << endl;
  cout << d.rs.size() << endl;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];
    float x = d.rs[aaEvent] * cos(d.phis[aaEvent]);
    float y = d.rs[aaEvent] * sin(d.phis[aaEvent]);
    posHist->Fill(x, y);
  }
  TCanvas* posCanvas = new TCanvas();
  posHist->Draw("colz");
  posCanvas->SaveAs("plots/spatial_topDown.png");
  

  cout << "Time to run: " << timer.RealTime() << " seconds" << endl;
  
  return 0;
}
