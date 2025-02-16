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
    if(fMinimizers.count(minimizerName)){
        auto minimizer = fMinimizers[minimizerName];
        minimizer->Clear();
        minimizer->SetTrack(track);
        return minimizer;
    }
    else if(fCreators.count(minimizerName)){
        fMinimizers[minimizerName] = fCreators[minimizerName](track, minimizerType);
        return fMinimizers[minimizerName];
    }
    else return nullptr;
}
