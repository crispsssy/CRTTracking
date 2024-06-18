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
