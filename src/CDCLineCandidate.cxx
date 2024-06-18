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
