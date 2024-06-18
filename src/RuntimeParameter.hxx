#ifndef _RUNTIMEPARAMETER_HXX_
#define _RUNTIMEPARAMETER_HXX_

#include <TMath.h>

namespace RuntimePar
{
	//Basic information
	extern int runNum;
	extern int const nCh;

	//for Preprocess
	extern bool const adcSumCut;
	extern bool const tdcCut;
	extern int const adcSumThreshold;
	extern int const adcSumCutAtTDC;
	extern bool const numHitCut;
	extern int const minNumHits;
	extern int const maxNumHits;
	//for Hough transform
	extern int const maxItr;
	extern int const coefficient; //ratio between bins of hough and fill intervals
	extern int const nbinPhi;
	extern int const nFillPhi;
	extern double const minPhi;
	extern double const maxPhi;
	extern double const dPhi;
	extern double const dFillPhi;
	extern int const nbinRho;
	extern double const minRho;
	extern double const maxRho;
	extern double const dRho;
}

#endif
