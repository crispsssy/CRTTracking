#ifndef _CALIBINFO_HXX_
#define _CALIBINFO_HXX_

#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TVector3.h>
#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>
#include "RuntimeParameter.hxx"
#include "CDCGeom.hxx"

class CalibInfo{
public:
	static CalibInfo& Get();
    double const GetDriftTime(TVector3 const& trkPos, TVector3 const& trkDir, int const channel);
    double const GetDriftTime(TVector3 const& trkPos, TVector3 const& trkDir, int const channel, TVector2& posCell, double& shift);
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
    void SetupMinimizer();
    double CalculateDriftTime(double const* pars); //only for minimization usage

    TGraph* fSimpleXTGraph;
    TGraph* fSimpleResoGraph;
    TF1* fSimpleXTFunc;
    TF1* fSimpleResoFunc;
    std::map<int, std::shared_ptr<TGraph2D>> fGraphs_x2t_mean;
    std::map<int, std::shared_ptr<TGraph2D>> fGraphs_x2t_std;
    ROOT::Math::Minimizer* fFit = nullptr;
    double fMinimizationThreshold = 6.; //do minimization for drift time if exceed this threshold
    TVector3 fCurrentTrkPos;
    TVector3 fCurrentTrkDir;
    int fCurrentChannel;

    //for simple XT
    int const fMaxDocaIndexXT = 94;
    int const fMaxDocaIndexReso = 74;
    double const fdDoca = 0.1;
    //for percise XT
    double const fMaxR = 20.;
    double const nShift = 90;
    double const dShift = 0.1;
    double const shiftOffset = -4.5;
    double const maxShift = shiftOffset + nShift * dShift;
};

#endif
