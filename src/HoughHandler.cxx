#include "HoughHandler.hxx"

using RuntimePar::maxItr;
using RuntimePar::coefficient; //ratio between bins of hough and fill intervals
using RuntimePar::nbinPhi;
using RuntimePar::nFillPhi;
using RuntimePar::minPhi;
using RuntimePar::maxPhi;
using RuntimePar::dPhi;
using RuntimePar::dFillPhi;
using RuntimePar::nbinRho;
using RuntimePar::minRho;
using RuntimePar::maxRho;
using RuntimePar::dRho;

HoughHandler::~HoughHandler(){

}

HoughHandler* HoughHandler::fHoughHandler = nullptr;

HoughHandler& HoughHandler::Get(){
	if(!fHoughHandler){
		fHoughHandler = new HoughHandler();
	}
	return *fHoughHandler;
}

CDCLineCandidateContainer* HoughHandler::FindCandidates(CDCHitContainer* hits){
	TH2D* houghOdd = new TH2D("houghOdd", "Hough transform for odd layers", nbinPhi, minPhi-0.5*dPhi, maxPhi-0.5*dPhi, nbinRho, minRho, maxRho);
	TH2D* houghEven = new TH2D("houghEven", "Hough transform for even layers", nbinPhi, minPhi-0.5*dPhi, maxPhi-0.5*dPhi, nbinRho, minRho, maxRho);

	CDCLineCandidateContainer* lines = new CDCLineCandidateContainer();
	CDCHitContainer remainHits = *hits;
	for(int itr = 0; itr < maxItr; ++itr){
//		std::cout<<"start "<<itr<<"th iteration of hough transform"<<std::endl;
		for(std::vector<CDCHit*>::const_iterator hit = remainHits.begin(); hit != remainHits.end(); ++hit){
			for(int iFillPhi = 0; iFillPhi < nFillPhi; ++iFillPhi){
				int channel = (*hit)->GetChannelID();
				TVector2 pos = CDCGeom::Get().ChannelToROPos(channel);

				double theta = iFillPhi * dFillPhi - TMath::Pi() * 0.5;
				if(theta < 0) theta += TMath::Pi() * 2;
				double rho = pos.Mod() * cos(theta - pos.Phi());
//				std::cout<<"theta:rho "<<theta<<":"<<rho<<std::endl;

				if(CDCGeom::Get().ChannelToLayer(channel) % 2 == 1){
					houghOdd->Fill(theta, rho);
				}
				else{
					houghEven->Fill(theta, rho);
				}
			}
		}

		//Find peak position and line candidate
		CDCLineCandidate* lineOdd = FindCandidate(houghOdd, 1);
		CDCLineCandidate* lineEven = FindCandidate(houghEven, 0);

		//Save line candidate
		if(IsGoodCandidate(lineOdd, lineEven, &remainHits)){
//			std::cout<<"found good candidate at "<<itr + 1<<"th Hough transform"<<std::endl;
			if(lineOdd) lines->push_back(lineOdd);
			if(lineEven) lines->push_back(lineEven);
//			std::cout<<"size of candidate container: "<<lines->size()<<std::endl;
		}
		else{
//			std::cout<<"no good candidate at "<<itr + 1<<"th Hough transform"<<std::endl;
			delete lineOdd;
			delete lineEven;
			break;
		}
/*
		std::cout<<"Draw hough histograms"<<std::endl;
		TCanvas* c = new TCanvas();
		c->Divide(2,1);
		c->cd(1);
		houghOdd->Draw("COLZ");
		c->cd(2);
		houghEven->Draw("COLZ");
		break;
*/
	}

	//release memory
	delete houghOdd;
	delete houghEven;

	return lines;
}

CDCLineCandidate* HoughHandler::FindCandidate(TH2D* hist, bool oddEven){
	int binX, binY, binZ;
	hist->GetMaximumBin(binX, binY, binZ);

	double theta = hist->GetXaxis()->GetBinCenter(binX);
	double rho = hist->GetYaxis()->GetBinCenter(binY);
	double maxVal = hist->GetBinContent(binX, binY);
//	std::cout<<"Peak at theta:rho "<<theta<<":"<<rho<<std::endl;

	TVector3 pos(rho * cos(theta), rho * sin(theta), 0);
	TVector3 dir(-sin(theta), cos(theta), 0);
//	std::cout<<"pos:dir "<<pos.X()<<","<<pos.Y()<<":"<<dir.X()<<","<<dir.Y()<<std::endl;

	CDCLineCandidate* line = new CDCLineCandidate(pos, dir, oddEven);
	return line;
}

bool HoughHandler::IsGoodCandidate(CDCLineCandidate*& lineOdd, CDCLineCandidate*& lineEven, CDCHitContainer* remainHits){
	//Loop over hits to collect hits that used by this line
//	std::cout<<"Start to judge if candidate is good"<<std::endl;
	TVector3 posOdd = lineOdd->GetPos();
	TVector3 dirOdd = lineOdd->GetDir();
	TVector3 posEven = lineEven->GetPos();
	TVector3 dirEven = lineEven->GetDir();
	std::queue<unsigned int> usedAt;
	for(auto hit = remainHits->begin(); hit != remainHits->end(); ++hit){
		int const channel = (*hit)->GetChannelID();
		int const layer = CDCGeom::Get().ChannelToLayer( channel );
		TVector2 posHit = CDCGeom::Get().ChannelToROPos(channel);
		TVector3 posHit3(posHit.X(), posHit.Y(), 0);
		if(layer % 2 == 1){
			if(CalculateDistance(posHit3, posOdd, dirOdd) < fLineDisThreshold){
				lineOdd->AddHit(*hit);
				usedAt.push(hit - remainHits->begin());
			}
//			else std::cout<<"unused hit at channel"<<channel<<std::endl;
		}
		else{
			if(CalculateDistance(posHit3, posEven, dirEven) < fLineDisThreshold){
                                lineEven->AddHit(*hit);
				usedAt.push(hit - remainHits->begin());
                        }
//			else std::cout<<"unused hit at channel"<<channel<<std::endl;
		}
	}
//	std::cout<<"Finish judge if candidate is good"<<std::endl;

	//update hit container
	if(usedAt.size() != 0){
		CDCHitContainer newRemainHits;
		for(auto hit = remainHits->begin(); hit != remainHits->end(); ++hit){
			if(usedAt.empty()){
				newRemainHits.push_back(*hit);
				continue;
			}
//			std::cout<<hit - remainHits->begin()<<" "<<usedAt.front()<<std::endl;
			if(hit - remainHits->begin() != usedAt.front()){
//				std::cout<<"push_back at"<<hit - remainHits->begin()<<std::endl;
				newRemainHits.push_back(*hit);
			}
			else{
//				std::cout<<"used hit at "<<usedAt.front()<<std::endl;
				usedAt.pop();
			}
		}
		remainHits->swap(newRemainHits);
		remainHits->shrink_to_fit();
//		std::cout<<"swap hit containers to the new one, new container has size of "<<remainHits->size()<<std::endl;
	}

	//return result
	if(lineOdd->GetHits()->size() > 5 && lineEven->GetHits()->size() > 5){
//		std::cout<<"good "<<std::endl;
		return true;
	}
	else if(lineOdd->GetHits()->size() > 5){
//		std::cout<<"partially (odd) good"<<std::endl;
		delete lineEven;
		lineEven = nullptr;
		return true;
	}
	else if(lineEven->GetHits()->size() > 5){
//		std::cout<<"partially (even) good"<<std::endl;
		delete lineOdd;
		lineOdd = nullptr;
		return true;
	}
	else return false;
}

double HoughHandler::CalculateDistance(TVector3 pos_point, TVector3 pos_line, TVector3 dir_line){
	TVector3 pointToLine = pos_line - pos_point;
	TVector3 crossProduct = pointToLine.Cross(dir_line);
//	std::cout<<"distance is "<<crossProduct.Mag() / dir_line.Mag()<<std::endl;
	return crossProduct.Mag() / dir_line.Mag();
}
