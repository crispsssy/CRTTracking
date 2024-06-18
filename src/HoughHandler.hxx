#ifndef _HOUGHHANDLER_HXX_
#define _HOUGHHANDLER_HXX_

#include <iostream>
#include <queue>
#include <TH1.h>
#include <TH2.h>
#include <TVector2.h>
#include <TVector3.h>
#include <TCanvas.h>
#include "CDCHit.hxx"
#include "CDCGeom.hxx"
#include "CDCLineCandidate.hxx"
#include "RuntimeParameter.hxx"

class HoughHandler
{
public:
	~HoughHandler();
	static HoughHandler& Get();

	CDCLineCandidateContainer* FindCandidates(CDCHitContainer* hits);
	double CalculateDistance(TVector3 pos_point, TVector3 pos_line, TVector3 dir_line);

private:
	HoughHandler(){}
	HoughHandler(HoughHandler const& src);
	HoughHandler& operator=(HoughHandler const& rhs);

	CDCLineCandidate* FindCandidate(TH2D* h_hough, bool oddEven);
	bool IsGoodCandidate(CDCLineCandidate*& lineOdd, CDCLineCandidate*& lineEven, CDCHitContainer* remainHits);

	static HoughHandler* fHoughHandler;

	double fLineDisThreshold = 20; //hits that inside 20mm range around line will be collected
};

#endif
