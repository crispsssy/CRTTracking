#include "EventDisplay.hxx"

using RuntimePar::f_in_path;
using RuntimePar::runNum;
using RuntimePar::startEvent;

EventDisplay::EventDisplay(){
	short adc[4992][32];
	short tdcNhit[4992];
	TFile f(f_in_path.c_str(), "READ");
	TTree* t = (TTree*)f.Get("RECBE");
	t->SetBranchAddress("adc", &adc);
	t->SetBranchAddress("tdcNhit", &tdcNhit);
	t->GetEntry(startEvent);
	for(int iCh = 0; iCh < 4992; ++iCh){
		if(tdcNhit[iCh] > 0){
			TGraph* g = new TGraph();
			g->SetName(Form("g_%d", iCh));
			g->SetTitle(Form("waveform of ch %d", iCh));
			g->SetMarkerStyle(58);
			g->SetMarkerColor(kRed);
			g->GetXaxis()->SetTitle("ADC sample index");
			g->GetYaxis()->SetTitle("ADC value");
			for(int iSample = 0; iSample < 32; ++iSample){
				g->AddPoint(iSample, adc[iCh][iSample]);
			}
			waveforms[iCh] = g;
		}
	}
}

EventDisplay::~EventDisplay(){
	for(std::vector<TCanvas*>::iterator itr = fCanvases.begin(); itr != fCanvases.end(); ++itr){
		delete (*itr);
	}
}

EventDisplay* EventDisplay::fEventDisplay = nullptr;

EventDisplay& EventDisplay::Get(){
	if(!fEventDisplay){
		fEventDisplay = new EventDisplay();
	}
	return *fEventDisplay;
}

void EventDisplay::HighlightGraph(TVirtualPad* pad, TObject* obj, Int_t ihp, Int_t y){
	auto store = gPad;
	c_waveform = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c_waveform");
	if(!c_waveform){
		std::cout<<"Make new Canvas for ADC waveform"<<std::endl;
		c_waveform = new TCanvas("c_waveform", "ADC waveform", 505, 0, 600, 400);
	}
	
	if(ihp == -1){
		delete c_waveform;
		return;
	}

	if(waveforms.size() != 0 && waveforms.find(ihp) != waveforms.end()){
		c_waveform->cd();
		waveforms.at(ihp)->Draw("APL");
		gPad->Update();
	}
	gPad = store;
}

void EventDisplay::DrawHits(CDCHitContainer* hits, int event){
	TCanvas* c = new TCanvas();
	fCanvases.push_back(c);
	c->Connect("Highlighted(TVirtualPad*, TObject*, Int_t, Int_t)", "EventDisplay", this, "HighlightGraph(TVirtualPad*, TObject*, Int_t, Int_t)");

	c->Divide(2,1);
	TGraph* hitmap_even = new TGraph();
	TGraph* hitmap_odd = new TGraph();
	for(auto hit = hits->begin(); hit != hits->end(); ++hit){
		int channel = (*hit)->GetChannelID();
		int layer = CDCGeom::Get().ChannelToLayer(channel);
		TVector2 pos = CDCGeom::Get().ChannelToROPos( (*hit)->GetChannelID() );
		if(layer % 2 == 0){
			hitmap_even->SetPoint(channel, pos.X(), pos.Y());
		}
		else hitmap_odd->SetPoint(channel, pos.X(), pos.Y());
	}
	//move unused point out of window
	for(int i=0; i<hitmap_odd->GetN(); ++i){
		double x, y = 0.;
		hitmap_odd->GetPoint(i, x, y);
		if(x == 0 && y == 0) hitmap_odd->SetPoint(i, 99999, 99999);
	}
	for(int i=0; i<hitmap_even->GetN(); ++i){
		double x, y = 0.;
		hitmap_even->GetPoint(i, x, y);
		if(x == 0 && y == 0) hitmap_even->SetPoint(i, 99999, 99999);
	}

	//odd layer
	c->cd(1);
	TH1F* frame_odd = gPad->DrawFrame(-850, -850, 850, 850);
	frame_odd->SetTitle(Form("Event display for odd layers at entry %d", event));
	frame_odd->GetXaxis()->SetTitle("X [mm]");
	frame_odd->GetYaxis()->SetTitle("Y [mm]");
	DrawCDCXY();
        hitmap_odd->SetMarkerStyle(20);
        hitmap_odd->SetMarkerSize(0.4);
        hitmap_odd->SetMarkerColor(1);
        hitmap_odd->Draw("P");
        gPad->Update();

	//even layer
	c->cd(2);
	TH1F* frame_even = gPad->DrawFrame(-850, -850, 850, 850);
	frame_even->SetTitle(Form("Event display for even layers at entry %d", event));
	frame_even->GetXaxis()->SetTitle("X [mm]");
	frame_even->GetYaxis()->SetTitle("Y [mm]");
	DrawCDCXY();
        hitmap_even->SetMarkerStyle(20);
        hitmap_even->SetMarkerSize(0.4);
        hitmap_even->SetMarkerColor(1);
        hitmap_even->Draw("P");
        gPad->Update();

	hitmap_odd->SetHighlight();
	hitmap_even->SetHighlight();

	std::cout<<"Drawn hits"<<std::endl;
}

void EventDisplay::DrawCDCXY(){
	TEllipse* cdcOuterWall = new TEllipse(0, 0, 840.);
	TEllipse* cdcInnerWall = new TEllipse(0, 0, 496.);
	cdcOuterWall->Draw();
	cdcInnerWall->Draw();
}

void EventDisplay::DrawCDCZY(){
	TLine* cdcUpperOuterWall = new TLine( -792., 850., 792., 850.);
	TLine* cdcLowerOuterWall = new TLine( -792., -850., 792., -850.);
	TLine* cdcUpperInnerWall = new TLine( -736., 496., 736., 496.);
	TLine* cdcLowerInnerWall = new TLine( -736., -496., 736., -496.);
	TLine* cdcROUpperWall = new TLine( -792., 850., -736., 496.);
	TLine* cdcROLowerWall = new TLine( -792., -850., -736., -496.);
	TLine* cdcHVUpperWall = new TLine( 792., 850., 736., 496.);
	TLine* cdcHVLowerWall = new TLine( 792., -850., 736., -496.);
	cdcUpperOuterWall->Draw();
	cdcLowerOuterWall->Draw();
	cdcUpperInnerWall->Draw();
	cdcLowerInnerWall->Draw();
	cdcROUpperWall->Draw();
	cdcROLowerWall->Draw();
	cdcHVUpperWall->Draw();
	cdcHVLowerWall->Draw();
}

void EventDisplay::DrawLineCandidates(CDCLineCandidateContainer* lines, int event){
	//Draw Hits
	CDCHitContainer totalHits;
	for(auto line = lines->begin(); line != lines->end(); ++line){
		CDCHitContainer* thisHits = (*line)->GetHits();
		if(!thisHits){
			std::cerr<<"Try to draw line candidates but no hits stored in candidates!!!"<<std::endl;
			return;
		}
		for(auto hit = thisHits->begin(); hit != thisHits->end(); ++hit){
			totalHits.push_back(*hit);
		}
	}
	DrawHits(&totalHits, event);
	TCanvas* c = fCanvases.back();
	
	//Draw lines
	for(auto line = lines->begin(); line != lines->end(); ++line){
		bool oddEven = (*line)->GetOddEven();
		if(oddEven == 1){
			c->cd(1);
		}
		else c->cd(2);
		TVector3 pos = (*line)->GetPos();
		TVector3 dir = (*line)->GetDir();
		double y1 = 850.;
		double t1 = (y1 - pos.Y()) / dir.Y();
		double x1 = pos.X() + t1 * dir.X();
		double y2 = -850.;
		double t2 = (y2 - pos.Y()) / dir.Y();
		double x2 = pos.X() + t2 * dir.X();
		TLine* l = new TLine( x1, y1, x2, y2);
		l->SetLineColor(kRed);
		l->Draw("SAME");
	}

	std::cout<<"Drawn andidates"<<std::endl;
}

void EventDisplay::DrawEventDisplay(CDCLineCandidateContainer* tracks, int event){
	//Draw CDC
	TCanvas* c = new TCanvas();
	fCanvases.push_back(c);
	c->Connect("Highlighted(TVirtualPad*, TObject*, Int_t, Int_t)", "EventDisplay", this, "HighlightGraph(TVirtualPad*, TObject*, Int_t, Int_t)");
	c->Divide(2,1);
	c->cd(1);
	TH1F* frameXY = gPad->DrawFrame(-850., -850., 850., 850.);
	frameXY->SetTitle(Form("Event display for XY plane at RO run%d entry%d", runNum, event));
	frameXY->GetXaxis()->SetTitle("X [mm]");
	frameXY->GetYaxis()->SetTitle("Y [mm]");
	DrawCDCXY();
	c->cd(2);
	TH1F* frameZY = gPad->DrawFrame(-850., -850., 850., 850.);
	frameZY->SetTitle(Form("Event display for ZY plane at X=0 run%d entry%d", runNum, event));
	frameZY->GetXaxis()->SetTitle("Z [mm]");
	frameZY->GetYaxis()->SetTitle("Y [mm]");
	DrawCDCZY();

	//Add hits to graph and create lines
	for(auto track = tracks->begin(); track != tracks->end(); ++track){
		if( !(*track) ) continue;
		TGraph* gXY = new TGraph();
		TGraph* gZY = new TGraph();
		TLine* lXY = new TLine( (*track)->GetXAtY(850.), 850., (*track)->GetXAtY(-850.), -850.);
		TLine* lZY = new TLine( (*track)->GetZAtY(850.), 850., (*track)->GetZAtY(-850.), -850.);

		for(auto hit = (*track)->GetHits()->begin(); hit != (*track)->GetHits()->end(); ++hit){
			int channel = (*hit)->GetChannelID();
			TVector2 ROPos = CDCGeom::Get().ChannelToROPos(channel);
			gXY->SetPoint(channel, ROPos.X(), ROPos.Y() );
			double residual = (*hit)->GetDriftTime(0) - CalibInfo::Get().GetTAtR((*hit)->GetDOCA());
/*			TLatex* txy = new TLatex(ROPos.X(), ROPos.Y(), Form("#splitline{ch %d}{residual %f}", channel, residual));
			txy->SetTextColor(2 + track - tracks->begin());
			txy->SetTextFont(43);
			txy->SetTextSize(10);
*/			gZY->SetPoint(channel, (*hit)->GetZ(), CDCGeom::Get().ChannelToROPos(channel).Y() );
/*			TLatex* tzy = new TLatex((*hit)->GetZ(), ROPos.Y(), Form("#splitline{ch %d}{residual %f}", channel, residual));
			tzy->SetTextColor(2 + track - tracks->begin());
			tzy->SetTextFont(43);
			tzy->SetTextSize(10);
*/
			c->cd(1);
//			txy->Draw("SAME");
			gPad->Update();
//			std::cout<<"Drawn hit info: ch:z:t_expect:residual "<<channel<<":"<<(*hit)->GetZ()<<":"<<CalibInfo::Get().GetTAtR((*hit)->GetDOCA())<<":"<<residual<<std::endl;
			c->cd(2);
//			tzy->Draw("SAME");
			gPad->Update();
		}
		for(int i=0; i<gXY->GetN(); ++i){
			double x, y = 0.;
			gXY->GetPoint(i, x, y);
			if(x == 0 && y == 0) gXY->SetPoint(i, 99999, 99999);
		}
		for(int i=0; i<gZY->GetN(); ++i){
			double x, y = 0.;
			gZY->GetPoint(i, x, y);
			if(x == 0 && y == 0) gZY->SetPoint(i, 99999, 99999);
		}

		gXY->SetMarkerStyle(20);
		gXY->SetMarkerSize(0.4);
		gXY->SetMarkerColor( 2 + track - tracks->begin() );
		lXY->SetLineColor( 2 + track - tracks->begin() );

		gZY->SetMarkerStyle(20);
		gZY->SetMarkerSize(0.4);
		gZY->SetMarkerColor( 2 + track - tracks->begin() );
		lZY->SetLineColor( 2 + track - tracks->begin() );

		c->cd(1);
		gXY->Draw("PSAME");
		lXY->Draw("SAME");
		gPad->Update();
		gXY->SetHighlight();
		c->cd(2);
		gZY->Draw("PSAME");
		lZY->Draw("SAME");
		gPad->Update();
		gZY->SetHighlight();
	}

	std::cout<<"Drawn event display"<<std::endl;
}
