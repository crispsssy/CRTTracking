#include "Preprocess.hxx"
using RuntimePar::runNum;

PreProcess::PreProcess(){
    fRunMode = RuntimePar::runMode;
	fADCSumCut = RuntimePar::adcSumCut;
	fTDCCut = RuntimePar::tdcCut;
	fNumHitCut = RuntimePar::numHitCut;
	fADCSumThreshold = RuntimePar::adcSumThreshold;
	fADCSumCutAtTDC = RuntimePar::adcSumCutAtTDC;
	fMinNumHits = RuntimePar::minNumHits;
	fMaxNumHits = RuntimePar::maxNumHits;
}

PreProcess::~PreProcess(){

}

PreProcess* PreProcess::fPreProcess = nullptr;

PreProcess& PreProcess::Get(){
	if(!fPreProcess){
		fPreProcess = new PreProcess();
	}
	return *fPreProcess;
}

void PreProcess::DetermineT0AndPedestal(TTree* t_in){
    if(ReadT0AndPedestal()) return; //if already exist

	short tdcNhit[4992];
	short adc[4992][32];
	int tdcDiff[4992][32];
    int tdcDiff0[48][32];
	TH1I* h_tdc = new TH1I("h_tdc", "h_tdc", 3000, -2000, 1000);
    int const nBoard = 104;
    TH1I* h_tdc_board[nBoard];
    for(int i=0; i<104; ++i){
        h_tdc_board[i] = new TH1I(Form("h_tdc_%d", i), Form("T0 distribution at board %d", i), 3000, -2000, 1000);
    }
	gDirectory->GetList()->Remove(h_tdc);
	TH1S* h_pedestal[4992];
	for(int i=0; i<4992; ++i){
		h_pedestal[i] = new TH1S(Form("h_%d", i), Form("pedestal at ch %d", i), 200, 100, 300);
	}
	t_in->SetBranchAddress("tdcNhit", &tdcNhit);
    t_in->SetBranchAddress("tdcDiff", &tdcDiff);
    t_in->SetBranchAddress("tdcDiff0", &tdcDiff0);
	t_in->SetBranchAddress("adc", &adc);

	for(int iEvent=0; iEvent<t_in->GetEntries(); ++iEvent){  //FIXME new use 5000 entries
		t_in->GetEntry(iEvent);
		for(int iCh = 0; iCh<4992; ++iCh){
			if(tdcNhit[iCh] > 0){
				h_tdc->Fill(tdcDiff[iCh][0] - tdcDiff0[0][0]);
                h_tdc_board[iCh/48]->Fill(tdcDiff[iCh][0] - tdcDiff0[0][0]);
//				std::cout<<"entry:iCh:tdcDiff "<<iEvent<<":"<<iCh<<":"<<tdcDiff[iCh][0]<<std::endl;
			}
			else{
				for(int iSample=0; iSample<5; ++iSample){
					if(adc[iCh][iSample] == 0) break;
					h_pedestal[iCh]->Fill(adc[iCh][iSample]);
				}
			}
		}
		if( (iEvent+1)%5000 == 0) std::cout<<"Processed events for T0 determination: "<<iEvent+1<<std::endl;
	}

    TFitResultPtr p;
    fT0.insert({-1, FitT0(h_tdc, p)}); //global t0
    for(int i=0; i<nBoard; ++i){
        double t0 = FitT0(h_tdc_board[i], p);
        if(p->Status() > 1 || fabs(t0 - fT0.at(-1)) > 10){
            t0 = fT0.at(-1);
        }
        for(int iCh=0; iCh<48; ++iCh){
            int channel = i*48 + iCh;
            fT0.insert({channel, t0});
        }
    }

	for(int iCh=0; iCh<4992; ++iCh){
		fPedestal[iCh] = h_pedestal[iCh]->GetXaxis()->GetBinCenter(h_pedestal[iCh]->GetMaximumBin());
		if(fPedestal[iCh] < 160){
			std::cout<<"ch:pedestal "<<iCh<<":"<<fPedestal[iCh]<<" abort this channel"<<std::endl;
			fPedestal[iCh] = 999;
		}
		delete h_pedestal[iCh];
	}

	TCanvas* c = new TCanvas();
	c->SetTitle("Preprocess");
	c->Divide(1,3);
	c->cd(1);
	h_tdc->Draw();
	std::cout<<"histogram drawn"<<std::endl;
	TMarker* m = new TMarker(fT0.at(-1), 0, 22);
	m->SetMarkerColor(kRed);
	m->SetMarkerSize(1);
	m->Draw();

    c->cd(2);
    TGraph* g_t0 = new TGraph();
    g_t0->SetName("g_t0");
    g_t0->SetTitle("T0 values vs boardID");
    for(auto itr = fT0.begin(); itr != fT0.end(); ++itr){
        g_t0->AddPoint(itr->first, itr->second);
    }
    g_t0->Draw("AP");
    gPad->SetGrid();

	c->cd(3);
	TGraph* g_ped = new TGraph();
    g_ped->SetName("g_ped");
    g_ped->SetTitle("Pedestal values for all channels");
	for(int i=0; i<4992; ++i){
		g_ped->AddPoint(i, fPedestal[i]);
	}
	g_ped->Draw("AP");
	c->Update();

    //save T0 distribution plots in output root file
    TFile* f_t0 = new TFile(Form("t0_pedestal/T0Pedestal_run0%d.root", runNum), "RECREATE");
    double t0 = 0;
    short pedestal = 0;
    TTree* t = new TTree("t", "t");
    t->Branch("t0", &t0);
    t->Branch("pedestal", &pedestal);
    for(int i=0; i<4992; ++i){
        t0 = fT0.at(i);
        pedestal = fPedestal[i];
        t->Fill();
    }
    t->Write();
    f_t0->Close();

    //release memory
    for(int i=0; i<nBoard; ++i){
        delete h_tdc_board[i];
        h_tdc_board[i] = nullptr;
    }
}

bool PreProcess::ReadT0AndPedestal(){
    double t0;
    short pedestal;    
    TFile* f_t0 = TFile::Open(Form("t0_pedestal/T0Pedestal_run0%d.root", runNum), "READ");
    if(!f_t0){
        std::cout<<"Couldn't find t0/pedestal data, start to determine T0 and Pedestal valus"<<std::endl;
        return false;
    }
    TTree* t = (TTree*)f_t0->Get("t");
    t->SetBranchAddress("t0", &t0);
    t->SetBranchAddress("pedestal", &pedestal);
    for(int i=0; i<t->GetEntries(); ++i){
        t->GetEntry(i);
        fPedestal[i] = pedestal;
        fT0.insert({i, t0});
    }
    f_t0->Close();
    std::cout<<"Found prestored t0/pedestal data, skip T0 and pedestal determination"<<std::endl;
    return true;
}

double PreProcess::FitT0(TH1I* h_tdc, TFitResultPtr& p){
    p = nullptr;
    double peakX = h_tdc->GetXaxis()->GetBinCenter(h_tdc->GetMaximumBin());
    double peakY = h_tdc->GetMaximum();
    TF1* sigmoid = new TF1("sigmoid", "[0]/(1+exp(([1]-x)/[2]))");
    sigmoid->SetParameter(0, peakY);
    sigmoid->SetParameter(1, peakX);
    sigmoid->SetParameter(2, 1.);
    sigmoid->SetLineColor(kBlue);
    h_tdc->Fit(sigmoid, "QS", "", peakX - 100, peakX + 100);

    double xMax = peakX + 300;
    double y_xMax = h_tdc->GetBinContent(h_tdc->FindBin(xMax));
    TF1* exponential = new TF1("exponential", "[0]*exp([1]*(x-[2]))");
    exponential->SetParameter(0, sigmoid->GetParameter(0));
    exponential->SetParameter(1, log(y_xMax/peakY) / (xMax - peakX));
    exponential->SetParameter(2, sigmoid->GetParameter(1));
    exponential->SetLineColor(kMagenta);
    h_tdc->Fit(exponential, "QS+", "", peakX, peakX + 300);

    TF1* f_T0 = new TF1("f_T0", "[0]+[1]*(exp([2]*(x-[3]))/(1+exp(([4]-x)/[5])))");
    f_T0->SetParameter(0,0.);
    f_T0->SetParameter(1,sigmoid->GetParameter(0));
    f_T0->SetParameter(2,exponential->GetParameter(1));
    f_T0->SetParameter(3,sigmoid->GetParameter(1));
    f_T0->SetParameter(4,sigmoid->GetParameter(1));
    f_T0->SetParameter(5,sigmoid->GetParameter(2));
    p = h_tdc->Fit(f_T0, "QS+", "", peakX - 100, peakX + 300);
    return f_T0->GetParameter(4);
}

double PreProcess::GetT0(int const channel){
    auto itr = fT0.find(channel);
    if(itr == fT0.end()){
        std::cout<<"PreProcess::GetT0: channel "<<channel<<" out of range"<<std::endl;
        exit(1);
    }
    return itr->second;
}

double PreProcess::GetPedestal(int const channel){
    if(channel < 4992) return fPedestal[channel];
    else return 999;
}

std::shared_ptr<CDCHit> PreProcess::CheckHit(int const channel, std::vector<short> const& thisADC, std::vector<int> const& thisTDC){
	int layer = CDCGeom::Get().ChannelToLayer(channel);
	if(layer == 0 || layer == 19) return nullptr;

    //Get T0
    double T0 = GetT0(channel);
    
    //ADC sum cut
	if(fADCSumCut){
		short adcSum = 0;
		for(std::vector<short>::const_iterator adc = thisADC.begin(); adc != thisADC.end(); ++adc){
			if(*adc - fPedestal[channel] > 0) adcSum += *adc - fPedestal[channel];
		}
		if(adcSum < fADCSumThreshold && thisTDC.at(0) - T0 < fADCSumCutAtTDC){
			if(fRunMode) std::cout<<"abort hit at ch "<<channel<<" with adcsum "<<adcSum<<", pedestal is "<<fPedestal[channel]<<std::endl;
			return nullptr;
		}
	}

    std::shared_ptr<CDCHit> hit = std::make_shared<CDCHit>(channel);

    //T0 cut
	if(fTDCCut){	
		for(std::vector<int>::const_iterator tdc = thisTDC.begin(); tdc!= thisTDC.end(); ++tdc){
			if(*tdc == 0) break;
			else if(*tdc < T0 - 20) continue;
			else{
				int driftTDC = *tdc - T0;
				double driftTime = (double)driftTDC * 1000 / 960; //TDC sampling rate 960MHz to ns
				hit->InsertDriftTime(driftTime);
			}
		}
	}
	
	if(hit->GetDriftTime().size() == 0){
        return nullptr;
    }

    //Fill ADCs
    hit->SetADCs(thisADC);
    //Frequency domain filter and Crosstalk filter
    if(FrequencyDomainFilter(hit)){
        if(fRunMode) std::cout<<"Abort hit at ch "<<channel<<" by frequency domain filter"<<std::endl;
        return nullptr;
    }
    else if(CrosstalkFilter(hit)){
        if(fRunMode) std::cout<<"Abort hit at ch "<<channel<<" by crosstalk filter"<<std::endl;
        return nullptr;
    }
	return hit;
}

bool PreProcess::FrequencyDomainFilter(std::shared_ptr<CDCHit> hit){
    bool filter = false;
    int chID = hit->GetChannelID();
    std::vector<short> adcs = hit->GetADCs();
    TH1S h_waveform("h_waveform", "h_waveform", 33, -0.5, 32.5);
    TH1* h_freq = nullptr;
    for(auto adc = adcs.begin(); adc != adcs.end(); ++adc){
        h_waveform.Fill(adc - adcs.begin(), *adc);
    }
    h_freq = h_waveform.FFT(h_freq, "MAG");
    //Get rid of DC component
    h_freq->SetBinContent(1, 0);
    //Get rid of negative component
    h_freq->GetXaxis()->SetRange(2, 16);
    //judge if peak fall in filter range
    double peak = h_freq->GetBinCenter(h_freq->GetMaximumBin());
    if(peak > 11 && peak < 13){
//        std::cout<<"channel "<<hit->GetChannelID()<<" found peak "<<peak<<" at frequency domain, filtered."<<std::endl;
        filter = true;
    }
    delete h_freq;
    h_freq = nullptr;
    return filter;
}

bool PreProcess::CrosstalkFilter(std::shared_ptr<CDCHit> hit){
    bool filter = false;
    int chID = hit->GetChannelID();
    std::vector<short> adcs = hit->GetADCs();
    short adcMax = *std::max_element(adcs.begin(), adcs.end());
    short adcMin = *std::min_element(adcs.begin(), adcs.end());
    if(fabs(adcMin - fPedestal[chID]) > 0.3 * fabs(adcMax - fPedestal[chID])){
        filter = true;
    }
    return filter;
}

void PreProcess::CheckNumHits(std::shared_ptr<CDCHitContainer> hits){
	if(fNumHitCut){
		if(hits->size() > fMaxNumHits || hits->size() < fMinNumHits){
//			std::cout<<hits->size()<<" hits in this event, the event will be aborted"<<std::endl;
			hits->clear();
			hits->shrink_to_fit();
		}
	}
}
