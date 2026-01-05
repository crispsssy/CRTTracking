#include "ReFitLoop.hxx"

using RuntimePar::runMode;
using RuntimePar::itr_index;
using RuntimePar::runNum;
using RuntimePar::maxDriftTime;

void ReFitLoop(std::string const& f_in_path, std::string const& f_out_path, int const startEvent, int const numEvent){
    //Input
    itr_index = FindPar(f_in_path, "itr");
    runNum = FindPar(f_in_path, "run");
    int eventID = 0;
    std::vector<TVector3>* trkPos_in = nullptr;
    std::vector<TVector3>* trkDir_in = nullptr;
    std::vector<std::vector<int>>* channel_in = nullptr;
    std::vector<std::vector<double>>* driftTime_in = nullptr;
    std::unique_ptr<TFile> f_in{ TFile::Open(f_in_path.c_str(), "READ") };
    if(!f_in){
        std::string msg = "Can not open input file: " + f_in_path;
        throw std::runtime_error(msg);
    }
    TTree* t_in = f_in->Get<TTree>("t");
    t_in->SetBranchAddress("eventID", &eventID);
    t_in->SetBranchAddress("trkPos", &trkPos_in);
    t_in->SetBranchAddress("trkDir", &trkDir_in);
    t_in->SetBranchAddress("channel", &channel_in);
    t_in->SetBranchAddress("driftTime", &driftTime_in);

    int endEvent = startEvent + numEvent;
    if(endEvent > t_in->GetEntries()) endEvent = t_in->GetEntries();

    //Output file
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
    std::vector<std::vector<TVector2>> fPosCell;
    std::vector<std::vector<double>> fDriftTime;
    std::vector<std::vector<double>> fResidual;
    std::vector<std::vector<TVector3>> fTrkPos_excludeHit;
    std::vector<std::vector<TVector3>> fTrkDir_excludeHit;
    std::string f_name_out = f_out_path + "/recon_run" + Form("%05d", runNum) + "_maxDriftTime" + std::to_string((int)maxDriftTime) + "_start" + std::to_string(startEvent) + "_numEvent" + std::to_string(endEvent - startEvent) + "_itr" + std::to_string(itr_index + 1) + ".root";
    std::unique_ptr<TFile> f_out{ TFile::Open(f_name_out.c_str(), "RECREATE") };
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
    t_out->Branch("posCell", &fPosCell);
    t_out->Branch("driftTime", &fDriftTime);
    t_out->Branch("residual", &fResidual);
    t_out->Branch("trkPos_excludeHit", &fTrkPos_excludeHit);
    t_out->Branch("trkDir_excludeHit", &fTrkDir_excludeHit);

    //Timer
    clock_t tStart = clock();
    //Loop over events
    for(int iEvent = startEvent; iEvent < endEvent; ++iEvent){
        t_in->GetEntry(iEvent);

        //Create track and hit objects
        std::shared_ptr<CDCLineCandidateContainer> tracks = std::make_shared<CDCLineCandidateContainer>();
        for(int iTrk = 0; iTrk < trkPos_in->size(); ++iTrk){
            TVector3 const& pos = (*trkPos_in)[iTrk];
            TVector3 const& dir = (*trkDir_in)[iTrk];
            std::vector<int> const& channels = (*channel_in)[iTrk];
            std::vector<double> const& driftTimes = (*driftTime_in)[iTrk];
            std::shared_ptr<CDCLineCandidate> track = std::make_shared<CDCLineCandidate>(pos, dir);
            std::shared_ptr<CDCHitContainer> hits = std::make_shared<CDCHitContainer>();
            for(int iHit = 0; iHit < channels.size(); ++iHit){
                std::shared_ptr<CDCHit> hit = std::make_shared<CDCHit>(channels[iHit]);
                hit->InsertDriftTime(driftTimes[iHit]);
                track->AddHit(hit);
            }
            tracks->push_back(track);
        }
            
        //reFit
        TrackFitHandler::Get().ReFit(tracks);
        
        //Save new track parameters
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
        std::vector<std::vector<TVector2>> posCell;
        std::vector<std::vector<double>> driftTime;
        std::vector<std::vector<double>> residual;
        std::vector<std::vector<TVector3>> trkPos_excludeHit;
        std::vector<std::vector<TVector3>> trkDir_excludeHit;
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
            std::vector<TVector2> posCells;
            std::vector<double> driftTimes;
            std::vector<double> residuals;
            std::vector<TVector3> trkPos_residuals;
            std::vector<TVector3> trkDir_residuals;
            CDCHitContainer* hits = (*track)->GetHits();
            std::shared_ptr<CDCLineCandidateContainer> trackResiduals = (*track)->GetTrackResidual();
            if(!trackResiduals){
                std::cout<<"not residual found in event "<<iEvent<<", track No."<<track - tracks->begin()<<std::endl;
                continue;
            }
            for(auto hit = hits->begin(); hit != hits->end(); ++hit){
                channels.push_back((*hit)->GetChannelID());
                posCells.push_back((*hit)->GetPosCell());
                driftTimes.push_back((*hit)->GetDriftTime(0)); //only save the first TDC
                residuals.push_back((*hit)->GetResidual());
                trkPos_residuals.push_back(trackResiduals->at(hit - hits->begin())->GetPos());
                trkDir_residuals.push_back(trackResiduals->at(hit - hits->begin())->GetDir());
            }
            channel.push_back(std::move(channels));
            posCell.push_back(std::move(posCells));
            driftTime.push_back(std::move(driftTimes));
            residual.push_back(std::move(residuals));
            trkPos_excludeHit.push_back(std::move(trkPos_residuals));
            trkDir_excludeHit.push_back(std::move(trkDir_residuals));
        }
        fEventID = eventID;
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
        fPosCell = std::move(posCell);
        fDriftTime = std::move(driftTime);
        fResidual = std::move(residual);
        fTrkPos_excludeHit = std::move(trkPos_excludeHit);
        fTrkDir_excludeHit = std::move(trkDir_excludeHit);
        t_out->Fill();

        if( (iEvent+1) % 100 == 0) std::cout<<"Preprocessed events: "<<iEvent + 1 - startEvent<<std::endl;
    }
    std::cout<<"Finished process events, now saving to output file"<<std::endl;
    std::cout<<std::fixed<<std::setprecision(5)<<(double)(clock() - tStart)/CLOCKS_PER_SEC<<" s for looping "<<numEvent<<" events"<<std::endl;

    f_in->Close();
    f_out->cd();
    t_out->Write();
    f_out->Close();
}
