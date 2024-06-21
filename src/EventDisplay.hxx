#ifndef _EVENTDISPLAY_HXX_
#define _EVENTDISPLAY_HXX_

#include <iostream>
#include <TROOT.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TVector2.h>
#include <TEllipse.h>
#include <TLine.h>
#include <TText.h>
#include <TLatex.h>
#include "CDCHit.hxx"
#include "CDCGeom.hxx"
#include "CDCLineCandidate.hxx"
#include "RuntimeParameter.hxx"

class EventDisplay
{
public:
	~EventDisplay();
	static EventDisplay& Get();
	void DrawHits(CDCHitContainer* hits, int event);
	void DrawCDCXY();
	void DrawCDCZY();
	void DrawLineCandidates(CDCLineCandidateContainer* lines, int event);
	void DrawEventDisplay(CDCLineCandidateContainer* tracks, int event);
	void HighlightGraph(TVirtualPad* pad, TObject* obj, Int_t ihp, Int_t y);

private:
	EventDisplay();
	EventDisplay(EventDisplay const& src);
	EventDisplay& operator=(EventDisplay const& rhs);
	static EventDisplay* fEventDisplay;

	std::vector<TCanvas*> fCanvases;
	std::vector<TGraph*> fGraphs;
	std::vector<TH1*> fHisograms;
	std::map<int, TGraph*> waveforms;
	TCanvas* c_waveform;

};

#endif
