#include "TrackFitMinimizerFactory.hxx"

TrackFitMinimizerFactory* TrackFitMinimizerFactory::fTrackFitMinimizerFactory = nullptr;

TrackFitMinimizerFactory& TrackFitMinimizerFactory::Get()
{
    if(!fTrackFitMinimizerFactory){
        fTrackFitMinimizerFactory = new TrackFitMinimizerFactory();
    }
    return *fTrackFitMinimizerFactory;
}

std::shared_ptr<TrackFitMinimizerBase> TrackFitMinimizerFactory::CreateTrackFitMinimizer(std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerName, std::string const& minimizerType)
{
    if(fCreators.count(minimizerName)){
        return fCreators[minimizerName](track, minimizerType);
    }
    return nullptr;
}
