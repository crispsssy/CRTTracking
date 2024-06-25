#include "TrackFitMinimizer.hxx"

using RuntimePar::XTMode;

TrackFitMinimizer::TrackFitMinimizer(CDCLineCandidate* track){
	fTrack = track;
	fFit = ROOT::Math::Factory::CreateMinimizer("Minuit2");
        if(!fFit){
                std::cerr<<"Can not create minimizer"<<std::endl;
                exit(-1);
        }
//	std::cout<<"fit constructed"<<std::endl;
}

TrackFitMinimizer::~TrackFitMinimizer(){
	delete fFit;
}

void TrackFitMinimizer::TrackFitting(){
	if(!fTrack) return;

        fFit->SetTolerance(0.001);
        fFit->SetPrintLevel(-1);

	std::function<double(double const*)> func = [this](double const* pars){
		return this->FittingFunctionRT(pars);
	};
        ROOT::Math::Functor functionRT(func, 4);
        fFit->SetFunction(functionRT);

	TVector3 pocaT;
	TVector3 pocaW;
	CDCGeom::Get().GetPOCA(fTrack->GetPos(), fTrack->GetDir(), TVector3(0., 0., 0.), TVector3(0., 0., 1.), pocaT, pocaW);
	TVector2 pocaTXY(pocaT.X(), pocaT.Y());
        fFit->SetVariable(0,     "rho",          pocaTXY.Mod(),                             0.001);
        fFit->SetVariable(1,     "phi",     	 fTrack->GetDir().Phi(),                      0.001);
        fFit->SetVariable(2,     "alpha",        atan2(pocaT.Y(), pocaT.Z()),               0.001);
        fFit->SetVariable(3,     "theta",        fTrack->GetDir().Theta(),                  0.001);
        fFit->SetVariableLimits(0,       0.,             850.);
        fFit->SetVariableLimits(1,       0.,             TMath::Pi()*2);
        fFit->SetVariableLimits(2,       -TMath::Pi()*2,   TMath::Pi()*2);
        fFit->SetVariableLimits(3,       0.,             TMath::Pi()*2);
	std::cout<<"rho:phi:z:alpha:theta "<<pocaTXY.Mod()<<":"<<fTrack->GetDir().Phi()<<":"<<pocaT.Z()<<":"<<atan2(pocaT.Y(),pocaT.Z())<<":"<<fTrack->GetDir().Theta()<<std::endl;

//	std::cout<<"Start minimize with TMinuit2"<<std::endl;
        fFit->Minimize();
	fTrack->SetChi2( fFit->MinValue() );
	double const* pars = fFit->X();
//	std::cout<<"Minimum chi2 is "<<fFit->MinValue()<<std::endl;
	UpdateTrack(pars);

	Optimize();
}

double TrackFitMinimizer::GetChi2(){
	double const pars[4]{fTrack->GetXAtZ(0), fTrack->GetYAtZ(0.), fTrack->GetDir().Phi(), fTrack->GetDir().Theta() };
	return FittingFunctionRT(pars);
}

double TrackFitMinimizer::FittingFunctionRT(double const* pars){
//	std::cout<<"Fitting Function RT called"<<std::endl;
        //Define chi2 of fFitting here
	double rho        = pars[0];
	double phi        = pars[1];
	double alpha      = pars[2];
	double theta      = pars[3];
	double chi2 = 0.;
	double phi_poca = phi - TMath::Pi() / 2;
	TVector3 trkPos(rho * cos(phi_poca), rho * sin(phi_poca), rho * sin(phi_poca) / tan(alpha));
	TVector3 trkDir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
//	std::cout<<"px:py:pz : alpha:phi:theta "<<trkPos.X()<<":"<<trkPos.Y()<<":"<<trkPos.Z()<<" : "<<alpha<<":"<<trkDir.Phi()<<":"<<trkDir.Theta()<<std::endl;

//	std::cout<<"start to loop over hits to calculate chi2"<<std::endl;
	for(auto hit = fTrack->GetHits()->begin(); hit != fTrack->GetHits()->end(); ++hit){
		int channel = (*hit)->GetChannelID();
		double DOCA = CDCGeom::Get().GetDOCA(trkPos, trkDir, channel);
		double driftTime = (*hit)->GetDriftTime(0);
		double t_expect = CalibInfo::Get().GetTAtR(DOCA);
//		std::cout<<"DOCA:t_meas:t_expect "<<DOCA<<":"<<driftTime<<":"<<t_expect<<std::endl;
		double sigma = CalibInfo::Get().GetTimeResolution(driftTime);
		chi2 += pow(driftTime - t_expect, 2) / pow(sigma, 2);
	}
//	std::cout<<"!!!!!!!!!!!!!!chi2: "<<chi2<<std::endl;
	return chi2;
}

double TrackFitMinimizer::FittingFunctionRTT0(double const* pars){
	//Define chi2 of fFitting here
	return 999;
}

void TrackFitMinimizer::UpdateTrack(double const* pars){
	double rho        = pars[0];
	double phi        = pars[1];
	double alpha      = pars[2];
	double theta      = pars[3];
	double chi2 = 0.;
	double phi_poca = phi - TMath::Pi() / 2;
	TVector3 trkPos(rho * cos(phi_poca), rho * sin(phi_poca), rho * sin(phi_poca) / tan(alpha));
	TVector3 trkDir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

	fTrack->SetPos(trkPos);
	fTrack->SetDir(trkDir);
	std::cout<<"updated track rho:phi:z:alpha:theta "<<rho<<":"<<phi<<":"<<rho*sin(phi_poca)/tan(alpha)<<":"<<alpha<<":"<<theta<<std::endl;

	if(XTMode == "RT"){
		//update hit position at z
		for(auto hit = fTrack->GetHits()->begin(); hit!= fTrack->GetHits()->end(); ++hit){
			int channel = (*hit)->GetChannelID();
			TVector3 pocaT;
			TVector3 pocaW;
			CDCGeom::Get().GetPOCA(trkPos, trkDir, channel, pocaT, pocaW);
			double DOCA = (pocaT - pocaW).Mag();
			(*hit)->SetZ(pocaW.Z());
			(*hit)->SetDOCA(DOCA);
		}
	}
	else if(XTMode == "RZT"){
		std::cout<<std::endl;
	}
	else if(XTMode == "XYZT"){
		std::cout<<std::endl;
	}
	else{
		std::cout<<"No such kind of XT mode!!"<<std::endl;
		exit(-1);
	}
}

void TrackFitMinimizer::Optimize(){

}
