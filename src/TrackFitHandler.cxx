#include "TrackFitHandler.hxx"

using RuntimePar::runMode;
using RuntimePar::XTMode;
using RuntimePar::numTrackCut;

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
    std::vector<std::map<unsigned int, std::shared_ptr<CDCLineCandidate>>> pairs;
    CDCLineCandidateContainer* tracks = new CDCLineCandidateContainer();
    //Preprocess of line candidates
    for(auto lineOdd = lines->begin(); lineOdd != lines->end(); ++lineOdd){
        //pick a odd layer candidate
        if( (*lineOdd)->GetOddEven() == 1 ){
            for(auto lineEven = lines->begin(); lineEven != lines->end(); ++lineEven){
                //pick up a even layer candidate
                if( (*lineEven)->GetOddEven() == 0 ){
                    if(IsGoodPair(*lineOdd, *lineEven)){
                        std::map<unsigned int, std::shared_ptr<CDCLineCandidate>> linePair;
                        linePair[lineOdd - lines->begin()] = *lineOdd;
                        linePair[lineEven - lines->begin()] = *lineEven;
                        pairs.push_back(linePair);
                    }
                }
            }
        }
    }

    //Number of track cut
    if(numTrackCut !=0 && pairs.size() > numTrackCut){
        std::cout<<"number of pairs "<<pairs.size()<<" been cut"<<std::endl;
        delete tracks;
        tracks = nullptr;
        return nullptr;
    }

    //3D fitting with hits with simple XT (radial symmetric) t < 400 ns
    //	std::cout<<"start 3D track fitting"<<std::endl;
    for(std::vector<std::map<unsigned int, std::shared_ptr<CDCLineCandidate>>>::const_iterator linePair = pairs.begin(); linePair != pairs.end(); ++linePair){
        std::shared_ptr<CDCLineCandidate> lineOdd;
        std::shared_ptr<CDCLineCandidate> lineEven;
        for(auto line = linePair->begin(); line != linePair->end(); ++line){
            if( line->second->GetOddEven() == 1) lineOdd = line->second;
            else lineEven = line->second;
        }
        std::shared_ptr<CDCLineCandidate> track = FindInitialTrack(lineOdd, lineEven);
        if(!track) continue; //FIXME check why hit.Z is so large, maybe because alpha is near 1.57?
        std::shared_ptr<TrackFitMinimizerBase> fit = TrackFitMinimizerFactory::Get().CreateTrackFitMinimizer(track);
        fit->TrackFitting("RT");
        //		fit.TrackFittingRTT0();
        tracks->push_back(track);
    }

    //Find the best candidate as the triggered track. then exclude pairs that contain hits in triggered track
    //TODO to be done

    //Fit with T0 for tracks that are not triggered track
    //TODO to be done

    //precise fit with all hits and precise XT
    ReFit(tracks);

    return tracks;
}

void TrackFitHandler::ReFit(CDCLineCandidateContainer* tracks){
    for(std::shared_ptr<CDCLineCandidate> track : (*tracks)){
        std::shared_ptr<TrackFitMinimizerBase> fit = TrackFitMinimizerFactory::Get().CreateTrackFitMinimizer(track);
        fit->TrackFitting("XYZT");
        std::shared_ptr<CDCLineCandidateContainer> tracks_residual = std::make_shared<CDCLineCandidateContainer>();
        CDCHitContainer* hits_all = track->GetHits();
        for(auto hit = hits_all->begin(); hit != hits_all->end(); ++hit){
//            std::cout<<"Fitting track by excluding each hit to get residual, now "<<hit - hits_all->begin()<<" th hit"<<std::endl;
            CDCHitContainer* hits = new CDCHitContainer();
            std::copy(hits_all->begin(), hit, std::back_inserter(*hits));
            std::copy(hit + 1, hits_all->end(), std::back_inserter(*hits));
            std::shared_ptr<CDCLineCandidate> track_residual = std::make_shared<CDCLineCandidate>(*track);
            track_residual->SetHits(hits);
            std::shared_ptr<TrackFitMinimizerBase> fit_residual = TrackFitMinimizerFactory::Get().CreateTrackFitMinimizer(track_residual);
            fit_residual->TrackFitting("XYZT");
            tracks_residual->push_back(track_residual);
            (*hit)->SetResidual((*hit)->GetDriftTime(0) - CalibInfo::Get().GetDriftTime(track_residual->GetPos(), track_residual->GetDir(), (*hit)->GetChannelID()));
        }
        track->SetTrackResidual(tracks_residual);
    }
}

bool TrackFitHandler::IsGoodPair(std::shared_ptr<CDCLineCandidate> lineOdd, std::shared_ptr<CDCLineCandidate> lineEven){
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
        if(runMode) std::cout<<"out of distance range"<<std::endl;
        return false;
    }
    //angle difference cut
    else if( !(phiDiff < fMaxPhiDiff && phiDiff > -fMaxPhiDiff)                            &&
            !(phiDiff < TMath::Pi() + fMaxPhiDiff && phiDiff > TMath::Pi() - fMaxPhiDiff) &&
            !(phiDiff < -TMath::Pi() + fMaxPhiDiff && phiDiff > -TMath::Pi() - fMaxPhiDiff)){
        if(runMode) std::cout<<"out of angle range"<<std::endl;
        return false;
    }
    else{
        return true;
    }
}

std::shared_ptr<CDCLineCandidate> TrackFitHandler::FindInitialTrack(std::shared_ptr<CDCLineCandidate> lineOdd, std::shared_ptr<CDCLineCandidate> lineEven){
    std::shared_ptr<CDCLineCandidate> track = std::make_shared<CDCLineCandidate>();

    TVector3 posOdd = lineOdd->GetPos();
    TVector3 posEven = lineEven->GetPos();
    TVector3 pos = ( posOdd + posEven) * 0.5;
    double phiOdd = lineOdd->GetDir().Phi();
    double phiEven = lineEven->GetDir().Phi();
    double phi = (phiOdd + phiEven) / 2;
//    	std::cout<<"phiOdd:phiEven:phi "<<phiOdd<<":"<<phiEven<<":"<<phi<<std::endl;

    //Find theta and POCA.Z of the track
    double xOddBottum, yOddBottum, xOddTop, yOddTop;
    double xEvenBottum, yEvenBottum, xEvenTop, yEvenTop;
    lineOdd->GetROXYAtR(850., xOddBottum, yOddBottum, xOddTop, yOddTop);
    lineEven->GetROXYAtR(850., xEvenBottum, yEvenBottum, xEvenTop, yEvenTop);
    double disBottum = sqrt( pow(xOddBottum - xEvenBottum, 2) + pow(yOddBottum - yEvenBottum, 2) );
    if(atan2(yOddBottum, xOddBottum) < atan2(yEvenBottum, xEvenBottum)) disBottum = -disBottum;
    double disTop = sqrt( pow(xOddTop - xEvenTop, 2) + pow(yOddTop - yEvenTop, 2) );
    if(atan2(yOddTop, xOddTop) > atan2(yEvenTop, xEvenTop)) disTop = -disTop;
    double cdcLength = CDCGeom::Get().GetCDCLength();
    double projectZ = (disBottum - disTop) / fMaxDistanceEO * cdcLength;
    double theta = atan2(1750., projectZ);
    if(sin(phi) < 0 && theta < TMath::Pi() / 2) theta = TMath::Pi() - theta;
    if(sin(phi) > 0 && theta > TMath::Pi() / 2) theta = TMath::Pi() - theta;
    //	std::cout<<"xOddBottum:xEvenBottum:xOddTop:xEvenTop:projectZ "<<xOddBottum<<":"<<xEvenBottum<<":"<<xOddTop<<":"<<xEvenTop<<":"<<projectZ<<std::endl;
    //	std::cout<<"disBottum:disTop:projectZ "<<disBottum<<":"<<disTop<<":"<<projectZ<<std::endl;
    TVector3 dir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    track->SetDir(dir);

    double posZ = cdcLength / 2 - (disBottum + disTop) / 2 / fMaxDistanceEO * cdcLength;
    pos.SetZ(posZ);
    track->SetPos(pos);
    //	std::cout<<"z position of track "<<posZ<<std::endl;

    //Set z position of hits in track
    for(auto hitOdd = lineOdd->GetHits()->begin(); hitOdd != lineOdd->GetHits()->end(); ++hitOdd){
        int channel = (*hitOdd)->GetChannelID();
        double z = CDCGeom::Get().GetWireTrackIntersectionZY(track, channel).X();
        (*hitOdd)->SetZ(z);
        //		std::cout<<"z position of hit "<<z<<std::endl;
        CDCHit* hit = new CDCHit(**hitOdd);
        track->AddHit(hit);
    } 
    for(auto hitEven = lineEven->GetHits()->begin(); hitEven != lineEven->GetHits()->end(); ++hitEven){
        int channel = (*hitEven)->GetChannelID();
        double z = CDCGeom::Get().GetWireTrackIntersectionZY(track, channel).X();
        (*hitEven)->SetZ(z);
        //		std::cout<<"z position of hit "<<z<<std::endl;
        CDCHit* hit = new CDCHit(**hitEven);
        track->AddHit(hit);
    }

    //Final check of z position of hits, if out of CDC then abort this track
    for(auto hit = track->GetHits()->begin(); hit != track->GetHits()->end(); ++hit){
        int channel = (*hit)->GetChannelID();
        if( (*hit)->GetZ() < -1500. || (*hit)->GetZ() > 1500.){
            if(runMode) std::cout<<"Track candidate initial Z out of range, abort. ch = "<<channel<<" Z = "<<(*hit)->GetZ()<<std::endl;
            track.reset();
            return nullptr;
        }
        else if( (*hit)->GetZ() < -1000.){
            (*hit)->SetZ(-CDCGeom::Get().ChannelToMaximumZ(channel));
        }
        else if( (*hit)->GetZ() > 1000.){
            (*hit)->SetZ(CDCGeom::Get().ChannelToMaximumZ(channel));
        }

    }
    return track;
}
