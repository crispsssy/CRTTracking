#ifndef _TRACKFITMINIMIZERDEFAULT_HXX_
#define _TRACKFITMINIMIZERDEFAULT_HXX_

#include <iostream>
#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>
#include "CDCLineCandidate.hxx"
#include "CDCHit.hxx"
#include "CDCGeom.hxx"
#include "CalibInfo.hxx"
#include "RuntimeParameter.hxx"
#include "TrackFitMinimizerBase.hxx"
#include "TrackFitMinimizerFactory.hxx"

class TrackFitMinimizerDefault : public TrackFitMinimizerBase{
public:
    TrackFitMinimizerDefault(std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType);
    void TrackFitting(std::string const& XTMode);
    void TrackFittingRTT0();
    double GetChi2();

private:
    void SetupParameters(std::string const& XTMode);
    double FittingFunctionRT(double const* pars);
    double FittingFunctionXYZT(double const* pars);
    double FittingFunctionRTT0(double const* pars);
    void UpdateTrack(double const* pars, double const* errors);
    void Optimize();

    static bool registered;
};

#endif
