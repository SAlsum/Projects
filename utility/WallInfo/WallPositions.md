Overview

The positions of the walls were obtained using the run IV background data with new reconstructed positions.
The new positions differ from the original positions by excluding the PMT #26 from the reconstruction.
Nonetheless, the original position reconstruction code was not modified.
The new positions of the wall were obtained for each time bin. The binning is, as before, performed in both the drift and the azimuth. 
We calculated two different wall positions which differ on the binning.

The old binning used in the run IV original analysis is saved in a .mat file called WallPositions_OldBinning.mat. The variable is called avgRa.
It has the dimensions TBxDTxPhi --> 4x29x28
The first dimension is the time bins using our run IV time bin definitions.
The second dimension is drift. Drift bins are from 20 to 310 in steps of 10.
The third dimension is phi. Phi bins are from -pi to pi in steps of pi/14. 
Each element of avgRad contains the average radius of the wall in that time, drift, phi bin. 
For example avgRad[0,0,0] contains the avgerage radius of the wall in TB1, Drift bin 10-20 and phi bin -pi to -13pi/14. 

The new binning is saved in a matlab file called WallPositions.mat. This file has a structure called wallres. It has several members but what matters is the radial median (wallres.median).
All elements have dimensions TBxPhixDT ---> 4x360x380 and you just need to round the drift time and the azimuth to get the correct dimension.
There are also ASCII files with the variable wallres.median for each time bins.
