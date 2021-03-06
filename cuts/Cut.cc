#include "Cut.h"

#include "../Settings.h"
#include "../utility/StandardPlots.h"
#include "../utility/SaveData.h"

#include <string>
#include <iostream>


using std::string;
using std::cout;
using std::endl;


// the base class cut from which the actual cuts will be derived
// usually simply overriding the virtual "performCut" function, sometimes
// the plot function as well
Cut::Cut(){}

Cut::Cut(string cutName)
  :thisCutName(cutName)
{}

void Cut::plot(dataContainer &d, string plotPrefix){
  plotStandards(d, plotPrefix);
}

void Cut::save(dataContainer &d, string cutName){
  saveData(d, cutName);
}

void Cut::execute(dataContainer&d, bool cutOnly /*=false*/,
                  bool prePlots /*=false*/){
  if(prePlots){
    cout << "plotting data prior to " << thisCutName << endl;
    plot(d, "pre_" + thisCutName);
  }
  cout << "performing " << thisCutName << " cut" << endl;
  d.recentlyCutIndex.clear();
  performCut(d);
  if(!cutOnly){
    cout << "plotting results of " << thisCutName << endl;
    plot(d, "post_" + thisCutName);
    cout << "saving events passing " << thisCutName << " to file" << endl;
    save(d, thisCutName);
  }
}
