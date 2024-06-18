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
	inline TVector3 const& GetPos() const { return fPos; }
	inline TVector3 const& GetDir() const { return fDir; }
	CDCHitContainer* GetHits() const { return fHits; }
	CDCHit* GetHit(int const hit_index) const { return fHits->at(hit_index); }
	inline bool const GetOddEven() const { return fOddEven; }
	double const GetXAtY(double y) const;
	double const GetYAtX(double x) const;
	double const GetZAtY(double y) const;
	double const GetYAtZ(double z) const;

private:
	bool fOddEven; //Even layer for 0 and odd layer for 1
	TVector3 fPos;
	TVector3 fDir;
	CDCHitContainer* fHits = nullptr;
};

class CDCLineCandidateContainer : public std::vector<CDCLineCandidate*>
{
public:
	CDCLineCandidateContainer(){}

};

#endif
