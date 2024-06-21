#include "TrackFitMinimizer.hxx"

using RuntimePar::XTMode;

TrackFitMinimizer::TrackFitMinimizer(CDCLineCandidate* track){
	fTrack = track;
	fFit = ROOT::Math::Factory::CreateMinimizer("Minuit2");
        if(!fFit){
                std::cerr<<"Can not create minimizer"<<std::endl;
                exit(-1);
        }
	std::cout<<"fit constructed"<<std::endl;
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
        fFit->SetVariable(0,     "x",     fTrack->GetXAtZ(0.),       0.01);
        fFit->SetVariable(1,     "y",     fTrack->GetYAtZ(0.),       0.01);
        fFit->SetVariable(2,     "phi",   fTrack->GetDir().Phi(),    0.001);
        fFit->SetVariable(3,     "theta", fTrack->GetDir().Theta(),  0.001);
        fFit->SetVariableLimits(0,       -1700.,       1700.);
        fFit->SetVariableLimits(1,       -1700.,       1700.);
        fFit->SetVariableLimits(2,       0.,           TMath::Pi() * 2);
        fFit->SetVariableLimits(3,       -TMath::Pi(), TMath::Pi());

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
	double xAtZ0 = pars[0];
	double yAtZ0 = pars[1];
	double phi = pars[2];
	double theta = pars[3];
	double chi2 = 0.;
	TVector3 trkPosZ0(xAtZ0, yAtZ0, 0.);
	TVector3 trkDir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

//	std::cout<<"start to loop over hits to calculate chi2"<<std::endl;
	for(auto hit = fTrack->GetHits()->begin(); hit != fTrack->GetHits()->end(); ++hit){
		int channel = (*hit)->GetChannelID();
		double DOCA = CDCGeom::Get().GetDOCA(trkPosZ0, trkDir, channel);
		double driftTime = (*hit)->GetDriftTime(0);
		double driftDistance = CalibInfo::Get().GetRAtT(driftTime);
		double sigma = CalibInfo::Get().GetSpatialResolution(driftDistance);
		chi2 += pow(DOCA - driftDistance, 2) / pow(sigma, 2);
	}
	return chi2;
}

void TrackFitMinimizer::UpdateTrack(double const* pars){
	if(XTMode == "RT"){
		TVector3 trkPosZ0(pars[0], pars[1], 0.);
		TVector3 trkDir(sin(pars[3]) * cos(pars[2]), sin(pars[3]) * sin(pars[2]), cos(pars[3]));
		TVector3 trkPos;
		TVector3 zPos; //don't use
		CDCGeom::Get().GetPOCA(trkPosZ0, trkDir, TVector3(0., 0., 0.), TVector3(0., 0., 1.), trkPos, zPos);
		
		fTrack->SetPos(trkPos);
		fTrack->SetDir(trkDir);
		
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
