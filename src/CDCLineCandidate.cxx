#include "CDCLineCandidate.hxx"

CDCLineCandidate::CDCLineCandidate(){
	fHits = new CDCHitContainer();
}

CDCLineCandidate::CDCLineCandidate(TVector3 const& pos, TVector3 const& dir, bool const oddEven){
	fPos = pos;
	fDir = dir;
	fOddEven = oddEven;
	fHits = new CDCHitContainer();
}

CDCLineCandidate::~CDCLineCandidate(){
	delete fHits;
}

double const CDCLineCandidate::GetXAtY(double y) const{
	double t = (y - fPos.Y())/fDir.Y();
	return fPos.X() + t * fDir.X();
}

double const CDCLineCandidate::GetYAtX(double x) const{
	double t = (x - fPos.X())/fDir.X();
	return fPos.Y() + t * fDir.Y();
}

double const CDCLineCandidate::GetZAtY(double y) const{
	double t = (y - fPos.Y())/fDir.Y();
	return fPos.Z() + t * fDir.Z();
}

double const CDCLineCandidate::GetYAtZ(double z) const{
	double t = (z - fPos.Z())/fDir.Z();
	return fPos.Y() + t * fDir.Y();
}
