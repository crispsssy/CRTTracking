#include "TrackFitMinimizerBase.hxx"

using RuntimePar::runMode;

TrackFitMinimizerBase::TrackFitMinimizerBase(std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType)
: fTrack(track), 
  fFit(ROOT::Math::Factory::CreateMinimizer(minimizerType))
{
    if(!fFit){
        std::cerr<<"TrackFitMinimizerBase: Can not create minimizer"<<std::endl;
        exit(1);
    }
    fFit->SetPrintLevel(runMode);
}

void TrackFitMinimizerBase::Clear()
{
    fFit->Clear();
    fTrack.reset();
}

void TrackFitMinimizerBase::SetTrack(std::shared_ptr<CDCLineCandidate> track)
{
    fTrack = track;
}
