#ifndef LOADDATA_H
#define LOADDATA_H

#include "../Settings.h"

#include <string>
#include <vector>

// wrapper around loading either a number of processed files, or one
// custom cut file
dataContainer loadData(std::vector<std::string>);

// load a number of processed files
dataContainer loadRQData(std::vector<std::string>);

// load a cut file
dataContainer loadCutData(std::string);



#endif
