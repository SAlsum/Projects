#include "utility/HistInfo.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <random>


using std::cout;
using std::endl;
using std::vector;


template <typename T>
void printContents(T vec){
  for(auto element : vec){
    cout << element << endl;
  }
}


int main(){

  vector<double> randomXs;
  vector<double> randomYs;

  int nPts = 100000;

      std::uniform_real_distribution<double> unif(0, 10);
      std::default_random_engine re;

  for(int aa = 0; aa < nPts; aa++){
    double rx = unif(re);
    double ry = unif(re);
    randomXs.push_back(rx);
    randomYs.push_back(ry);
  }

  /*
  for(int aa = 0; aa < randomXs.size(); aa++){
    cout << "x: " << randomXs[aa] << "   y: " << randomYs[aa] << randomYs[aa]
         << endl;
  }
  */

  HistInfo myHistInfo(0, 2, 5);
  cout << myHistInfo.getBinCenters().size() << endl;
  cout << "bin centers" << endl;
  printContents(myHistInfo.getBinCenters());
  for(int aaPt = 0; aaPt < nPts; aaPt++){
    myHistInfo.addPoint(randomXs[aaPt], randomYs[aaPt]);
  }
  cout << myHistInfo.getQuantile(2).size() << endl;
  cout << "2 sigma quantile" << endl;
  printContents(myHistInfo.getQuantile(2));
  cout << "means" << endl;
  printContents(myHistInfo.getQuantile(0));
  
}
