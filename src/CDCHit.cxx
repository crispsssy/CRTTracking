#include "CDCHit.hxx"

bool CDCHit::IncreaseTDCIndex(){
	if(fUseTDC + 1 >= fDriftTime.size()) return false;
	else{
		++fUseTDC;
		return true;
	}
}

void CDCHit::ls() const{
	std::cout<<"Hit at ChannelID "<<fChannelID<<std::endl;
}

void CDCHitContainer::ls() const{
	for(const_iterator v = begin(); v != end(); ++v){
		(*v)->ls();
	}
}
