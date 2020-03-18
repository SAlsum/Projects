#include "PlotUtils.h"

#include "TGraph.h"


void stylizeCutGraph(TGraph* theGraph){
  theGraph->SetMarkerStyle(5);
  theGraph->SetMarkerColor(2);
}
