#include "EventDisplay.hxx"

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

void EventDisplay::DrawHits(CDCHitContainer* hits, int event){
	TCanvas* c = new TCanvas();
	fCanvases.push_back(c);

	c->Divide(2,1);
	TGraph* hitmap_even = new TGraph();
	TGraph* hitmap_odd = new TGraph();
	for(auto hit = hits->begin(); hit != hits->end(); ++hit){
		int layer = CDCGeom::Get().ChannelToLayer( (*hit)->GetChannelID() );
		TVector2 pos = CDCGeom::Get().ChannelToROPos( (*hit)->GetChannelID() );
		if(layer % 2 == 0){
			hitmap_even->AddPoint(pos.X(), pos.Y());
		}
		else hitmap_odd->AddPoint(pos.X(), pos.Y());
	}

	//odd layer
	c->cd(1);
	TH1F* frame_odd = gPad->DrawFrame(-850, -850, 850, 850);
	frame_odd->SetTitle(Form("Event display for odd layers at entry %d", event));
	DrawCDC();
	hitmap_odd->GetXaxis()->SetTitle("X [mm]");
	hitmap_odd->GetYaxis()->SetTitle("Y [mm]");
        hitmap_odd->SetMarkerStyle(20);
        hitmap_odd->SetMarkerSize(0.4);
        hitmap_odd->SetMarkerColor(1);
        hitmap_odd->Draw("P");
        gPad->Update();

	//even layer
	c->cd(2);
	TH1F* frame_even = gPad->DrawFrame(-850, -850, 850, 850);
	frame_even->SetTitle(Form("Event display for even layers at entry %d", event));
	DrawCDC();
	hitmap_even->GetXaxis()->SetTitle("X [mm]");
	hitmap_even->GetYaxis()->SetTitle("Y [mm]");
        hitmap_even->SetMarkerStyle(20);
        hitmap_even->SetMarkerSize(0.4);
        hitmap_even->SetMarkerColor(1);
        hitmap_even->Draw("P");
        gPad->Update();

	std::cout<<"Drawn hits"<<std::endl;
}

void EventDisplay::DrawCDC(){
	TEllipse* cdcOuterWall = new TEllipse(0, 0, 840.);
	TEllipse* cdcInnerWall = new TEllipse(0, 0, 496.);
	cdcOuterWall->Draw();
	cdcInnerWall->Draw();
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

