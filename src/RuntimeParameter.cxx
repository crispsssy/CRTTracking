#include "RuntimeParameter.hxx"

namespace RuntimePar
{
	//Basic information
	std::string f_in_path("");
	int runNum = 0;
	int startEvent = 0;
	int const nCh = 4992;
	std::string XTMode("RT");

	//for Preprocess
	bool const tdcCut = true;
	bool const adcSumCut = true;
	int const adcSumThreshold = 10;
	int const adcSumCutAtTDC = 400;
	bool const numHitCut = true;
	int const minNumHits = 12;
	int const maxNumHits = 640;
	//for Hough transform
	int const maxItr = 4;
	int const coefficient = 8; //ratio between bins of hough and fill intervals
	int const nbinPhi = 200;
	int const nFillPhi = nbinPhi * coefficient;
	double const minPhi = TMath::Pi() * 0;
	double const maxPhi = TMath::Pi() * 2;
	double const dPhi = (maxPhi - minPhi) / nbinPhi;
	double const dFillPhi = dPhi / coefficient;
	int const nbinRho = 50;
	double const minRho = -1;
	double const maxRho = 999;
	double const dRho = (maxRho - minRho) / nbinRho;
}
