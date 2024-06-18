#include "CDCGeom.hxx"

CDCGeom::CDCGeom(){
	ReadChMap();
}

CDCGeom::~CDCGeom(){

}

CDCGeom* CDCGeom::fCDCGeom = nullptr;

CDCGeom& CDCGeom::Get(){
	if(!fCDCGeom){
		if(fCDCGeom) delete fCDCGeom;
		fCDCGeom = new CDCGeom();
	}
	return *fCDCGeom;
}

void CDCGeom::ReadChMap(){
	int BoardID, ChanID, LayerID, CellID;
	double xRO, yRO, zRO, xHV, yHV, zHV;
	TFile f_map(fMapPath.c_str(), "READ");
	TTree* t_map = (TTree*)f_map.Get("t");
	t_map->SetBranchAddress("xRO", &xRO);
        t_map->SetBranchAddress("yRO", &yRO);
        t_map->SetBranchAddress("zRO", &zRO);
        t_map->SetBranchAddress("xHV", &xHV);
        t_map->SetBranchAddress("yHV", &yHV);
        t_map->SetBranchAddress("zHV", &zHV);
        t_map->SetBranchAddress("BoardID", &BoardID);
        t_map->SetBranchAddress("ChanID", &ChanID);
        t_map->SetBranchAddress("LayerID", &LayerID);
        t_map->SetBranchAddress("CellID", &CellID);

	for(int i=0; i<t_map->GetEntries(); ++i){
		t_map->GetEntry(i);
		if(BoardID > -1){
			int channel = BoardID * 48 + ChanID;
			layerMap[channel] = LayerID;
			cellMap[channel] = CellID;
			XRO[channel] = xRO;
			YRO[channel] = yRO;
			ZRO[channel] = zRO;
			XHV[channel] = xHV;
			YHV[channel] = yHV;
			ZHV[channel] = zHV;
		}
	}

	f_map.Close();
	std::cout<<"CDC geometry info has been loaded"<<std::endl;
}

double const CDCGeom::ChannelYToZ(int channel, double y){
	double dy = YRO[channel] - YHV[channel];
	double dz = ZRO[channel] - ZHV[channel];
	double t = (y - YHV[channel]) / dy;
	return ZHV[channel] + t * dz;
}

double const CDCGeom::ChannelZToY(int channel, double z){
	double dz = ZRO[channel] - ZHV[channel];
	double dy = YRO[channel] - YHV[channel];
	double t = (z - ZHV[channel]) / dz;
	return YHV[channel] + t * dy;
}

TVector2 const CDCGeom::GetWireTrackIntersectionZY(CDCLineCandidate* track, int channel){
	double wire_dydz = (YRO[channel] - YHV[channel]) / (ZRO[channel] - ZHV[channel]);
	double wire_yAtz0 = ChannelZToY(channel, 0.);
	double track_dydz = track->GetDir().Y() / track->GetDir().Z();
	double track_yAtz0 = track->GetYAtZ(0.);
	double z = -(wire_yAtz0 - track_yAtz0)/(wire_dydz - track_dydz);
	double y = wire_dydz * z + wire_yAtz0;
	return TVector2(z, y);
}
