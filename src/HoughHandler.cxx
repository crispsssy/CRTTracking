#include "HoughHandler.hxx"

using RuntimePar::runMode;
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

std::shared_ptr<CDCLineCandidateContainer> HoughHandler::FindCandidates(std::shared_ptr<CDCHitContainer> hits){
	TH2D* houghOdd = new TH2D("houghOdd", "Hough transform for odd layers", nbinPhi, minPhi-0.5*dPhi, maxPhi-0.5*dPhi, nbinRho, minRho, maxRho);
	TH2D* houghEven = new TH2D("houghEven", "Hough transform for even layers", nbinPhi, minPhi-0.5*dPhi, maxPhi-0.5*dPhi, nbinRho, minRho, maxRho);

	std::shared_ptr<CDCLineCandidateContainer> lines = std::make_shared<CDCLineCandidateContainer>();
	CDCHitContainer remainHits = *hits;
	for(int itr = 0; itr < maxItr; ++itr){
//		if(runMode) std::cout<<"start "<<itr<<"th iteration of hough transform"<<std::endl;
		for(std::vector<std::shared_ptr<CDCHit>>::const_iterator hit = remainHits.begin(); hit != remainHits.end(); ++hit){
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
		std::shared_ptr<CDCLineCandidate> lineOdd = FindCandidate(houghOdd, 1);
		std::shared_ptr<CDCLineCandidate> lineEven = FindCandidate(houghEven, 0);

		//Save line candidate
        bool isOddGood = false, isEvenGood = false;
        IsGoodCandidate(lineOdd, lineEven, isOddGood, isEvenGood, &remainHits);
//        			std::cout<<"found good candidate at "<<itr + 1<<"th Hough transform"<<std::endl;
        if(isOddGood) lines->push_back(lineOdd);
        if(isEvenGood) lines->push_back(lineEven);
//        			std::cout<<"size of candidate container: "<<lines->size()<<std::endl;
        if((!isOddGood) && (!isEvenGood)) break;
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

std::shared_ptr<CDCLineCandidate> HoughHandler::FindCandidate(TH2D* hist, bool oddEven){
	int binX, binY, binZ;
	hist->GetMaximumBin(binX, binY, binZ);

	double theta = hist->GetXaxis()->GetBinCenter(binX);
	double rho = hist->GetYaxis()->GetBinCenter(binY);
	double maxVal = hist->GetBinContent(binX, binY);
//	std::cout<<"Peak at theta:rho "<<theta<<":"<<rho<<std::endl;

	TVector3 pos(rho * cos(theta), rho * sin(theta), 0);
	TVector3 dir(-sin(theta), cos(theta), 0);
//	std::cout<<"pos:dir "<<pos.X()<<","<<pos.Y()<<":"<<dir.X()<<","<<dir.Y()<<std::endl;

	return std::make_shared<CDCLineCandidate>(pos, dir, oddEven);
}

void HoughHandler::IsGoodCandidate(std::shared_ptr<CDCLineCandidate> lineOdd, std::shared_ptr<CDCLineCandidate> lineEven, bool& isOddGood, bool& isEvenGood, CDCHitContainer* remainHits){
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
//			else if(runMode) std::cout<<"Abort hit at ch "<<channel<<" by distance "<<CalculateDistance(posHit3, posOdd, dirOdd)<<std::endl;
		}
		else{
			if(CalculateDistance(posHit3, posEven, dirEven) < fLineDisThreshold){
                                lineEven->AddHit(*hit);
				usedAt.push(hit - remainHits->begin());
            }
//			else if(runMode) std::cout<<"Abort hit at ch"<<channel<<" by distance "<<CalculateDistance(posHit3, posOdd, dirOdd)<<std::endl;
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
	if(lineOdd->GetHits()->size() > 5) isOddGood = true;
	if(lineEven->GetHits()->size() > 5) isEvenGood = true;
}

double HoughHandler::CalculateDistance(TVector3 pos_point, TVector3 pos_line, TVector3 dir_line){
	TVector3 pointToLine = pos_line - pos_point;
	TVector3 crossProduct = pointToLine.Cross(dir_line);
//	std::cout<<"distance is "<<crossProduct.Mag() / dir_line.Mag()<<std::endl;
	return crossProduct.Mag() / dir_line.Mag();
}
