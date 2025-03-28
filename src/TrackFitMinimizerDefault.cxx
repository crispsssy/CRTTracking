#include "TrackFitMinimizerDefault.hxx"

using RuntimePar::runMode;

TrackFitMinimizerDefault::TrackFitMinimizerDefault(std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType)
: TrackFitMinimizerBase(track, minimizerType)
{

}

void TrackFitMinimizerDefault::TrackFitting(std::string const& XTMode){
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
    //	std::cout<<"rho:phi:z:alpha:theta "<<pocaTXY.Mod()<<":"<<fTrack->GetDir().Phi()<<":"<<pocaT.Z()<<":"<<atan2(pocaT.Y(),pocaT.Z())<<":"<<fTrack->GetDir().Theta()<<std::endl;

    //	std::cout<<"Start minimize with TMinuit2"<<std::endl;
    fFit->Minimize();
    fTrack->SetChi2( fFit->MinValue() );
    fTrack->SetNdf( fTrack->GetHits()->size() - 4 );
    double const* pars = fFit->X();
    double const* errors = fFit->Errors();
//    std::cout<<"Minimum chi2 is "<<fFit->MinValue()<<std::endl;
    UpdateTrack(pars, errors);

}

void TrackFitMinimizerDefault::SetupParameters(std::string const& XTMode){
    TVector3 pocaT;
    TVector3 pocaW;
    CDCGeom::Get().GetPOCA(fTrack->GetPos(), fTrack->GetDir(), TVector3(0., 0., 0.), TVector3(0., 0., 1.), pocaT, pocaW);
    TVector2 pocaTXY(pocaT.X(), pocaT.Y());
    double rho = pocaTXY.Mod();
    double phi = fTrack->GetDir().Phi();
    double alpha = atan2(pocaT.Y(), pocaT.Z());
    double theta = fTrack->GetDir().Theta();
    double step = 1e-4;
    double rhoMin = 0.;
    double rhoMax = 850.;
    double phiMin = -TMath::Pi()*2;
    double phiMax = TMath::Pi()*2;
    double alphaMin = -TMath::Pi()*2;
    double alphaMax = TMath::Pi()*2;
    double thetaMin = -TMath::Pi()*2;
    double thetaMax = TMath::Pi()*2;
    fFit->SetTolerance(0.1);
    if(XTMode == "XYZT"){
        fFit->SetTolerance(0.001);
        fFit->SetStrategy(2);
        fFit->SetMaxFunctionCalls(1e4);
        step = 1e-5;
        double dRho = 2.;
        double dPhi = 2e-2;
        double dAlpha = 2e-2;
        double dTheta = 0.2;
        rhoMin = rho - dRho;
        rhoMax = rho + dRho;
        phiMin = phi - dPhi;
        phiMax = phi + dPhi;
        alphaMin = alpha - dAlpha;
        alphaMax = alpha + dAlpha;
        thetaMin = theta - dTheta;
        thetaMax = theta + dTheta;
    }
    if(runMode){
        std::cout<<"rho : rhoMin : rhoMax "<<rho<<":"<<rhoMin<<":"<<rhoMax<<std::endl;
        std::cout<<"phi : phiMin : phiMax "<<phi<<":"<<phiMin<<":"<<phiMax<<std::endl;
        std::cout<<"alpha : alphaMin : alphaMax "<<alpha<<":"<<alphaMin<<":"<<alphaMax<<std::endl;
        std::cout<<"theta : thetaMin : thetaMax "<<theta<<":"<<thetaMin<<":"<<thetaMax<<std::endl;
    }

    fFit->SetVariable(0,     "rho",          rho,               step);
    fFit->SetVariable(1,     "phi",          phi,               step);
    fFit->SetVariable(2,     "alpha",        alpha,             step);
    fFit->SetVariable(3,     "theta",        theta,             step);
    fFit->SetVariableLimits(0,       rhoMin,                    rhoMax);
    fFit->SetVariableLimits(1,       phiMin,                    phiMax);
    fFit->SetVariableLimits(2,       alphaMin,                  alphaMax);
    fFit->SetVariableLimits(3,       thetaMin,                  thetaMax);
}

        

double TrackFitMinimizerDefault::GetChi2(){
    double const pars[4]{fTrack->GetXAtZ(0), fTrack->GetYAtZ(0.), fTrack->GetDir().Phi(), fTrack->GetDir().Theta() };
    return FittingFunctionRT(pars);
}

double TrackFitMinimizerDefault::FittingFunctionRT(double const* pars){
    //Abort hits that drift time larger than 400 ns since simple XT doesn't fit corner hits.
//    	std::cout<<"Fitting Function RT called------------------------------"<<std::endl;
    //Define chi2 of fFitting here
    double rho        = pars[0];
    double phi        = pars[1];
    double alpha      = pars[2];
    double theta      = pars[3];
    ROOT::Math::KahanSum<double> chi2{0.0};
    double phi_poca = phi - TMath::Pi() / 2;
    if(alpha == 0 ) alpha = 1e-20;
    TVector3 trkPos(rho * cos(phi_poca), rho * sin(phi_poca), rho * sin(phi_poca) / tan(alpha));
    TVector3 trkDir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
//    	std::cout<<"px:py:pz : alpha:phi:theta "<<trkPos.X()<<":"<<trkPos.Y()<<":"<<trkPos.Z()<<" : "<<alpha<<":"<<trkDir.Phi()<<":"<<trkDir.Theta()<<std::endl;

    //	std::cout<<"start to loop over hits to calculate chi2"<<std::endl;
    for(auto hit = fTrack->GetHits()->begin(); hit != fTrack->GetHits()->end(); ++hit){
        int channel = (*hit)->GetChannelID();
        double driftTime = (*hit)->GetDriftTime(0);
        if(driftTime > fRTMaxDriftTime) continue; //drift time cut
        double DOCA = CDCGeom::Get().GetDOCA(trkPos, trkDir, channel);
        double t_expect = CalibInfo::Get().GetTAtR(DOCA);
        //		std::cout<<"DOCA:t_meas:t_expect "<<DOCA<<":"<<driftTime<<":"<<t_expect<<std::endl;
        double sigma = CalibInfo::Get().GetTimeResolution(DOCA);
        chi2.Add( (driftTime - t_expect) * (driftTime - t_expect) / (sigma * sigma) );
    }
    //	std::cout<<"!!!!!!!!!!!!!!chi2: "<<chi2.Sum()<<std::endl;
    return chi2.Sum();
}

double TrackFitMinimizerDefault::FittingFunctionXYZT(double const* pars)
{
    double rho        = pars[0];
    double phi        = pars[1];
    double alpha      = pars[2];
    double theta      = pars[3];
    ROOT::Math::KahanSum<double> chi2{0.0};
    double phi_poca = phi - TMath::Pi() / 2;
    TVector3 trkPos(rho * cos(phi_poca), rho * sin(phi_poca), rho * sin(phi_poca) / tan(alpha));
    TVector3 trkDir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    TVector2 pos_cell;
    double shift;

    if(runMode){
        static int rr = 0;
        rr++;
        std::cout<<"++++++++++++++++++++++++++++++minimize round: "<<rr<<std::endl;
        std::cout<<"rho : phi : alpha : theta "<<rho<<" : "<<phi<<" : "<<alpha<<" : "<<theta<<std::endl;
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

void TrackFitMinimizerDefault::UpdateTrack(double const* pars, double const* errors){
    double rho        = pars[0];
    double phi        = pars[1];
    double alpha      = pars[2];
    double theta      = pars[3];
    double phi_poca = phi - TMath::Pi() / 2;
    TVector3 trkPos(rho * cos(phi_poca), rho * sin(phi_poca), rho * sin(phi_poca) / tan(alpha));
    TVector3 trkDir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

    fTrack->SetPos(trkPos);
    fTrack->SetDir(trkDir);

    double err_rho    = errors[0];
    double err_phi    = errors[1];
    double err_alpha  = errors[2];
    double err_theta  = errors[3];
    fTrack->SetRhoError(err_rho);
    fTrack->SetPhiError(err_phi);
    fTrack->SetAlphaError(err_alpha);
    fTrack->SetThetaError(err_theta);
    //	std::cout<<"updated track rho:phi:z:alpha:theta "<<rho<<":"<<phi<<":"<<rho*sin(phi_poca)/tan(alpha)<<":"<<alpha<<":"<<theta<<" "<<fTrack->GetDir().Theta()<<std::endl;
    //	std::cout<<"updated track rho:phi:z:theta "<<rho<<":"<<phi<<":"<<z<<":"<<theta<<std::endl;

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

void TrackFitMinimizerDefault::TrackFittingRTT0(){
    if(!fTrack) return;

    fFit->SetTolerance(0.1);
    fFit->SetPrintLevel(1);

    std::function<double(double const*)> func = [this](double const* pars){
        return this->FittingFunctionRT(pars);
    };
    ROOT::Math::Functor functionRT(func, 5);
    fFit->SetFunction(functionRT);

    TVector3 pocaT;
    TVector3 pocaW;
    CDCGeom::Get().GetPOCA(fTrack->GetPos(), fTrack->GetDir(), TVector3(0., 0., 0.), TVector3(0., 0., 1.), pocaT, pocaW);
    TVector2 pocaTXY(pocaT.X(), pocaT.Y());
    double phi = atan2(pocaT.Y(), pocaT.X()) + TMath::Pi() / 2;
    fFit->SetVariable(0,     "rho",          pocaTXY.Mod(),                             0.0001);
    fFit->SetVariable(1,     "phi",          phi,                                       0.0001);
    fFit->SetVariable(2,     "alpha",        atan2(pocaT.Y(), pocaT.Z()),               0.0001);
    fFit->SetVariable(3,     "theta",        fTrack->GetDir().Theta(),                  0.0001);
    fFit->SetVariable(4,     "t0",           fTrack->GetDir().Theta(),                  1);
    fFit->SetVariableLimits(0,       0.,             850.);
    fFit->SetVariableLimits(1,       -TMath::Pi()*2,             TMath::Pi()*2);
    fFit->SetVariableLimits(2,       -TMath::Pi()*2,             TMath::Pi()*2);
    fFit->SetVariableLimits(3,       -TMath::Pi()*2,             TMath::Pi()*2);
    fFit->SetVariableLimits(4,       -100.,             900.);
    std::cout<<"rho:phi:z:alpha:theta "<<pocaTXY.Mod()<<":"<<phi<<":"<<pocaT.Z()<<":"<<atan2(pocaT.Y(),pocaT.Z())<<":"<<fTrack->GetDir().Theta()<<std::endl;

    //      std::cout<<"Start minimize with TMinuit2"<<std::endl;
    fFit->Minimize();
    fTrack->SetChi2( fFit->MinValue() );
    double const* pars = fFit->X();
    double const* errors = fFit->Errors();
    //      std::cout<<"Minimum chi2 is "<<fFit->MinValue()<<std::endl;
    UpdateTrack(pars, errors);

}

double TrackFitMinimizerDefault::FittingFunctionRTT0(double const* pars){
    double rho        = pars[0];
    double phi        = pars[1];
    double alpha      = pars[2];
    double theta      = pars[3];
    double t0         = pars[4];
    ROOT::Math::KahanSum<double> chi2{0.0};
    double phi_poca = phi - TMath::Pi() / 2;
    TVector3 trkPos(rho * cos(phi_poca), rho * sin(phi_poca), rho * sin(phi_poca) / tan(alpha));
    TVector3 trkDir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    //      std::cout<<"px:py:pz : alpha:phi:theta "<<trkPos.X()<<":"<<trkPos.Y()<<":"<<trkPos.Z()<<" : "<<alpha<<":"<<trkDir.Phi()<<":"<<trkDir.Theta()<<std::endl;
    std::cout<<"px:py:pz : phi:theta "<<trkPos.X()<<":"<<trkPos.Y()<<":"<<trkPos.Z()<<" : "<<":"<<trkDir.Phi()<<":"<<trkDir.Theta()<<std::endl;

    //      std::cout<<"start to loop over hits to calculate chi2"<<std::endl;
    for(auto hit = fTrack->GetHits()->begin(); hit != fTrack->GetHits()->end(); ++hit){
        int channel = (*hit)->GetChannelID();
        double DOCA = CDCGeom::Get().GetDOCA(trkPos, trkDir, channel);
        double driftTime = (*hit)->GetDriftTime(0) - t0;
        double t_expect = CalibInfo::Get().GetTAtR(DOCA);
        std::cout<<"DOCA:t_meas:t_expect "<<DOCA<<":"<<driftTime<<":"<<t_expect<<std::endl;
        double sigma = CalibInfo::Get().GetTimeResolution(driftTime);
        chi2.Add( pow(driftTime - t_expect, 2) / pow(sigma, 2) );
    }
    std::cout<<"!!!!!!!!!!!!!!chi2: "<<chi2.Sum()<<std::endl;
    return chi2.Sum();
}

bool TrackFitMinimizerDefault::registered = [](){
    TrackFitMinimizerFactory::Get().RegisterTrackFitMinimizer("TrackFitMinimizerDefault", 
        [](std::shared_ptr<CDCLineCandidate> track, std::string const& minimizerType) {
            return std::make_shared<TrackFitMinimizerDefault>(track, minimizerType);
        }
    );
    std::cout<<"TrackFitMinimizerDefault registered to minimizer factory"<<std::endl;
    return true;
}();
