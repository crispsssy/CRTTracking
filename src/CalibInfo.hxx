#ifndef _CALIBINFO_HXX_
#define _CALIBINFO_HXX_

#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TF1.h>
#include "RuntimeParameter.hxx"

class CalibInfo{
public:
	static CalibInfo& Get();
	double const GetTAtR(double r);
	double const GetTimeResolution(double t);

private:
	CalibInfo(){}
	CalibInfo(CalibInfo const& src);
	CalibInfo& operator=(CalibInfo const& rhs);

	static CalibInfo* fCalibInfo;

	TF1* fTimeToDistance;
};

#endif
