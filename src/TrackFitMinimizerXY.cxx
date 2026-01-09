#include "TrackFitMinimizerXY.hxx"

using RuntimePar::runMode;

TrackFitMinimizerXY::TrackFitMinimizerXY(std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType)
: TrackFitMinimizerBase(track, minimizerType)
{

}

void TrackFitMinimizerXY::TrackFitting(std::string const& XTMode){
    if(!fTrack) return;

    std::function<double(double const*)> func = [this, XTMode](double const* pars){
        if(XTMode == "RT"){
            return this->FittingFunctionRT(pars);
        }
        else if(XTMode == "XYZT"){
            return this->FittingFunctionXYZT(pars);
        }
        else return 0.;
    };
    ROOT::Math::Functor functionRT(func, 4);
    fFit->SetFunction(functionRT);
    SetupParameters(XTMode);

    //Start Minimization
    fFit->Minimize();
    fTrack->SetChi2( fFit->MinValue() );
    fTrack->SetNdf( fTrack->GetHits()->size() - 4 );
    double const* pars = fFit->X();
    double const* errors = fFit->Errors();
//    std::cout<<"Minimum chi2 is "<<fFit->MinValue()<<std::endl;
    UpdateTrack(pars, errors);

}

void TrackFitMinimizerXY::SetupParameters(std::string const& XTMode){
    TVector3 pocaT;
    TVector3 pocaW;
    CDCGeom::Get().GetPOCA(fTrack->GetPos(), fTrack->GetDir(), TVector3(0., 0., 0.), TVector3(0., 0., 1.), pocaT, pocaW);
    TVector2 pocaTXY(pocaT.X(), pocaT.Y());
    double x = pocaTXY.X();
    double y = pocaTXY.Y();
    double z = pocaT.Z();
    double theta = fTrack->GetDir().Theta();
    double xStep = 10.;
    double yStep = 10.;
    double zStep = 11.;
    double thetaStep = 1e-4;
    double xMin = -850;
    double xMax = 850.;
    double yMin = -TMath::Pi()*2;
    double yMax = TMath::Pi()*2;
    double zMin = -1500;
    double zMax = 1500;
    double thetaMin = -TMath::Pi()*2;
    double thetaMax = TMath::Pi()*2;
    fFit->SetTolerance(0.1);
    if(XTMode == "XYZT"){
        fFit->SetTolerance(0.001);
        fFit->SetStrategy(2);
        fFit->SetMaxFunctionCalls(1e4);
        xStep = 1.;
        yStep = 1.;
        zStep = 1.;
        thetaStep = 1e-5;
        double dX = 5;
        double dY = 5;
        double dZ = 5;
        double dTheta = 1e-2;
        xMin = x - dX;
        xMax = x + dX;
        yMin = y - dY;
        yMax = y + dY;
        zMin = z - dZ;
        zMax = z + dZ;
        thetaMin = theta - dTheta;
        thetaMax = theta + dTheta;
    }
    if(runMode){
        std::cout<<"x : xMin : xMax "<<x<<":"<<xMin<<":"<<xMax<<std::endl;
        std::cout<<"y : yMin : yMax "<<y<<":"<<yMin<<":"<<yMax<<std::endl;
        std::cout<<"z : zMin : zMax "<<z<<":"<<zMin<<":"<<zMax<<std::endl;
        std::cout<<"theta : thetaMin : thetaMax "<<theta<<":"<<thetaMin<<":"<<thetaMax<<std::endl;
    }
    fFit->SetVariable(0,     "x",            x,                 xStep);
    fFit->SetVariable(1,     "y",            y,                 yStep);
    fFit->SetVariable(2,     "z",            z,                 zStep);
    fFit->SetVariable(3,     "theta",        theta,             thetaStep);
    fFit->SetVariableLimits(0,       xMin,                    xMax);
    fFit->SetVariableLimits(1,       yMin,                    yMax);
    fFit->SetVariableLimits(2,       zMin,                      zMax);
    fFit->SetVariableLimits(3,       thetaMin,                  thetaMax);
}

double TrackFitMinimizerXY::FittingFunctionRT(double const* pars){
    //Abort hits that drift time larger than 400 ns since simple XT doesn't fit corner hits.
    double x          = pars[0];
    double y          = pars[1];
    double z          = pars[2];
    double theta      = pars[3];
    ROOT::Math::KahanSum<double> chi2{0.0};
    TVector3 trkPos(x, y, z);
    TVector3 trkDir(sin(theta) * cos(y), sin(theta) * sin(y), cos(theta));

    for(auto hit = fTrack->GetHits()->begin(); hit != fTrack->GetHits()->end(); ++hit){
        int channel = (*hit)->GetChannelID();
        double driftTime = (*hit)->GetDriftTime(0);
        if(driftTime > fRTMaxDriftTime) continue;
        double DOCA = CDCGeom::Get().GetDOCA(trkPos, trkDir, channel);
        double t_expect = CalibInfo::Get().GetTAtR(DOCA);
        //      std::cout<<"DOCA:t_meas:t_expect "<<DOCA<<":"<<driftTime<<":"<<t_expect<<std::endl;
        double sigma = CalibInfo::Get().GetTimeResolution(DOCA);
        chi2.Add( (driftTime - t_expect) * (driftTime - t_expect) / (sigma * sigma) );
    }
    return chi2.Sum();
}

double TrackFitMinimizerXY::FittingFunctionXYZT(double const* pars)
{
    double x          = pars[0];
    double y          = pars[1];
    double z          = pars[2];
    double theta      = pars[3];
    ROOT::Math::KahanSum<double> chi2{0.0};
    TVector3 trkPos(x, y, z);
    TVector3 trkDir(sin(theta) * cos(y), sin(theta) * sin(y), cos(theta));
    TVector2 pos_cell;
    double shift;

    if(runMode){
        static int rr = 0;
        rr++;
        std::cout<<"++++++++++++++++++++++++++++++minimize round: "<<rr<<std::endl;
        std::cout<<std::fixed<<std::setprecision(6)<<"x : y : z : theta "<<x<<" : "<<y<<" : "<<z<<" : "<<theta<<std::endl;
    }
    for(auto hit = fTrack->GetHits()->begin(); hit != fTrack->GetHits()->end(); ++hit){
        int channel = (*hit)->GetChannelID();
        double driftTime = (*hit)->GetDriftTime(0);
        double t_expect = CalibInfo::Get().GetDriftTime(trkPos, trkDir, channel, pos_cell, shift);
        if(runMode) std::cout<<"x:y:shift "<<pos_cell.X()<<":"<<pos_cell.Y()<<":"<<shift<<" t_meas:t_expect:residual "<<driftTime<<":"<<t_expect<<":"<<driftTime - t_expect<<std::endl;
        double sigma = CalibInfo::Get().GetTimeResolution(pos_cell.X(), pos_cell.Y(), shift);
        chi2.Add( (driftTime - t_expect) * (driftTime - t_expect) / (sigma * sigma) );
    }
    if(runMode){
        std::cout<<"chi2 = "<<chi2.Sum()<<std::endl;
    }
    return chi2.Sum();
}

void TrackFitMinimizerXY::UpdateTrack(double const* pars, double const* errors){
    double x          = pars[0];
    double y          = pars[1];
    double z          = pars[2];
    double theta      = pars[3];
    ROOT::Math::KahanSum<double> chi2{0.0};
    TVector3 trkPos(x, y, z);
    TVector3 trkDir(sin(theta) * cos(y), sin(theta) * sin(y), cos(theta));

    fTrack->SetPos(trkPos);
    fTrack->SetDir(trkDir);

    double err_x    = errors[0];
    double err_y    = errors[1];
    double err_z      = errors[2];
    double err_theta  = errors[3];

    double rho = sqrt(x*x + y*y);
    double err_rho = sqrt(x * x * err_x * err_x + y * y * err_y * err_y) / rho;
    double err_phi = sqrt(y * y * err_x * err_x + x * x * err_y * err_y) / (rho * rho);
    fTrack->SetRhoError(err_rho);
    fTrack->SetPhiError(err_phi);
    fTrack->SetAlphaError(err_z);
    fTrack->SetThetaError(err_theta);
    //  std::cout<<"updated track x:y:z:z:theta "<<x<<":"<<y<<":"<<x*sin(y_poca)/tan(z)<<":"<<z<<":"<<theta<<" "<<fTrack->GetDir().Theta()<<std::endl;
    //  std::cout<<"updated track x:y:z:theta "<<x<<":"<<y<<":"<<z<<":"<<theta<<std::endl;

    //update hit position at z
    for(auto hit = fTrack->GetHits()->begin(); hit!= fTrack->GetHits()->end(); ++hit){
        int channel = (*hit)->GetChannelID();
        TVector3 pocaT;
        TVector3 pocaW;
        CDCGeom::Get().GetPOCA(trkPos, trkDir, channel, pocaT, pocaW);
        double DOCA = (pocaT - pocaW).Mag();
        TVector2 posCell = CDCGeom::Get().LocalPositionToCellPositionXY(pocaT, channel);
        (*hit)->SetZ(pocaT.Z());
        (*hit)->SetDOCA(DOCA);
        (*hit)->SetPosCell(posCell);
    }
}

bool TrackFitMinimizerXY::registered = [](){
    TrackFitMinimizerFactory::Get().RegisterTrackFitMinimizer("TrackFitMinimizerXY",
        [](std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType){
            return std::make_shared<TrackFitMinimizerXY>(track, minimizerType);
        }
    );
    std::cout<<"TrackFitMinimizerXY registered to minimizer factory"<<std::endl;
    return true;
}();
