#include "TrackFitHandler.hxx"

TrackFitHandler::TrackFitHandler(){

}

TrackFitHandler* TrackFitHandler::fTrackFitHandler = nullptr;

TrackFitHandler& TrackFitHandler::Get(){
	if(!fTrackFitHandler){
		fTrackFitHandler = new TrackFitHandler();
	}
	return *fTrackFitHandler;
}

CDCLineCandidateContainer* TrackFitHandler::Find3DTracks(CDCLineCandidateContainer* lines){
	std::vector<std::map<unsigned int, CDCLineCandidate*>> pairs;
	CDCLineCandidateContainer* tracks = new CDCLineCandidateContainer();
	//Preprocess of line candidates
	for(auto lineOdd = lines->begin(); lineOdd != lines->end(); ++lineOdd){
		//pick a odd layer candidate
		if( (*lineOdd)->GetOddEven() == 1 ){
			for(auto lineEven = lines->begin(); lineEven != lines->end(); ++lineEven){
				//pick up a even layer candidate
				if( (*lineEven)->GetOddEven() == 0 ){
					if(IsGoodPair(*lineOdd, *lineEven)){
						std::map<unsigned int, CDCLineCandidate*> linePair;
						linePair[lineOdd - lines->begin()] = *lineOdd;
						linePair[lineEven - lines->begin()] = *lineEven;
						pairs.push_back(std::move(linePair));
					}
				}
			}
		}
	}

	//3D fitting with all layers
//	std::cout<<"start 3D track fitting"<<std::endl;
	for(std::vector<std::map<unsigned int, CDCLineCandidate*>>::const_iterator linePair = pairs.begin(); linePair != pairs.end(); ++linePair){
		CDCLineCandidate* lineOdd;
		CDCLineCandidate* lineEven;
		for(auto line = linePair->begin(); line != linePair->end(); ++line){
			if( line->second->GetOddEven() == 1) lineOdd = line->second;
			else lineEven = line->second;
		}
		CDCLineCandidate* track = FindInitialTrack(lineOdd, lineEven);
		TrackFitting(track);
		tracks->push_back(track);
	}

	return tracks;
}

bool TrackFitHandler::IsGoodPair(CDCLineCandidate* lineOdd, CDCLineCandidate* lineEven){
	double phiOdd = lineOdd->GetDir().Phi();
	TVector3 posOdd = lineOdd->GetPos();
	double phiEven = lineEven->GetDir().Phi();
	TVector3 posEven = lineEven->GetPos();
	double dis = TVector3(posOdd - posEven).Mag();
	double phiDiff = phiOdd - phiEven;
//	std::cout<<"phiOdd:phiEven "<<phiOdd<<":"<<phiEven<<" , phi difference is "<<phiDiff<<std::endl;
//	std::cout<<"posOdd:posEven "<<posOdd.X()<<","<<posOdd.Y()<<":"<<posEven.X()<<","<<posEven.Y()<<" distance is "
//		 <<dis<<std::endl;
	//distance cut
	if( dis > fMaxDistanceEO ){
//		std::cout<<"out of distance range"<<std::endl;
		return false;
	}
	//angle difference cut
	else if( !(phiDiff < fMaxPhiDiff && phiDiff > -fMaxPhiDiff)                            &&
		 !(phiDiff < TMath::Pi() + fMaxPhiDiff && phiDiff > TMath::Pi() - fMaxPhiDiff) &&
		 !(phiDiff < -TMath::Pi() + fMaxPhiDiff && phiDiff > -TMath::Pi() - fMaxPhiDiff)){
//		std::cout<<"out of angle range"<<std::endl;
		return false;
	}
	else{
//		std::cout<<"return good pair"<<std::endl;
		return true;
	}
}

CDCLineCandidate* TrackFitHandler::FindInitialTrack(CDCLineCandidate* lineOdd, CDCLineCandidate* lineEven){
	CDCLineCandidate* track = new CDCLineCandidate();

	TVector3 posOdd = lineOdd->GetPos();
	TVector3 posEven = lineEven->GetPos();
	TVector3 pos = ( posOdd + posEven) * 0.5;
	TVector3 dirOdd = lineOdd->GetDir();
	TVector3 dirEven = lineEven->GetDir();
	double phiOdd = dirOdd.Phi();
	if(phiOdd < 0) phiOdd += TMath::Pi();
	double phiEven = dirEven.Phi();
	if(phiEven < 0) phiEven += TMath::Pi();
	double phi = (phiOdd + phiEven) / 2;
//	std::cout<<"phiOdd:phiEven:phi "<<phiOdd<<":"<<phiEven<<":"<<phi<<std::endl;

	//find x of bottum hit and top hit
	double xOddBottum = lineOdd->GetXAtY(-850.);
	double xOddTop    = lineOdd->GetXAtY(850.);
	double xEvenBottum = lineEven->GetXAtY(-850.);
	double xEvenTop    = lineEven->GetXAtY(850.);
	double cdcLength = CDCGeom::Get().GetCDCLength();
	double projectZ = ((xOddBottum - xEvenBottum) - (xOddTop - xEvenTop)) / fMaxDistanceEO * cdcLength;
	double theta;
	if(projectZ == 0) theta = TMath::Pi() / 2;
	theta = atan( 1750. / projectZ);
//	std::cout<<"xOddBottum:xEvenBottum:xOddTop:xEvenTop:projectZ "<<xOddBottum<<":"<<xEvenBottum<<":"<<xOddTop<<":"<<xEvenTop<<":"<<projectZ<<std::endl;
	std::cout<<"phi:theta "<<phi<<":"<<theta<<std::endl;

	//Set track parameters
	TVector3 dir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
	track->SetDir(dir);

	double posZ = cdcLength / 2 - (xOddBottum - xEvenBottum + xOddTop - xEvenTop) / 2 / fMaxDistanceEO * cdcLength;
	pos.SetZ(posZ);
	track->SetPos(pos);
//	std::cout<<"z position of track "<<posZ<<std::endl;

	//Store hits in track
	for(auto hitOdd = lineOdd->GetHits()->begin(); hitOdd != lineOdd->GetHits()->end(); ++hitOdd){
		int channel = (*hitOdd)->GetChannelID();
		double z = CDCGeom::Get().GetWireTrackIntersectionZY(track, channel).X();
		std::cout<<"hitZ is "<<z<<std::endl;
		(*hitOdd)->SetZ(z);
		track->AddHit(*hitOdd);
	} 
	for(auto hitEven = lineEven->GetHits()->begin(); hitEven != lineEven->GetHits()->end(); ++hitEven){
		int channel = (*hitEven)->GetChannelID();
		double z = CDCGeom::Get().GetWireTrackIntersectionZY(track, channel).X();
		std::cout<<"hitZ is "<<z<<std::endl;
		(*hitEven)->SetZ(z);
		track->AddHit(*hitEven);
	} 
/*
	//Final check of z position of hits, if out of CDC then abort this track
	for(auto hit = track->GetHits()->begin(); hit != track->GetHits()->end(); ++hit){
		if( (*hit)->GetZ() < -800. || (*hit)->GetZ() > 800.){
			delete track;
			return nullptr;
		}
	}
*/	return track;
}

void TrackFitHandler::TrackFitting(CDCLineCandidate* track){

}
