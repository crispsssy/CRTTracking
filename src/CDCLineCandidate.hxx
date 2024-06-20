#ifndef _LINECANDIDATE_HXX_
#define _LINECANDIDATE_HXX_

#include <iostream>
#include <cmath>
#include <TVector3.h>

#include "CDCHit.hxx"


class CDCLineCandidate
{
public:
	CDCLineCandidate();
        CDCLineCandidate(TVector3 const& pos, TVector3 const& dir,  bool const oddEven = 0);
//	CDCLineCandidate(CDCHitContainer* hits, double rho, double phi, double theta = 0.);
	~CDCLineCandidate();

//	void SetCandidate(double rho, double phi, double theta){ fRho = rho; fPhi = phi; fTheta = theta; }
	void AddHit(CDCHit* hit){ fHits->push_back(hit); }

	void SetPos(TVector3 pos) { fPos = pos; }
	void SetDir(TVector3 dir) { fDir = dir; }
	void SetChi2(double chi2) { fChi2 = chi2; }
	inline TVector3 const& GetPos() const { return fPos; }
	inline TVector3 const& GetDir() const { return fDir; }
	inline double const GetChi2() const { return fChi2; }
	CDCHitContainer* GetHits() const { return fHits; }
	CDCHit* GetHit(int const hit_index) const { return fHits->at(hit_index); }
	inline bool const GetOddEven() const { return fOddEven; }
	double const GetXAtY(double y) const;
	double const GetYAtX(double x) const;
	double const GetZAtY(double y) const;
	double const GetYAtZ(double z) const;
	double const GetXAtZ(double z) const;
	double const GetZAtX(double x) const;
	void GetROXYAtR(double r, double& xBottum, double& yBottum, double& xTop, double& yTop);

private:
	bool fOddEven; //Even layer for 0 and odd layer for 1
	TVector3 fPos;
	TVector3 fDir;
	CDCHitContainer* fHits = nullptr;
	double fChi2 = 99999.;
};

class CDCLineCandidateContainer : public std::vector<CDCLineCandidate*>
{
public:
	CDCLineCandidateContainer(){}

};

#endif
