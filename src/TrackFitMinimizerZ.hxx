#ifndef _TRACKFITMINIMIZERZ_HXX_
#define _TRACKFITMINIMIZERZ_HXX_

#include <iostream>
#include <iomanip>
#include <Math/Util.h>
#include "TrackFitMinimizerBase.hxx"
#include "TrackFitMinimizerFactory.hxx"
#include "CDCLineCandidate.hxx"
#include "CDCHit.hxx"
#include "CDCGeom.hxx"
#include "CalibInfo.hxx"
#include "RuntimeParameter.hxx"

class TrackFitMinimizerZ : public TrackFitMinimizerBase
{
public:
    TrackFitMinimizerZ(std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType);
    void TrackFitting(std::string const& XTMode);
    void TrackFittingRTT0();

private:
    void SetupParameters(std::string const& XTMode);
    double FittingFunctionRT(double const* pars);
    double FittingFunctionXYZT(double const* pars);
    void UpdateTrack(double const* pars, double const* errors);

    static bool registered;
};

#endif
