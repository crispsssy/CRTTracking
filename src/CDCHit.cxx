#include "CDCHit.hxx"

void CDCHit::ls() const{
	std::cout<<"Hit at ChannelID "<<fChannelID<<std::endl;
}

void CDCHitContainer::ls() const{
	for(const_iterator v = begin(); v != end(); ++v){
		(*v)->ls();
	}
}
