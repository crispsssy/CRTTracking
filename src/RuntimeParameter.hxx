#ifndef _RUNTIMEPARAMETER_HXX_
#define _RUNTIMEPARAMETER_HXX_

#include <string>
#include <TMath.h>

namespace RuntimePar
{
	//Basic information
	extern std::string f_in_path;
	extern int runNum;
    extern bool const runMode;
	extern int startEvent;
	extern int const nCh;
	extern std::string XTMode;

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

    //for CalibInfo
    extern double const XT_reso_x;
    extern double const XT_reso_y;
    extern int const XT_nshift;
}

#endif
