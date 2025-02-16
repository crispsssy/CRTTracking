#include "TrackFitMinimizerNegativeLogLikelihood.hxx"

using RuntimePar::runMode;

TrackFitMinimizerNegativeLogLikelihood::TrackFitMinimizerNegativeLogLikelihood(std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType)
: TrackFitMinimizerBase(track, minimizerType)
{

}

void TrackFitMinimizerNegativeLogLikelihood::TrackFitting(std::string const& XTMode)
{
    if(!fTrack) return;

    fFit->SetTolerance(0.001);
    fFit->SetPrintLevel(runMode);

    std::function<double(double const*)> func = [this, XTMode](double const* pars){
        return FittingFunction(pars);
    };
    ROOT::Math::Functor functionFit(func, 4);
    fFit->SetFunction(functionFit);
    SetupParameters(XTMode);

    fFit->Minimize();
    double const* pars = fFit->X();
    double const* errors = fFit->Errors();
    UpdateTrack(pars, errors);
}

void TrackFitMinimizerNegativeLogLikelihood::SetupParameters(std::string const& XTMode)
{

}

double TrackFitMinimizerNegativeLogLikelihood::FittingFunction(double const* pars)
{
    return 0.;
}

void TrackFitMinimizerNegativeLogLikelihood::UpdateTrack(double const* pars, double const* errors)
{

}

bool TrackFitMinimizerNegativeLogLikelihood::registered = [](){
    TrackFitMinimizerFactory::Get().RegisterTrackFitMinimizer("TrackFitMinimizerNegativeLogLikelihood",
        [](std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType) {
            return std::make_shared<TrackFitMinimizerNegativeLogLikelihood>(track, minimizerType);
        }
    );
    std::cout<<"TrackFitMinimizerNegativeLogLikelihood registered to minimizer factory"<<std::endl;
    return true;
}();
