#ifndef _TRACKFITMINIMIZERNEGATIVELOGLIKELIHOOD_HXX_
#define _TRACKFITMINIMIZERNEGATIVELOGLIKELIHOOD_HXX_

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

class TrackFitMinimizerNegativeLogLikelihood : public TrackFitMinimizerBase{
public:
    TrackFitMinimizerNegativeLogLikelihood(std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType);
    void TrackFitting(std::string const& XTMode = "XYZT");
    
private:
    void SetupParameters(std::string const& XTMode);
    double FittingFunction(double const* pars);
    void UpdateTrack(double const* pars, double const* errors);

    static bool registered;
};

#endif
