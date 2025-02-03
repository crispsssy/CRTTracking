#include "EventLoop.hxx"

using RuntimePar::runMode;
using RuntimePar::runNum;
using RuntimePar::XTMode;

void EventLoop(std::string f_in_path, std::string f_out_path, int startEvent, int numEvent){
//	gInterpreter->GenerateDictionary("vector<TVector3>", "vector;TVector3.h");
//	gInterpreter->GenerateDictionary("vector<vector<int>>", "vector");

	//Efficiency histograms
	TH1D* h_preprocess = new TH1D("h_preprocess", "preprocess efficiency", 100, 0, 1);
	TH1D* h_hough = new TH1D("h_hough", "Hough transform efficiency", 100, 0, 1);
	TH1D* h_phiDiff = new TH1D("h_angleDiff", "angle diff", 256, -6.4, 6.4);
	TH1D* h_dis = new TH1D("h_dis", "distance", 200, 0, 1000);

	short tdcNhit[4992];
	short adc[4992][32];
	int tdcDiff[4992][32];
    int tdcDiff0[48][32];
	TFile* f_in = new TFile(f_in_path.c_str(), "READ");
	TTree* t_in = (TTree*)f_in->Get("RECBE");
	if(numEvent - startEvent > t_in->GetEntries()) numEvent = t_in->GetEntries();

	PreProcess::Get().DetermineT0AndPedestal(t_in);

	t_in->SetBranchAddress("tdcNhit", &tdcNhit);
	t_in->SetBranchAddress("tdcDiff", &tdcDiff);
	t_in->SetBranchAddress("tdcDiff0", &tdcDiff0);
	t_in->SetBranchAddress("adc", &adc);

	//output file config
	int fEventID = 0;
	std::vector<TVector3> fTrkPos;
	std::vector<TVector3> fTrkDir;
	std::vector<double> fChi2;
	std::vector<double> fNdf;
	std::vector<double> fProb;
	std::vector<double> fErr_rho;
	std::vector<double> fErr_phi;
	std::vector<double> fErr_alpha;
	std::vector<double> fErr_theta;
    std::vector<std::vector<int>> fChannel;
	std::string f_name_out = f_out_path + "/recon_run" + Form("%05d", runNum) + "_" + XTMode + ".root";
	TFile* f_out = new TFile(f_name_out.c_str(), "RECREATE");
	TTree* t_out = new TTree("t", "t");

	t_out->Branch("eventID", &fEventID);
	t_out->Branch("trkPos", &fTrkPos);
	t_out->Branch("trkDir", &fTrkDir);
	t_out->Branch("chi2", &fChi2);
	t_out->Branch("ndf", &fNdf);
	t_out->Branch("prob", &fProb);
	t_out->Branch("err_rho", &fErr_rho);
	t_out->Branch("err_phi", &fErr_phi);
	t_out->Branch("err_alpha", &fErr_alpha);
	t_out->Branch("err_theta", &fErr_theta);
    t_out->Branch("channel", &fChannel);

	//Preprocess of hits
	for(int iEvent = startEvent; iEvent < startEvent+numEvent; ++iEvent){
		t_in->GetEntry(iEvent);
//		std::cout<<"Start process event "<<iEvent<<std::endl;

		//Pre processing and save hit information
		int numRawHits = 0;
		CDCHitContainer* rawHits = new CDCHitContainer();
		CDCHitContainer* hits = new CDCHitContainer();
		for(int iCh = 0; iCh < 4992; ++iCh){
			if(tdcNhit[iCh] > 0){
				++numRawHits;
				std::vector<short> thisADC;
				std::vector<int> thisTDC;
				for(int iSample = 0; iSample < 32; ++iSample){
					thisADC.push_back(adc[iCh][iSample]);
					thisTDC.push_back(tdcDiff[iCh][iSample] - tdcDiff0[0][0]);
				}

				CDCHit* rawHit = new CDCHit(iCh);
				rawHits->push_back(rawHit);

				CDCHit* hit = PreProcess::Get().CheckHit(iCh, thisADC, thisTDC);
				if(!hit) continue;
				hits->push_back( hit );
			}
		}
		PreProcess::Get().CheckNumHits(hits);
		
		double efficiency_pre = (double)hits->size() / numRawHits;
		h_preprocess->Fill(efficiency_pre);
		if(hits->empty()) continue;
/*
		//For Preprocess debug
		std::cout<<"Preprocess debug part"<<std::endl;
		if(efficiency_pre < 0.7) std::cout<<"entry:Preprocess efficiency "<<iEvent<<":"<<efficiency_pre<<std::endl;
		EventDisplay::Get().DrawHits(rawHits, iEvent);
		EventDisplay::Get().DrawHits(hits, iEvent);
		std::cout<<"Preprocess finished for entry "<<iEvent<<std::endl;
*/

		//Hough transform
		CDCLineCandidateContainer* lines = HoughHandler::Get().FindCandidates(hits);
		if(!lines){
//			std::cout<<"no candidate found after Hough transform at entry "<<iEvent<<", abort this entry"<<std::endl;
			continue;
		}
//		std::cout<<"Found "<<lines->size()<<" candidates"<<std::endl;
		int remainHits_hough = 0;
		for(std::vector<CDCLineCandidate*>::const_iterator line = lines->begin(); line != lines->end(); ++line){
			remainHits_hough += (*line)->GetHits()->size();
		}
		double efficiency_hough = (double)remainHits_hough / numRawHits;
		h_hough->Fill(efficiency_hough);

		//For Hough transform debug
        if(runMode == 1){
            std::cout<<"Hough transform debug part"<<std::endl;
            if(efficiency_hough < 0.3) std::cout<<"entry:hough efficiency "<<iEvent<<":"<<efficiency_hough<<std::endl;
            EventDisplay::Get().DrawLineCandidates(lines, iEvent);
            std::cout<<"Hough transform finished for entry "<<iEvent<<std::endl;
        }

/*		//debug for angle diff, distance between lines
		for(auto lineOdd = lines->begin(); lineOdd != lines->end(); ++lineOdd){
			//pick a odd layer candidate
			if( (*lineOdd)->GetOddEven() == 1 ){
				for(auto lineEven = lines->begin(); lineEven != lines->end(); ++lineEven){
					if( (*lineEven)->GetOddEven() == 0 ){
						double dis = TVector3((*lineOdd)->GetPos() - (*lineEven)->GetPos()).Mag();
						double phiDiff = (*lineOdd)->GetDir().Phi() - (*lineEven)->GetDir().Phi();
						if(dis > 336) continue;
						if(phiDiff < -TMath::Pi()+0.5) phiDiff += TMath::Pi();
						else if(phiDiff > TMath::Pi()+0.5) phiDiff -= TMath::Pi();
						h_phiDiff->Fill(phiDiff);
						h_dis->Fill(dis);
					}
				}
			}
		}
*/	


		//3D Fitting
		CDCLineCandidateContainer* tracks = TrackFitHandler::Get().Find3DTracks(lines);
		if(!tracks){
			continue;
		}

		//Save track to output file
		std::vector<TVector3> trksPos;
		std::vector<TVector3> trksDir;
		std::vector<double> chi2;
		std::vector<double> ndf;
		std::vector<double> prob;
		std::vector<double> err_rho;
		std::vector<double> err_phi;
		std::vector<double> err_alpha;
		std::vector<double> err_theta;
        std::vector<std::vector<int>> channel;
		for(auto track = tracks->begin(); track != tracks->end(); ++track){
			if(!(*track)) continue; //TODO should remove this line after implementation
			trksPos.push_back( (*track)->GetPos() );
			trksDir.push_back( (*track)->GetDir() );
			chi2.push_back( (*track)->GetChi2() );
			ndf.push_back( (*track)->GetNdf() );
			prob.push_back( TMath::Prob( (*track)->GetChi2(), (*track)->GetNdf() ) );
			err_rho.push_back( (*track)->GetRhoError() );
			err_phi.push_back( (*track)->GetPhiError() );
			err_alpha.push_back( (*track)->GetAlphaError() );
			err_theta.push_back( (*track)->GetThetaError() );
            std::vector<int> channels;
            for(auto hit = (*track)->GetHits()->begin(); hit != (*track)->GetHits()->end(); ++hit){
                channels.push_back((*hit)->GetChannelID());
            }
            channel.push_back(std::move(channels));
		}
		fEventID = iEvent;
		fTrkPos = std::move(trksPos);
		fTrkDir = std::move(trksDir);
		fChi2 = std::move(chi2);
		fNdf = std::move(ndf);
		fProb = std::move(prob);
		fErr_rho = std::move(err_rho);
		fErr_phi = std::move(err_phi);
		fErr_alpha = std::move(err_alpha);
		fErr_theta = std::move(err_theta);
        fChannel = std::move(channel);
		t_out->Fill();

		//Track fitting debug part
        if(runMode == 1){
            std::cout<<"Track fitting debug part"<<std::endl;
            if(tracks->size() > 2) std::cout<<"entry:numTrack "<<iEvent<<":"<<tracks->size()<<std::endl;
            EventDisplay::Get().DrawEventDisplay(tracks, iEvent);
            std::cout<<"3D fitting finished for entry "<<iEvent<<std::endl;
        }


		//release memory
		for(auto track = tracks->begin(); track != tracks->end(); ++track){
			if(!(*track)) continue;
			for(auto hit = (*track)->GetHits()->begin(); hit != (*track)->GetHits()->end(); ++hit){
				delete (*hit);
			}
			delete (*track);
		}
		for(auto line = lines->begin(); line != lines->end(); ++line){
			delete *line;
		}
		for(auto hit = rawHits->begin(); hit != rawHits->end(); ++hit){
			delete *hit;
		}

		if( (iEvent+1) % 1000 == 0) std::cout<<"Preprocessed events: "<<iEvent + 1 - startEvent<<std::endl;
	}
	std::cout<<"Finished process events, now saving to output file"<<std::endl;

	f_in->Close();
	f_out->cd();
	t_out->Write();
	f_out->Close();

	TCanvas* c = new TCanvas();
	h_preprocess->Draw();
	TCanvas* c1 = new TCanvas();
	h_hough->Draw();

	TCanvas* c2 = new TCanvas();
	c2->Divide(2,1);
	c2->cd(1);
	h_phiDiff->Draw();
	c2->cd(2);
	h_dis->Draw();

}
