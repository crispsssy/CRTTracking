#ifndef _TRACKFITMINIMIZER_HXX_
#define _TRACKFITMINIMIZER_HXX_

#include <iostream>
#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>
#include "CDCLineCandidate.hxx"
#include "CDCHit.hxx"
#include "CDCGeom.hxx"
#include "CalibInfo.hxx"
#include "RuntimeParameter.hxx"

class TrackFitMinimizer{
public:
    TrackFitMinimizer(CDCLineCandidate* track);
    ~TrackFitMinimizer();
    void TrackFitting(std::string XTMode);
    void TrackFittingRTT0();
    double GetChi2();

private:
    void SetupParameters(std::string XTMode);
    double FittingFunctionRT(double const* pars);
    double FittingFunctionXYZT(double const* pars);
    double FittingFunctionRTT0(double const* pars);
    void UpdateTrack(double const* pars, double const* errors);
    void Optimize();

    CDCLineCandidate* fTrack = nullptr;
    ROOT::Math::Minimizer* fFit = nullptr;
};

#endif
