#ifndef _CALIBINFO_HXX_
#define _CALIBINFO_HXX_

#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TF1.h>

class CalibInfo{
public:
	static CalibInfo& Get();
	double const GetRAtT(double t);
	double const GetSpatialResolution(double r);

private:
	CalibInfo(){}
	CalibInfo(CalibInfo const& src);
	CalibInfo& operator=(CalibInfo const& rhs);

	static CalibInfo* fCalibInfo;

	TF1* fTimeToDistance;
};

#endif
