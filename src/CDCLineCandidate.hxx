#ifndef _LINECANDIDATE_HXX_
#define _LINECANDIDATE_HXX_

#include <iostream>
#include <memory>
#include <cmath>
#include <TVector3.h>

#include "CDCHit.hxx"

class CDCLineCandidateContainer;

class CDCLineCandidate
{
public:
	CDCLineCandidate();
    CDCLineCandidate(TVector3 const& pos, TVector3 const& dir,  bool const oddEven = 0);
//	CDCLineCandidate(std::shared_ptr<CDCHitContainer> hits, double rho, double phi, double theta = 0.);
	~CDCLineCandidate();

//	void SetCandidate(double rho, double phi, double theta){ fRho = rho; fPhi = phi; fTheta = theta; }
	void AddHit(std::shared_ptr<CDCHit> hit){ fHits->push_back(hit); }

	void SetPos(TVector3 pos) { fPos = pos; }
	void SetDir(TVector3 dir) { fDir = dir; }
	void SetChi2(double chi2) { fChi2 = chi2; }
	void SetNdf(int ndf) { fNdf = ndf; }
	void SetRhoError(double err_rho){ fErr_rho = err_rho; }
	void SetPhiError(double err_phi){ fErr_phi = err_phi; }
	void SetAlphaError(double err_alpha){ fErr_alpha = err_alpha; }
	void SetThetaError(double err_theta){ fErr_theta = err_theta; }
    void SetHits(std::shared_ptr<CDCHitContainer> hits){ fHits = hits; }
    void SetTrackResidual(std::shared_ptr<CDCLineCandidateContainer> tracks){ fTrackResidual = tracks; }
	inline TVector3 const& GetPos() const { return fPos; }
	inline TVector3 const& GetDir() const { return fDir; }
	inline double const GetChi2() const { return fChi2; }
	inline int const GetNdf() const { return fNdf; }
	inline double const GetRhoError() const { return fErr_rho; }
	inline double const GetPhiError() const { return fErr_phi; }
	inline double const GetAlphaError() const { return fErr_alpha; }
	inline double const GetThetaError() const { return fErr_theta; }
	std::shared_ptr<CDCHitContainer> GetHits() const { return fHits; }
	std::shared_ptr<CDCHit> GetHit(int const hit_index) const { return fHits->at(hit_index); }
    std::shared_ptr<CDCLineCandidateContainer> GetTrackResidual() const { return fTrackResidual; }
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
	std::shared_ptr<CDCHitContainer> fHits = nullptr;
	double fChi2 = 99999.;
	int fNdf = 0;
	double fErr_rho;
	double fErr_phi;
	double fErr_alpha;
	double fErr_theta;
    std::shared_ptr<CDCLineCandidateContainer> fTrackResidual;
};

class CDCLineCandidateContainer : public std::vector<std::shared_ptr<CDCLineCandidate>>
{
public:
	CDCLineCandidateContainer(){}

};

#endif
