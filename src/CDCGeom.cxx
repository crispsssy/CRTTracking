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
	int layerID_all, localWireID_all, BoardID, ChanID, LayerID, CellID;
	double xRO, yRO, zRO, xHV, yHV, zHV;
	TFile f_map(fMapPath.c_str(), "READ");
	TTree* t_map = (TTree*)f_map.Get("t");
    t_map->SetBranchAddress("layer", &layerID_all);
    t_map->SetBranchAddress("wire", &localWireID_all);
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
        int channel = -1;
        int wireID = -1;
        bool isSenseWire = false;
		if(BoardID > -1){
			channel = BoardID * 48 + ChanID;
            wireID = GetWireID(LayerID, CellID);
            isSenseWire = true;
			layerMap[channel] = LayerID;
			cellMap[channel] = CellID;
			XRO[channel] = xRO;
			YRO[channel] = yRO;
			ZRO[channel] = zRO;
			XHV[channel] = xHV;
			YHV[channel] = yHV;
			ZHV[channel] = zHV;
            fChannelToWireMap[channel] = wireID;
            fWireToChannelMap[wireID] = channel;
            fLayerWireMap[wireID] = std::pair<int, int>(layerID_all, localWireID_all);
		}
        CDCWireInfo wireInfo(wireID, LayerID, CellID, channel, BoardID, layerID_all, localWireID_all, isSenseWire, xRO, yRO, zRO, xHV, yHV, zHV);
        fWireInfoMap.insert(
            std::pair<std::pair<int,int>, CDCWireInfo>(
                std::pair<int, int>(layerID_all, localWireID_all), std::move(wireInfo)
            )
        );
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

TVector2 const CDCGeom::GetWireTrackIntersectionZY(std::shared_ptr<CDCLineCandidate> track, int channel){
	double wire_dydz = (YRO[channel] - YHV[channel]) / (ZRO[channel] - ZHV[channel]);
	double wire_yAtz0 = ChannelZToY(channel, 0.);
	double track_dydz = track->GetDir().Y() / track->GetDir().Z();
	double track_yAtz0 = track->GetYAtZ(0.);
	double z = -(wire_yAtz0 - track_yAtz0)/(wire_dydz - track_dydz);
	double y = wire_dydz * z + wire_yAtz0;
	return TVector2(z, y);
}

double const CDCGeom::GetStereoAngle(int channel){
	return atan( sqrt(pow(XRO[channel] - XHV[channel], 2) + pow(YRO[channel] - YHV[channel], 2)) / (ZRO[channel] - ZHV[channel]) );
}

void CDCGeom::GetPOCA(TVector3 const& trkPos, TVector3 const& trkDir, int channel, TVector3& pocaT, TVector3& pocaW){
	TVector2 wireZ0Pos = ChannelToZ0Pos(channel);
	TVector3 wirePos(wireZ0Pos.X(), wireZ0Pos.Y(), 0.);
	TVector3 wireDir = ChannelToWireDir(channel);
	GetPOCA(trkPos, trkDir, wirePos, wireDir, pocaT, pocaW);
}

void CDCGeom::GetPOCA(TVector3 const& trkPos, TVector3 const& trkDir, TVector3 const& wirePos, TVector3 const& wireDir, TVector3& pocaT, TVector3& pocaW){
	//normal vector of track and wire
	TVector3 n = wireDir.Cross(trkDir).Unit();
	//normal vector of plane contains track and n
	TVector3 n1 = trkDir.Cross(n).Unit();
	//normal vector of plane contains wire and n
	TVector3 n2 = wireDir.Cross(n).Unit();

	pocaT = trkPos + (wirePos - trkPos).Dot(n2) / trkDir.Dot(n2) * trkDir;
	pocaW = wirePos + (trkPos - wirePos).Dot(n1) / wireDir.Dot(n1) * wireDir;
//	std::cout<<"trkPos:trkDir "<<trkPos.X()<<","<<trkPos.Y()<<","<<trkPos.Z()<<" : "<<trkDir.X()<<","<<trkDir.Y()<<","<<trkDir.Z()<<std::endl;
//	std::cout<<"wirPos:wireDir "<<wirePos.X()<<","<<wirePos.Y()<<","<<wirePos.Z()<<" : "<<wireDir.X()<<","<<wireDir.Y()<<","<<wireDir.Z()<<std::endl; 
//	std::cout<<"pocaT:pocaW "<<pocaT.X()<<","<<pocaT.Y()<<","<<pocaT.Z()<<" : "<<pocaW.X()<<","<<pocaW.Y()<<","<<pocaW.Z()<<std::endl;
}

double const CDCGeom::GetDOCA(TVector3 const& trkPos, TVector3 const& trkDir, int channel){
	TVector3 pocaT;
	TVector3 pocaW;
	GetPOCA(trkPos, trkDir, channel, pocaT, pocaW);
	return (pocaW - pocaT).Mag();
}

int const CDCGeom::GetWireID(int layerID, int cellID) const
{
    int wireID = 0;
    for(int i = 0; i < layerID; ++i){
        wireID += GetNumberOfWiresPerLayer(i);
    }
    wireID += cellID;
    return wireID;
}

bool CDCGeom::IsInsideDeadTriangle(TVector3 const& local, int const wireIDClock, int const wireIDCounterClock) const
{
    //Define dead triangle as
    //  o     o     B     A     o     o
    //
    //   o      *      C      *      o
    //
    //   o      o      o      o      o
    //Notice that this is rare case, most of cases that A and B are the same wire
    double z = local.Z();
    int layerID_all = WireToLayerID_all(wireIDClock);
    if(layerID_all == 0 || layerID_all == 38) return false; //outer most guard layer
    int localWireID_all_clock = WireToLocalWireID_all(wireIDClock);
    int localWireID_all_counterClock = WireToLocalWireID_all(wireIDCounterClock);
    int localWireID_all_C;
    int tmp;
    GetAdjacentWires(layerID_all, localWireID_all_clock, tmp, localWireID_all_C);
    int localWireID_all_clockOuter = FindCloestOuterFieldWire(layerID_all, localWireID_all_clock, z);
    int localWireID_all_counterClockOuter = FindCloestOuterFieldWire(layerID_all, localWireID_all_counterClock, z);
    int localWireID_all_A;
    int localWireID_all_B;
    GetAdjacentWires(layerID_all + 1, localWireID_all_clockOuter, tmp, localWireID_all_A);
    GetAdjacentWires(layerID_all + 1, localWireID_all_counterClockOuter, localWireID_all_B, tmp);
    if(localWireID_all_A == localWireID_all_B) return false;
    else{
        TVector2 localxy(local.X(), local.Y());
        TVector2 posA = GetFieldWirePositionXY(layerID_all + 1, localWireID_all_A, z);
        TVector2 posB = GetFieldWirePositionXY(layerID_all + 1, localWireID_all_B, z);
        TVector2 posC = GetFieldWirePositionXY(layerID_all,     localWireID_all_C, z);
        std::vector<TVector2> pos;
        pos.push_back(std::move(posA));
        pos.push_back(std::move(posB));
        pos.push_back(std::move(posC));
        if(IsInsidePolygon(pos, localxy)) return true;
        else return false;
    }
}

bool CDCGeom::IsInsidePolygon(std::vector<TVector2> const& vertexs, TVector2 const& point) const
{
    int intersection = 0;
    //loop over all vertexs of polygon
    //calculate if there is intersection between segment and a ray start from the point to x+
    for(auto itr = vertexs.begin(); itr != vertexs.end(); ++itr){
        TVector2 v1 = *itr;
        TVector2 v2;
        TVector2 p(point);
        if(itr == vertexs.end() - 1) v2 = *(vertexs.begin());
        else v2 = *(itr + 1);

        if(v1.Y() > v2.Y()) std::swap(v1, v2);

        //We add a perturbation here to avoid the ray just pass the vertex
        while(p.Y() == v1.Y() || p.Y() == v2.Y()) p.SetY(point.Y() + 1e-100);

        if(p.Y() > v1.Y() && p.Y() < v2.Y() && p.X() <= std::max(v1.X(), v2.X())){
            double cross = (v1.X() - p.X()) * (v2.Y() - p.Y()) - (v1.Y() - p.Y()) * (v2.X() - p.X());
            if(cross >= 0) ++intersection;
        }
    }
    //Odd intersections --- inside
    //Even intersections --- outside
    return (intersection % 2);
}

void CDCGeom::GetAdjacentWires(int layerID_all, int localWireID_all, int& localWireID_all_clock, int& localWireID_all_counterClock) const
{
    int maxWireID_all = GetMaxNumWiresOnLayerID_all(layerID_all) - 1;
    if(localWireID_all == 0){
        localWireID_all_clock = maxWireID_all;
        localWireID_all_counterClock = localWireID_all + 1;
    }
    else if(localWireID_all == maxWireID_all){
        localWireID_all_clock = localWireID_all -1;
        localWireID_all_counterClock = 0;
    }
    else{
        localWireID_all_clock = localWireID_all -1;
        localWireID_all_counterClock = localWireID_all + 1;
    }
}

int const CDCGeom::FindCloestOuterFieldWire(int layerID_all, int localWireID_all, double z) const
{
    TVector2 pos = GetFieldWirePositionXY(layerID_all, localWireID_all, z);
    double phi = atan2(pos.Y(), pos.X());
    TVector2 pos_outer0 = GetFieldWirePositionXY(layerID_all + 1, 0, z);
    double phi0 = atan2(pos_outer0.Y(), pos_outer0.X());
    double dphi = TMath::Pi() * 2 / GetMaxNumWiresOnLayerID_all(layerID_all + 1);
    if(phi - phi0 < 0) phi += TMath::Pi() * 2;
    int localWireID_all_outer = (phi - phi0) / dphi + 0.5; //0.5 for rounding
    if(localWireID_all_outer == GetMaxNumWiresOnLayerID_all(layerID_all + 1)) localWireID_all_outer = 0;
//    std::cout<<"%"<<"FindCloestOuterFieldWire() z:phi0:dphi:phi "<<z<<":"<<phi0<<":"<<dphi<<":"<<phi<<std::endl;
    return localWireID_all_outer;
}

TVector2 CDCGeom::LocalPositionToCellPositionXY(TVector3 const& localPos, int const channel) const
{
    int wireID = ChannelToWire(channel);
    TVector2 pos_sense = GetWirePositionXY(wireID, localPos.Z());
    double rotateAngle = TVector2(0, 1).DeltaPhi(pos_sense);
    TVector2 senseToPos = TVector2(localPos.X(), localPos.Y()) - pos_sense;
    TVector2 pos_cell = senseToPos.Rotate(rotateAngle);
//    std::cout<<"%"<<"LocalPositionToCellPositionXY() localPos:wireID ("<<localPos.X()<<","<<localPos.Y()<<","<<localPos.Z()<<") : "<<wireID<<std::endl;
    return pos_cell;
}

double CDCGeom::GetCellShift(double const z, int const channel) const
{
    //Define field wires as:
    // D    C    B      outer layer
    //
    //  E    *    A     clockwise side from read out side of CDC
    //
    //  F    G    H     inner layer
    int wireID = ChannelToWire(channel);
    int layerID = ChannelToLayer(channel);
    int cellID = ChannelToCell(channel);
    if(layerID == 0 || layerID == 19) return 0;
    int layerID_all = WireToLayerID_all(wireID);
    int localWireID_all = WireToLocalWireID_all(wireID);
    int localWireID_all_C = FindCloestOuterFieldWire(layerID_all, localWireID_all, z);
    TVector2 posC = GetFieldWirePositionXY(layerID_all + 1, localWireID_all_C, z);
//    std::cout<<"GetCellShift() pass channelID to LocalPositionToCellPositionXY: "<<channel<<std::endl;
    posC = LocalPositionToCellPositionXY(TVector3(posC.X(), posC.Y(), z), channel);
/*    std::cout<<"channelID:wireID "<<channel<<":"<<wireID<<std::endl;
    std::cout<<"layerID_all:localWireID_all:localWireID_all_C "<<layerID_all<<":"<<localWireID_all<<":"<<localWireID_all_C<<std::endl
             <<"pos_C ("<<posC.X()<<","<<posC.Y()<<") z = "<<z<<std::endl;
*/             
    return posC.X();
}

TVector2 CDCGeom::GetWirePositionXY(int const wire, double const z) const
{
    CDCWireInfo const wireInfo = GetWireInfo(wire);
    TVector3 posRO = wireInfo.GetROPos();
    TVector3 posHV = wireInfo.GetHVPos();

    double efficiency = fabs(z - posHV.Z())/fabs(posRO.Z() - posHV.Z());
    double x = efficiency * (posRO.X() - posHV.X()) + posHV.X();
    double y = efficiency * (posRO.Y() - posHV.Y()) + posHV.Y();
    return TVector2(x, y);
}

TVector2 CDCGeom::GetFieldWirePositionXY(int const layerID_all, int const localWireID_all, double const z) const
{
    CDCWireInfo const wireInfo = GetWireInfo(layerID_all, localWireID_all);
    TVector3 posRO = wireInfo.GetROPos();
    TVector3 posHV = wireInfo.GetHVPos();

    double efficiency = fabs(z - posHV.Z())/fabs(posRO.Z() - posHV.Z());
    double x = efficiency * (posRO.X() - posHV.X()) + posHV.X();
    double y = efficiency * (posRO.Y() - posHV.Y()) + posHV.Y();
    return TVector2(x, y);
}

CDCWireInfo const& CDCGeom::GetWireInfo(int layerID_all, int localWireID_all) const
{
    std::map<std::pair<int, int>, CDCWireInfo>::const_iterator itr;
    itr = fWireInfoMap.find(std::pair<int, int>(layerID_all, localWireID_all));
    if(itr == fWireInfoMap.end()){ std::cout<<"GetWireInfo() can't find wireInfo"<<std::endl; exit(1); }
    else return itr->second;
}

CDCWireInfo const& CDCGeom::GetWireInfo(int wire) const
{
    std::pair<int, int> layerWire = fLayerWireMap.find(wire)->second;
    std::map<std::pair<int, int>, CDCWireInfo>::const_iterator itr = fWireInfoMap.find(layerWire);
    if(itr == fWireInfoMap.end()){ std::cout<<"GetWireInfo() can't find wireInfo"<<std::endl; exit(1); }
    else return itr->second;
}
