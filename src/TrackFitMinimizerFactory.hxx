#ifndef _TRACKFITMINIMIZERFACTORY_HXX_
#define _TRACKFITMINIMIZERFACTORY_HXX_

#include <iostream>
#include <unordered_map>
#include <functional>
#include "TrackFitMinimizerBase.hxx"

class TrackFitMinimizerFactory{

using CreateFunc = std::function<std::shared_ptr<TrackFitMinimizerBase>(std::shared_ptr<CDCLineCandidate>, std::string const&)>;

public:
    static TrackFitMinimizerFactory& Get();
    void RegisterTrackFitMinimizer(std::string const& minimizerName, CreateFunc func){ fCreators[minimizerName] = func; }
    std::shared_ptr<TrackFitMinimizerBase> CreateTrackFitMinimizer(std::shared_ptr<CDCLineCandidate> track, std::string const& minimierName = "TrackFitMinimizerDefault", std::string const& minimizerType = "Minuit2");

private:
    TrackFitMinimizerFactory(){}
    TrackFitMinimizerFactory(TrackFitMinimizerFactory const& src);
    TrackFitMinimizerFactory& operator=(TrackFitMinimizerFactory const& rhs) = delete;

    static TrackFitMinimizerFactory* fTrackFitMinimizerFactory;

    std::unordered_map<std::string, CreateFunc> fCreators;
    std::unordered_map<std::string, std::shared_ptr<TrackFitMinimizerBase>> fMinimizers;
};

#endif
