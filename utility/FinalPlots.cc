#include "FinalPlots.h"
#include "PlotUtils.h"

#include "../Settings.h"

#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"

using std::string;
using std::to_string;
using std::map;
using std::cout;
using std::endl;
using std::vector;


void plotFinals(dataContainer &d, string plotPrefix, string bandsFile){

  gStyle->SetLineWidth(2);
  
  // create graphs
  TGraph* s1S2Graph = new TGraph(d.keepIndex.size());
  TGraph* s1LogS2Graph = new TGraph(d.keepIndex.size());
  TGraph* s1LogS2S1Graph = new TGraph(d.keepIndex.size());
  TGraph* spatialGraph = new TGraph(d.keepIndex.size());
  TGraph* spatialR2Graph = new TGraph(d.keepIndex.size());

  // set non-masked events
  int index;
  for(int aaEvent = 0; aaEvent < d.keepIndex.size(); aaEvent++){
    index = d.keepIndex[aaEvent];

    s1S2Graph->SetPoint(aaEvent, d.s1s[index], d.s2s[index]);
    s1LogS2Graph->SetPoint(aaEvent, d.s1s[index], log10(d.s2s[index]));
    s1LogS2S1Graph->SetPoint(aaEvent, d.s1s[index],
                             log10(d.s2s[index]/d.s1s[index]));
    spatialGraph->SetPoint(aaEvent, d.rs[index], d.drifts[index]);
    spatialR2Graph->SetPoint(aaEvent, pow(d.rs[index], 2), d.drifts[index]);
                           
  } // end event loop

  // get er and nr bands
  
  bandCollection theBands = loadBands(bandsFile);

  cout << theBands.NR[1][3].at(5).gausMean << endl;

  vector<calcBandBin> calcBandER;
  vector<calcBandBin> calcBandNR;
  createBands(calcBandER, calcBandNR, theBands);


  // make band graphs
  TGraph* s1S2ERMaxUpper = new TGraph(calcBandER.size());
  TGraph* s1S2ERAvgUpper = new TGraph(calcBandER.size());
  TGraph* s1S2ERMean = new TGraph(calcBandER.size());
  TGraph* s1S2ERAvgLower = new TGraph(calcBandER.size());
  TGraph* s1S2ERMinLower = new TGraph(calcBandER.size());

  for(int aaPt = 0; aaPt < calcBandER.size(); aaPt++){
    s1S2ERMaxUpper->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                             calcBandER[aaPt].maxUpper);
    s1S2ERAvgUpper->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                             calcBandER[aaPt].meanUpper);
    s1S2ERMean->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                         calcBandER[aaPt].meanMean);
    s1S2ERAvgLower->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                             calcBandER[aaPt].meanLower);
    s1S2ERMinLower->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                             calcBandER[aaPt].minLower);
  }
  
  TGraph* s1S2NRMaxUpper = new TGraph(calcBandNR.size());
  TGraph* s1S2NRAvgUpper = new TGraph(calcBandNR.size());
  TGraph* s1S2NRMean = new TGraph(calcBandNR.size());
  TGraph* s1S2NRAvgLower = new TGraph(calcBandNR.size());
  TGraph* s1S2NRMinLower = new TGraph(calcBandNR.size());

  
  for(int aaPt = 0; aaPt < calcBandNR.size(); aaPt++){
    s1S2NRMaxUpper->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                             calcBandNR[aaPt].maxUpper);
    s1S2NRAvgUpper->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                             calcBandNR[aaPt].meanUpper);
    s1S2NRMean->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                         calcBandNR[aaPt].meanMean);
    s1S2NRAvgLower->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                             calcBandNR[aaPt].meanLower);
    s1S2NRMinLower->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                             calcBandNR[aaPt].minLower);
  }

  TGraph* s1LogS2ERMaxUpper = new TGraph(calcBandER.size());
  TGraph* s1LogS2ERAvgUpper = new TGraph(calcBandER.size());
  TGraph* s1LogS2ERMean = new TGraph(calcBandER.size());
  TGraph* s1LogS2ERAvgLower = new TGraph(calcBandER.size());
  TGraph* s1LogS2ERMinLower = new TGraph(calcBandER.size());

  for(int aaPt = 0; aaPt < calcBandER.size(); aaPt++){
    s1LogS2ERMaxUpper->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                                log10(calcBandER[aaPt].maxUpper));
    s1LogS2ERAvgUpper->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                                log10(calcBandER[aaPt].meanUpper));
    s1LogS2ERMean->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                            log10(calcBandER[aaPt].meanMean));
    s1LogS2ERAvgLower->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                                log10(calcBandER[aaPt].meanLower));
    s1LogS2ERMinLower->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                                log10(calcBandER[aaPt].minLower));
  }
  
  TGraph* s1LogS2NRMaxUpper = new TGraph(calcBandNR.size());
  TGraph* s1LogS2NRAvgUpper = new TGraph(calcBandNR.size());
  TGraph* s1LogS2NRMean = new TGraph(calcBandNR.size());
  TGraph* s1LogS2NRAvgLower = new TGraph(calcBandNR.size());
  TGraph* s1LogS2NRMinLower = new TGraph(calcBandNR.size());

  
  for(int aaPt = 0; aaPt < calcBandNR.size(); aaPt++){
    s1LogS2NRMaxUpper->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                                log10(calcBandNR[aaPt].maxUpper));
    s1LogS2NRAvgUpper->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                                log10(calcBandNR[aaPt].meanUpper));
    s1LogS2NRMean->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                            log10(calcBandNR[aaPt].meanMean));
    s1LogS2NRAvgLower->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                                log10(calcBandNR[aaPt].meanLower));
    s1LogS2NRMinLower->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                                log10(calcBandNR[aaPt].minLower));
  }

    TGraph* s1LogS2S1ERMaxUpper = new TGraph(calcBandER.size());
  TGraph* s1LogS2S1ERAvgUpper = new TGraph(calcBandER.size());
  TGraph* s1LogS2S1ERMean = new TGraph(calcBandER.size());
  TGraph* s1LogS2S1ERAvgLower = new TGraph(calcBandER.size());
  TGraph* s1LogS2S1ERMinLower = new TGraph(calcBandER.size());

  for(int aaPt = 0; aaPt < calcBandER.size(); aaPt++){
    s1LogS2S1ERMaxUpper->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                                log10(calcBandER[aaPt].maxUpper/calcBandER[aaPt].binCenter));
    s1LogS2S1ERAvgUpper->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                                log10(calcBandER[aaPt].meanUpper/calcBandER[aaPt].binCenter));
    s1LogS2S1ERMean->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                            log10(calcBandER[aaPt].meanMean/calcBandER[aaPt].binCenter));
    s1LogS2S1ERAvgLower->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                                log10(calcBandER[aaPt].meanLower/calcBandER[aaPt].binCenter));
    s1LogS2S1ERMinLower->SetPoint(aaPt, calcBandER[aaPt].binCenter,
                                log10(calcBandER[aaPt].minLower/calcBandER[aaPt].binCenter));
  }
  
  TGraph* s1LogS2S1NRMaxUpper = new TGraph(calcBandNR.size());
  TGraph* s1LogS2S1NRAvgUpper = new TGraph(calcBandNR.size());
  TGraph* s1LogS2S1NRMean = new TGraph(calcBandNR.size());
  TGraph* s1LogS2S1NRAvgLower = new TGraph(calcBandNR.size());
  TGraph* s1LogS2S1NRMinLower = new TGraph(calcBandNR.size());

  
  for(int aaPt = 0; aaPt < calcBandNR.size(); aaPt++){
    s1LogS2S1NRMaxUpper->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                                log10(calcBandNR[aaPt].maxUpper/calcBandER[aaPt].binCenter));
    s1LogS2S1NRAvgUpper->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                                log10(calcBandNR[aaPt].meanUpper/calcBandER[aaPt].binCenter));
    s1LogS2S1NRMean->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                            log10(calcBandNR[aaPt].meanMean/calcBandER[aaPt].binCenter));
    s1LogS2S1NRAvgLower->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                                log10(calcBandNR[aaPt].meanLower/calcBandER[aaPt].binCenter));
    s1LogS2S1NRMinLower->SetPoint(aaPt, calcBandNR[aaPt].binCenter,
                                log10(calcBandNR[aaPt].minLower/calcBandER[aaPt].binCenter));
  }

  // plot!
  TCanvas* s1S2Canvas = new TCanvas("s1S2", "s1S2", 1600, 1000);
  setGraphStyle(s1S2Graph, 0);
  s1S2Graph->Draw("ap");
  setGraphStyle(s1S2ERMaxUpper, 12);
  s1S2ERMaxUpper->Draw("same");
  setGraphStyle(s1S2ERAvgUpper, 11);
  s1S2ERAvgUpper->Draw("same");
  setGraphStyle(s1S2ERMean, 10);
  s1S2ERMean->Draw("same");
  setGraphStyle(s1S2ERAvgLower, 11);
  s1S2ERAvgLower->Draw("same");
  setGraphStyle(s1S2ERMinLower, 12);
  s1S2ERMinLower->Draw("same");

  setGraphStyle(s1S2NRMaxUpper, 22);
  s1S2NRMaxUpper->Draw("same");
  setGraphStyle(s1S2NRAvgUpper, 21);
  s1S2NRAvgUpper->Draw("same");
  setGraphStyle(s1S2NRMean, 20);
  s1S2NRMean->Draw("same");
  setGraphStyle(s1S2NRAvgLower, 21);
  s1S2NRAvgLower->Draw("same");
  setGraphStyle(s1S2NRMinLower, 22);
  s1S2NRMinLower->Draw("same");
  
  s1S2Canvas->SaveAs(("plots/" + plotPrefix + "_S1S2.png").c_str());


  TCanvas* s1LogS2Canvas = new TCanvas("s1LogS2", "s1LogS2", 1600, 1000);
    setGraphStyle(s1LogS2Graph, 0);
  s1LogS2Graph->Draw("ap");
  setGraphStyle(s1LogS2ERMaxUpper, 12);
  s1LogS2ERMaxUpper->Draw("same");
  setGraphStyle(s1LogS2ERAvgUpper, 11);
  s1LogS2ERAvgUpper->Draw("same");
  setGraphStyle(s1LogS2ERMean, 10);
  s1LogS2ERMean->Draw("same");
  setGraphStyle(s1LogS2ERAvgLower, 11);
  s1LogS2ERAvgLower->Draw("same");
  setGraphStyle(s1LogS2ERMinLower, 12);
  s1LogS2ERMinLower->Draw("same");

  setGraphStyle(s1LogS2NRMaxUpper, 22);
  s1LogS2NRMaxUpper->Draw("same");
  setGraphStyle(s1LogS2NRAvgUpper, 21);
  s1LogS2NRAvgUpper->Draw("same");
  setGraphStyle(s1LogS2NRMean, 20);
  s1LogS2NRMean->Draw("same");
  setGraphStyle(s1LogS2NRAvgLower, 21);
  s1LogS2NRAvgLower->Draw("same");
  setGraphStyle(s1LogS2NRMinLower, 22);
  s1LogS2NRMinLower->Draw("same");
  
  s1LogS2Canvas->SaveAs(("plots/" + plotPrefix + "_S1LogS2.png").c_str());


   TCanvas* s1LogS2S1Canvas = new TCanvas("s1LogS2S1", "s1LogS2S1", 1600, 1000);
    setGraphStyle(s1LogS2S1Graph, 0);
  s1LogS2S1Graph->Draw("ap");
  setGraphStyle(s1LogS2S1ERMaxUpper, 12);
  s1LogS2S1ERMaxUpper->Draw("same");
  setGraphStyle(s1LogS2S1ERAvgUpper, 11);
  s1LogS2S1ERAvgUpper->Draw("same");
  setGraphStyle(s1LogS2S1ERMean, 10);
  s1LogS2S1ERMean->Draw("same");
  setGraphStyle(s1LogS2S1ERAvgLower, 11);
  s1LogS2S1ERAvgLower->Draw("same");
  setGraphStyle(s1LogS2S1ERMinLower, 12);
  s1LogS2S1ERMinLower->Draw("same");

  setGraphStyle(s1LogS2S1NRMaxUpper, 22);
  s1LogS2S1NRMaxUpper->Draw("same");
  setGraphStyle(s1LogS2S1NRAvgUpper, 21);
  s1LogS2S1NRAvgUpper->Draw("same");
  setGraphStyle(s1LogS2S1NRMean, 20);
  s1LogS2S1NRMean->Draw("same");
  setGraphStyle(s1LogS2S1NRAvgLower, 21);
  s1LogS2S1NRAvgLower->Draw("same");
  setGraphStyle(s1LogS2S1NRMinLower, 22);
  s1LogS2S1NRMinLower->Draw("same");
  
  s1LogS2S1Canvas->SaveAs(("plots/" + plotPrefix + "_S1LogS2S1.png").c_str());
  

  TCanvas* spatialCanvas = new TCanvas("spatial", "spatial", 1000, 1000);
  setGraphStyle(spatialGraph, 0);
  spatialGraph->Draw("ap");
  spatialCanvas->SaveAs(("plots/" + plotPrefix + "_spatial.png").c_str());

  TCanvas* spatialR2Canvas = new TCanvas("spatialR2", "spatialR2", 1000, 1000);
  setGraphStyle(spatialR2Graph, 0);
  spatialR2Graph->Draw("ap");
  spatialR2Canvas->SaveAs(("plots/" + plotPrefix + "_spatialR2.png").c_str());

  
}




map<int, bandBin> loadBand(bool nr, int timeBin, int zSlice, string filepath){

  string typeLabel;
  string timeBinLabel = "tb" + to_string(timeBin);
  string driftBinLabel = "db" + to_string(zSlice);
  if(nr){typeLabel = "NRband";}
  else{typeLabel = "ERband";}

  map<int, bandBin> theBand;
  
  std::ifstream bandFile(filepath +
                         timeBinLabel + "_" + driftBinLabel + "_" + 
                         typeLabel + ".dat");
  bandBin tmpBin;
  string line;
  std::getline(bandFile, line); // throw out the label line
  while(std::getline(bandFile, line)){
    std::istringstream iss(line);
    iss >> tmpBin.binCenter >> tmpBin.binActual
        >> tmpBin.gausMean >> tmpBin.meanError
        >> tmpBin.gausSigma >> tmpBin.sigError
        >> tmpBin.chi2;
    theBand[(int)tmpBin.binCenter] = tmpBin;
  }

  return theBand;
}


bandCollection loadBands(string filepath){

  bandCollection theBands;
  
  vector< map<int, bandBin> > thisTimeBinNR;
  vector< map<int, bandBin> > thisTimeBinER;

  for(int aaTimeBin = 0; aaTimeBin < 4; aaTimeBin++){
    thisTimeBinNR.clear();
    thisTimeBinER.clear();
    for(int bbDriftBin = 0; bbDriftBin < 4; bbDriftBin++){
      thisTimeBinER.emplace_back(loadBand(false, aaTimeBin + 1,
                                          bbDriftBin + 1, filepath));
      thisTimeBinNR.emplace_back(loadBand(true, aaTimeBin + 1,
                                          bbDriftBin + 1, filepath));    
    } // end drift bin loop
    theBands.ER.push_back(thisTimeBinER);
    theBands.NR.push_back(thisTimeBinNR);    
  } // end time bin loop

  return theBands;
}


void createBands(vector<calcBandBin>& calcBandER,
                 vector<calcBandBin>& calcBandNR,
                 bandCollection& theBands){

  calcBandBin thisBinER;
  calcBandBin thisBinNR;
  for(int aaBin = 0; aaBin < 400; aaBin++){

    int numNR = 0;
    int numER = 0;
    float meanTotalER = 0;
    float meanTotalNR = 0;
    float lowerTotalER = 0;
    float lowerTotalNR = 0;
    float upperTotalER = 0;
    float upperTotalNR = 0;
    float currentMaxER = 0;
    float currentMaxNR = 0;
    float currentMinER = 1e10; // higher than any concievable value
    float currentMinNR = 1e10;
    
    for(int bbTimeBin = 0; bbTimeBin < 4; bbTimeBin++){
      for(int ccZSlice = 0; ccZSlice < 4; ccZSlice++){

        auto erIt = theBands.ER[bbTimeBin][ccZSlice].find(aaBin);
        if(erIt != theBands.ER[bbTimeBin][ccZSlice].end()){
          numER++;
          bandBin thisBin = erIt->second;
          meanTotalER += thisBin.gausMean;
          lowerTotalER += (thisBin.gausMean - thisBin.gausSigma);
          upperTotalER += (thisBin.gausMean + thisBin.gausSigma);
          if(currentMaxER < thisBin.gausMean + thisBin.gausSigma){
            currentMaxER = thisBin.gausMean + thisBin.gausSigma;
          }
          if(currentMinER > thisBin.gausMean - thisBin.gausSigma){
            currentMinER = thisBin.gausMean - thisBin.gausSigma;
          }
        } // add to ER calc if it exists
        auto nrIt = theBands.NR[bbTimeBin][ccZSlice].find(aaBin);
        if(nrIt != theBands.NR[bbTimeBin][ccZSlice].end()){
          numNR++;
          bandBin thisBin = nrIt->second;
          meanTotalNR += thisBin.gausMean;
          lowerTotalNR += (thisBin.gausMean - thisBin.gausSigma);
          upperTotalNR += (thisBin.gausMean + thisBin.gausSigma);
          if(currentMaxNR < thisBin.gausMean + thisBin.gausSigma){
            currentMaxNR = thisBin.gausMean + thisBin.gausSigma;
          }
          if(currentMinNR > thisBin.gausMean - thisBin.gausSigma){
            currentMinNR = thisBin.gausMean - thisBin.gausSigma;
          }
        } // add to NR calc if it exists  

      } // end zSlice (drift bin) loop
    } // end time bin loop

    if(numER){
      thisBinER.binCenter = (float)aaBin;
      thisBinER.meanMean = extractS2(meanTotalER/(float)numER, aaBin);
      thisBinER.meanLower = extractS2(lowerTotalER/(float)numER, aaBin);
      thisBinER.meanUpper = extractS2(upperTotalER/(float)numER, aaBin);
      thisBinER.minLower = extractS2(currentMinER, aaBin);
      thisBinER.maxUpper = extractS2(currentMaxER, aaBin);
      
      calcBandER.push_back(thisBinER);
    }
    if(numNR){
      thisBinNR.binCenter = (float)aaBin;
      thisBinNR.meanMean = extractS2(meanTotalNR/(float)numNR, aaBin);
      thisBinNR.meanLower = extractS2(lowerTotalNR/(float)numNR, aaBin);
      thisBinNR.meanUpper = extractS2(upperTotalNR/(float)numNR, aaBin);
      thisBinNR.minLower = extractS2(currentMinNR, aaBin);
      thisBinNR.maxUpper = extractS2(currentMaxNR, aaBin);
      
      calcBandNR.push_back(thisBinNR);
    }
    
  } // end looping through band bins

} // end createBands


float extractS2(float logS2S1, float s1){
  return pow(10, logS2S1) * s1;
}


void setGraphStyle(TGraph* theGraph, int style){

  // set line size to 2 by default
  theGraph->SetLineWidth(3);
  
  if(style == 0){
    // simple scatter plot style
    theGraph->SetMarkerStyle(8);
    theGraph->SetMarkerSize(.25);
    theGraph->SetMarkerColor(1);
  } else if(style == 10){
    // ER mean
    theGraph->SetLineColor(2);
  } else if(style == 11){
    // ER 1 sigma
        theGraph->SetLineColorAlpha(46, .7);
  }else if (style == 12){
    // ER 2 sigma
      theGraph->SetLineColorAlpha(45, .5);
  } else if(style == 20){
    // NR mean
    theGraph->SetLineColor(4);
  } else if(style == 21){
    // NR 1 sigma
    theGraph->SetLineColorAlpha(38, .7);
  } else if(style == 22){
    // NR 2 sigma
    theGraph->SetLineColorAlpha(39, .5);
  }
}
