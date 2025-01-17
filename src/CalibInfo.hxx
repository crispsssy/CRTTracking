#ifndef _CALIBINFO_HXX_
#define _CALIBINFO_HXX_

#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TF1.h>
#include <TGraph2D.h>
#include "RuntimeParameter.hxx"

class CalibInfo{
public:
	static CalibInfo& Get();
	double const GetTAtR(double r);
    double const GetTAtXYShift(double x, double y, double shift);
	double const GetTimeResolution(double t);

private:
	CalibInfo();
	CalibInfo(CalibInfo const& src);
	CalibInfo& operator=(CalibInfo const& rhs);

	static CalibInfo* fCalibInfo;

    void ReadXTTable();

    std::map<int, std::shared_ptr<TGraph2D>> graphs_x2t;
};

#endif
