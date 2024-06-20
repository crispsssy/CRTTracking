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

double const CDCLineCandidate::GetXAtZ(double z) const{
	double t = (z - fPos.Z())/fDir.Z();
	return fPos.X() + t * fDir.X();
}

double const CDCLineCandidate::GetZAtX(double x) const{
	double t = (x - fPos.X())/fDir.X();
	return fPos.Z() + t * fDir.Z();
}

void CDCLineCandidate::GetROXYAtR(double r, double& xBottum, double& yBottum, double& xTop, double& yTop){
	double r0 = sqrt( pow(fPos.X(), 2) + pow(fPos.Y(), 2) );
	double theta0 = atan2(fPos.Y(), fPos.X());
	double theta = acos(r0/r);
	
	double x1 = r * cos(theta0 + theta);
	double y1 = r * sin(theta0 + theta);
	double x2 = r * cos(theta0 - theta);
	double y2 = r * sin(theta0 - theta);
	std::cout<<"r0 : theta0   : theta   : x1   : y1   : x2   : y2 "<<std::endl
		 <<r0<<" : "<<theta0<<" : "<<theta<<" : "<<x1<<" : "<<y1<<" : "<<x2<<" : "<<y2<<std::endl;

	if(y2 > y1){
		xBottum = x1;
		yBottum = y1;
		xTop = x2;
		yTop = y2;
	}
	else{
		xBottum = x2;
		yBottum = y2;
		xTop = x1;
		yTop = y1;
	}
}
