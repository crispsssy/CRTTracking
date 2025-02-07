#include "Preprocess.hxx"

PreProcess::PreProcess(){
    fRunMode = RuntimePar::runMode;
	fADCSumCut = RuntimePar::adcSumCut;
	fTDCCut = RuntimePar::tdcCut;
    fMaxDriftTime = RuntimePar::maxDriftTime;
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
	short tdcNhit[4992];
	short adc[4992][32];
	int tdcDiff[4992][32];
    int tdcDiff0[48][32];
	TH1I* h_tdc = new TH1I("h_tdc", "h_tdc", 2000, -1000, 1000);
	gDirectory->GetList()->Remove(h_tdc);
	TH1S* h_pedestal[4992];
	for(int i=0; i<4992; ++i){
		h_pedestal[i] = new TH1S(Form("h_%d", i), Form("pedestal at ch %d", i), 200, 100, 300);
	}
	t_in->SetBranchAddress("tdcNhit", &tdcNhit);
    t_in->SetBranchAddress("tdcDiff", &tdcDiff);
    t_in->SetBranchAddress("tdcDiff0", &tdcDiff0);
	t_in->SetBranchAddress("adc", &adc);

	for(int iEvent=0; iEvent<5000; ++iEvent){  //FIXME new use 5000 entries
		t_in->GetEntry(iEvent);
		for(int iCh = 0; iCh<4992; ++iCh){
			if(tdcNhit[iCh] > 0){
				h_tdc->Fill(tdcDiff[iCh][0] - tdcDiff0[0][0]);
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

	double peakX = h_tdc->GetXaxis()->GetBinCenter(h_tdc->GetMaximumBin());
	TF1* sigmoid = new TF1("sigmoid", "[0]/(1+exp(([1]-x)/[2]))");
	sigmoid->SetParameter(0, h_tdc->GetMaximum());
	sigmoid->SetParameter(1, peakX);
	sigmoid->SetParameter(2, 1.);
	h_tdc->Fit(sigmoid, "Q", "", peakX - 100, peakX + 300);

	TF1* exponential = new TF1("exponential", "[0]*exp([1]*(x-[2]))");
	exponential->SetParameter(0, sigmoid->GetParameter(0));
	exponential->SetParameter(2, sigmoid->GetParameter(1));
	h_tdc->Fit(exponential, "Q", "", peakX - 100, peakX + 300);

	TF1* f_T0 = new TF1("f_T0", "[0]+[1]*(exp([2]*(x-[3]))/(1+exp(([4]-x)/[5])))");
	f_T0->SetParameter(0,0.);
	f_T0->SetParameter(1,sigmoid->GetParameter(0));
	f_T0->SetParameter(2,exponential->GetParameter(1));
	f_T0->SetParameter(3,sigmoid->GetParameter(1));
	f_T0->SetParameter(4,sigmoid->GetParameter(1));
	f_T0->SetParameter(5,sigmoid->GetParameter(2));
	h_tdc->Fit(f_T0, "Q", "", peakX - 100, peakX + 300);
	fT0 = f_T0->GetParameter(4);

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
	c->Divide(1,2);
	c->cd(1);
	h_tdc->Draw();
	std::cout<<"histogram drawn"<<std::endl;
//	f_T0->Draw();
	TMarker* m = new TMarker(fT0, 0, 22);
	m->SetMarkerColor(kRed);
	m->SetMarkerSize(1);
	m->Draw();

	c->cd(2);
	TGraph* g_ped = new TGraph();
    g_ped->SetName("g_ped");
    g_ped->SetTitle("Pedestal values for all channels");
	for(int i=0; i<4992; ++i){
		g_ped->AddPoint(i, fPedestal[i]);
	}
	g_ped->Draw("AP");
	c->Update();
}

CDCHit* PreProcess::CheckHit(int const channel, std::vector<short> const& thisADC, std::vector<int> const& thisTDC){
	int layer = CDCGeom::Get().ChannelToLayer(channel);
	if(layer == 0 || layer == 19) return nullptr;
    
    //ADC sum cut
	if(fADCSumCut){
		short adcSum = 0;
		for(std::vector<short>::const_iterator adc = thisADC.begin(); adc != thisADC.end(); ++adc){
			if(*adc - fPedestal[channel] > 0) adcSum += *adc - fPedestal[channel];
		}
		if(adcSum < fADCSumThreshold && thisTDC.at(0) - fT0 < fADCSumCutAtTDC){
			if(fRunMode) std::cout<<"abort hit at ch "<<channel<<" with adcsum "<<adcSum<<", pedestal is "<<fPedestal[channel]<<std::endl;
			return nullptr;
		}
	}

    //Drift time cut
    if(fMaxDriftTime != 0){
        double driftTime = (thisTDC.at(0) - fT0) * 1000 / 960;
        if(driftTime > fMaxDriftTime){
            if(fRunMode) std::cout<<"Abort hit at ch "<<channel<<" with drift time "<<driftTime<<" which exceeded maxDriftTime "<<fMaxDriftTime<<std::endl;
            return nullptr;
        }
    }

	CDCHit* hit = new CDCHit(channel);

    //T0 cut
	if(fTDCCut){	
		for(std::vector<int>::const_iterator tdc = thisTDC.begin(); tdc!= thisTDC.end(); ++tdc){
			if(*tdc == 0) break;
			else if(*tdc < fT0 - 20) continue;
			else{
				int driftTDC = *tdc - fT0;
				if(driftTDC < 0) driftTDC = 0;
				double driftTime = (double)driftTDC * 1000 / 960; //TDC sampling rate 960MHz to ns
				hit->InsertDriftTime(driftTime);
			}
		}
	}
	
	if(hit->GetDriftTime().size() == 0){
        delete hit;
        hit = nullptr;
        return nullptr;
    }

    //Fill ADCs
    hit->SetADCs(thisADC);
    //Frequency domain filter and Crosstalk filter
    if(FrequencyDomainFilter(hit)){
        delete hit;
        hit = nullptr;
        if(fRunMode) std::cout<<"Abort hit at ch "<<channel<<" by frequency domain filter"<<std::endl;
    }
    else if(CrosstalkFilter(hit)){
        delete hit;
        hit = nullptr;
        if(fRunMode) std::cout<<"Abort hit at ch "<<channel<<" by crosstalk filter"<<std::endl;
    }
	return hit;
}

bool PreProcess::FrequencyDomainFilter(CDCHit* hit){
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

bool PreProcess::CrosstalkFilter(CDCHit* hit){
    bool filter = false;
    int chID = hit->GetChannelID();
    std::vector<short> adcs = hit->GetADCs();
    short adcMax = *std::max_element(adcs.begin(), adcs.end());
    short adcMin = *std::min_element(adcs.begin(), adcs.end());
    if(fabs(adcMin - fPedestal[chID]) > 0.8 * fabs(adcMax - fPedestal[chID])){
        filter = true;
    }
    return filter;
}

void PreProcess::CheckNumHits(CDCHitContainer* hits){
	if(fNumHitCut){
		if(hits->size() > fMaxNumHits || hits->size() < fMinNumHits){
//			std::cout<<hits->size()<<" hits in this event, the event will be aborted"<<std::endl;
			hits->clear();
			hits->shrink_to_fit();
		}
	}
}
