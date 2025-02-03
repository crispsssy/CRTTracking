#ifndef _CALIBINFO_HXX_
#define _CALIBINFO_HXX_

#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include "RuntimeParameter.hxx"

class CalibInfo{
public:
	static CalibInfo& Get();
	double const GetTAtR(double r);
    double const GetTAtXYShift(double x, double y, double shift);
	double const GetTimeResolution(double r) const;
    double const GetTimeResolution(double const x, double const y, double const shift) const;

private:
	CalibInfo();
	CalibInfo(CalibInfo const& src);
	CalibInfo& operator=(CalibInfo const& rhs);

	static CalibInfo* fCalibInfo;

    void ReadXTTable();
    void GenerateSimpleXT();

    TGraph* fSimpleXTGraph;
    TGraph* fSimpleResoGraph;
    TF1* fSimpleXTFunc;
    TF1* fSimpleResoFunc;
    std::map<int, std::shared_ptr<TGraph2D>> fGraphs_x2t_mean;
    std::map<int, std::shared_ptr<TGraph2D>> fGraphs_x2t_std;

    //for simple XT
    int const fMaxDocaIndexXT = 94;
    int const fMaxDocaIndexReso = 74;
    double const fdDoca = 0.1;
};

#endif
