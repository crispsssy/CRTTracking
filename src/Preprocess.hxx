#ifndef _PREPROCESS_HXX_
#define _PREPROCESS_HXX_

#include <iostream>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TLine.h>
#include <TMarker.h>
#include <TFitResult.h>
#include "CDCHit.hxx"
#include "CDCGeom.hxx"
#include "RuntimeParameter.hxx"

class PreProcess{
public:
	~PreProcess();
	static PreProcess& Get();
	void DetermineT0AndPedestal(TTree* t_in);
    bool ReadT0AndPedestal();
    double FitT0(TH1I* h_tdc, TFitResultPtr& p);
    double GetT0(int const channel);
    double GetPedestal(int const channel);
	std::shared_ptr<CDCHit> CheckHit(int const channel, std::vector<short> const& thisADC, std::vector<int> const& thisTDC);
    bool FrequencyDomainFilter(std::shared_ptr<CDCHit> hit);
    bool CrosstalkFilter(std::shared_ptr<CDCHit> hit);
	void CheckNumHits(CDCHitContainer* hits);

private:
	PreProcess();
	PreProcess(PreProcess const& src){}
	PreProcess& operator=(PreProcess const& rhs);
    std::map<int, double> fT0;
	short fPedestal[4992] = {0};
    bool fRunMode = true;
	bool fADCSumCut = true;
	bool fTDCCut = true;
	bool fNumHitCut = true;
	int fADCSumThreshold = 10;
	int fADCSumCutAtTDC = 400;
	int fMaxNumHits = 320;
	int fMinNumHits = 12;
    double fMaxDriftTime = 0;

	static PreProcess* fPreProcess;
};

#endif
