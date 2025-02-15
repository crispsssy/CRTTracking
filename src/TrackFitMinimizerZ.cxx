#include "TrackFitMinimizerZ.hxx"

using RuntimePar::runMode;

TrackFitMinimizerZ::TrackFitMinimizerZ(std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType)
: TrackFitMinimizerBase(track, minimizerType)
{

}

void TrackFitMinimizerZ::TrackFitting(std::string const& XTMode){
    if(!fTrack) return;

    fFit->SetTolerance(0.001);
    fFit->SetPrintLevel(runMode);

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

void TrackFitMinimizerZ::SetupParameters(std::string const& XTMode){
    TVector3 pocaT;
    TVector3 pocaW;
    CDCGeom::Get().GetPOCA(fTrack->GetPos(), fTrack->GetDir(), TVector3(0., 0., 0.), TVector3(0., 0., 1.), pocaT, pocaW);
    TVector2 pocaTXY(pocaT.X(), pocaT.Y());
    double rho = pocaTXY.Mod();
    double phi = fTrack->GetDir().Phi();
    double z = pocaT.Z();
    double theta = fTrack->GetDir().Theta();
    double rhoStep = 0.1;
    double phiStep = 1e-4;
    double zStep = 1.;
    double thetaStep = 1e-4;
    double rhoMin = 0.;
    double rhoMax = 850.;
    double phiMin = -TMath::Pi()*2;
    double phiMax = TMath::Pi()*2;
    double zMin = -1500;
    double zMax = 1500;
    double thetaMin = -TMath::Pi()*2;
    double thetaMax = TMath::Pi()*2;
    if(XTMode == "XYZT"){
        fFit->SetStrategy(2);
        fFit->SetMaxFunctionCalls(1e4);
        rhoStep = 1e-5;
        phiStep = 1e-5;
        zStep = 1e-2;
        thetaStep = 1e-5;
        double dRho = 0.1;
        double dPhi = 1e-2;
        double dZ = 2;
        double dTheta = 1e-2;
        rhoMin = rho - dRho;
        rhoMax = rho + dRho;
        phiMin = phi - dPhi;
        phiMax = phi + dPhi;
        zMin = z - dZ;
        zMax = z + dZ;
        thetaMin = theta - dTheta;
        thetaMax = theta + dTheta;
    }
    if(runMode){
        std::cout<<"rho : rhoMin : rhoMax "<<rho<<":"<<rhoMin<<":"<<rhoMax<<std::endl;
        std::cout<<"phi : phiMin : phiMax "<<phi<<":"<<phiMin<<":"<<phiMax<<std::endl;
        std::cout<<"z : zMin : zMax "<<z<<":"<<zMin<<":"<<zMax<<std::endl;
        std::cout<<"theta : thetaMin : thetaMax "<<theta<<":"<<thetaMin<<":"<<thetaMax<<std::endl;
    }
    fFit->SetVariable(0,     "rho",          rho,               rhoStep);
    fFit->SetVariable(1,     "phi",          phi,               phiStep);
    fFit->SetVariable(2,     "z",            z,                 zStep);
    fFit->SetVariable(3,     "theta",        theta,             thetaStep);
    fFit->SetVariableLimits(0,       rhoMin,                    rhoMax);
    fFit->SetVariableLimits(1,       phiMin,                    phiMax);
    fFit->SetVariableLimits(2,       zMin,                      zMax);
    fFit->SetVariableLimits(3,       thetaMin,                  thetaMax);
}

double TrackFitMinimizerZ::FittingFunctionRT(double const* pars){
    //Abort hits that drift time larger than 400 ns since simple XT doesn't fit corner hits.
    double rho        = pars[0];
    double phi        = pars[1];
    double z          = pars[2];
    double theta      = pars[3];
    double chi2 = 0.;
    double phi_poca = phi - TMath::Pi() / 2;
    TVector3 trkPos(rho * cos(phi_poca), rho * sin(phi_poca), z);
    TVector3 trkDir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

    for(auto hit = fTrack->GetHits()->begin(); hit != fTrack->GetHits()->end(); ++hit){
        int channel = (*hit)->GetChannelID();
        double driftTime = (*hit)->GetDriftTime(0);
        if(driftTime > 400) continue;
        double DOCA = CDCGeom::Get().GetDOCA(trkPos, trkDir, channel);
        double t_expect = CalibInfo::Get().GetTAtR(DOCA);
        //      std::cout<<"DOCA:t_meas:t_expect "<<DOCA<<":"<<driftTime<<":"<<t_expect<<std::endl;
        double sigma = CalibInfo::Get().GetTimeResolution(DOCA);
        chi2 += (driftTime - t_expect) * (driftTime - t_expect) / (sigma * sigma);
    }
    return chi2;
}

double TrackFitMinimizerZ::FittingFunctionXYZT(double const* pars)
{
    double rho        = pars[0];
    double phi        = pars[1];
    double z          = pars[2];
    double theta      = pars[3];
    double chi2 = 0.;
    double phi_poca = phi - TMath::Pi() / 2;
    TVector3 trkPos(rho * cos(phi_poca), rho * sin(phi_poca), z);
    TVector3 trkDir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    TVector2 pos_cell;
    double shift;

    if(runMode){
        static int rr = 0;
        rr++;
        std::cout<<"++++++++++++++++++++++++++++++minimize round: "<<rr<<std::endl;
        std::cout<<std::fixed<<std::setprecision(6)<<"rho : phi : z : theta "<<rho<<" : "<<phi<<" : "<<z<<" : "<<theta<<std::endl;
    }
    for(auto hit = fTrack->GetHits()->begin(); hit != fTrack->GetHits()->end(); ++hit){
        int channel = (*hit)->GetChannelID();
        double driftTime = (*hit)->GetDriftTime(0);
        double t_expect = CalibInfo::Get().GetDriftTime(trkPos, trkDir, channel, pos_cell, shift);
        if(runMode) std::cout<<"x:y:shift "<<pos_cell.X()<<":"<<pos_cell.Y()<<":"<<shift<<" t_meas:t_expect:residual "<<driftTime<<":"<<t_expect<<":"<<driftTime - t_expect<<std::endl;
        double sigma = CalibInfo::Get().GetTimeResolution(pos_cell.X(), pos_cell.Y(), shift);
        chi2 += (driftTime - t_expect) * (driftTime - t_expect) / (sigma * sigma);
    }
    if(runMode){
        std::cout<<"chi2 = "<<chi2<<std::endl;
    }
    return chi2;
}

void TrackFitMinimizerZ::UpdateTrack(double const* pars, double const* errors){
    double rho        = pars[0];
    double phi        = pars[1];
    double z          = pars[2];
    double theta      = pars[3];
    double chi2 = 0.;
    double phi_poca = phi - TMath::Pi() / 2;
    TVector3 trkPos(rho * cos(phi_poca), rho * sin(phi_poca), z);
    TVector3 trkDir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

    fTrack->SetPos(trkPos);
    fTrack->SetDir(trkDir);

    double err_rho    = errors[0];
    double err_phi    = errors[1];
    double err_z      = errors[2];
    double err_theta  = errors[3];
    fTrack->SetRhoError(err_rho);
    fTrack->SetPhiError(err_phi);
    fTrack->SetAlphaError(err_z);
    fTrack->SetThetaError(err_theta);
    //  std::cout<<"updated track rho:phi:z:z:theta "<<rho<<":"<<phi<<":"<<rho*sin(phi_poca)/tan(z)<<":"<<z<<":"<<theta<<" "<<fTrack->GetDir().Theta()<<std::endl;
    //  std::cout<<"updated track rho:phi:z:theta "<<rho<<":"<<phi<<":"<<z<<":"<<theta<<std::endl;

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

bool TrackFitMinimizerZ::registered = [](){
    TrackFitMinimizerFactory::Get().RegisterTrackFitMinimizer("TrackFitMinimizerZ",
        [](std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType){
            return std::make_shared<TrackFitMinimizerZ>(track, minimizerType);
        }
    );
    std::cout<<"TrackFitMinimizerZ registered to minimizer factory"<<std::endl;
    return true;
}();
