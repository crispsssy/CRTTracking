#ifndef _PREPROCESS_HXX_
#define _PREPROCESS_HXX_

#include <iostream>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>
#include <TLine.h>
#include <TMarker.h>
#include "CDCHit.hxx"
#include "CDCGeom.hxx"
#include "RuntimeParameter.hxx"

class PreProcess{
public:
	~PreProcess();
	static PreProcess& Get();
	void DetermineT0AndPedestal(TTree* t_in);
	CDCHit* CheckHit(int const channel, std::vector<short> const& thisADC, std::vector<int> const& thisTDC);
    bool FrequencyDomainFilter(CDCHit* hit);
	void CheckNumHits(CDCHitContainer* hits);

private:
	PreProcess();
	PreProcess(PreProcess const& src){}
	PreProcess& operator=(PreProcess const& rhs);
	int fT0 = -1740;
	short fPedestal[4992] = {0};
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
