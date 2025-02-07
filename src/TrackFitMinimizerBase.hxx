#ifndef _TRACKFITMINIMIZERBASE_HXX_
#define _TRACKFITMINIMIZERBASE_HXX_

#include <iostream>
#include <memory>
#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>
#include "RuntimeParameter.hxx"
#include "CDCLineCandidate.hxx"

class TrackFitMinimizerBase{
public:
    TrackFitMinimizerBase(std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType);
    ~TrackFitMinimizerBase(){}
    virtual void TrackFitting(std::string const& XTMode) = 0;
    void Clear();
    void SetTrack(std::shared_ptr<CDCLineCandidate> track);

private:
    virtual void SetupParameters(std::string const& XTMode) = 0;
    virtual void UpdateTrack(double const* pars, double const* errors) = 0;

protected:
    std::shared_ptr<CDCLineCandidate> fTrack;
    std::unique_ptr<ROOT::Math::Minimizer> fFit;
};

#endif
