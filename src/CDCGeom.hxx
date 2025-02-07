#ifndef _CDCGeom_HXX_
#define _CDCGeom_HXX_

#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TVector2.h>
#include "CDCLineCandidate.hxx"
#include "CDCWireInfo.hxx"

class CDCGeom
{
public:
	~CDCGeom();
	static CDCGeom& Get();

	void ReadChMap();
    int const ChannelToWire(int channel) const { return fChannelToWireMap.at(channel); }
    int const WireToChannel(int wire) const { return fWireToChannelMap.at(wire); }
	int const ChannelToLayer(int channel) const { return layerMap[channel]; }
	int const ChannelToCell(int channel) const { return cellMap[channel]; }
    int const WireToLayerID_all(int wireID) const { return fLayerWireMap.at(wireID).first; }
    int const WireToLocalWireID_all(int wireID) const { return fLayerWireMap.at(wireID).second; }
	double const ChannelToMaximumX(int channel) const { return XRO[channel]; }
	double const ChannelToMinimumX(int channel) const { return XHV[channel]; }
	double const ChannelToMaximumY(int channel) const { return YRO[channel]; }
	double const ChannelToMinimumY(int channel) const { return YHV[channel]; }
	double const ChannelToMaximumZ(int channel) const { return ZRO[channel]; }
	double const ChannelToMinimumZ(int channel) const { return ZHV[channel]; }
	TVector2 const ChannelToROPos(int channel) const { return TVector2(XRO[channel], YRO[channel]); }
	TVector2 const ChannelToHVPos(int channel) const { return TVector2(XHV[channel], YHV[channel]); }
	TVector2 const ChannelToZ0Pos(int channel) const { return TVector2( (XRO[channel]+XHV[channel])/2, (YRO[channel]+YHV[channel])/2 ); }
	TVector3 const ChannelToWireDir(int channel) const { return TVector3(XRO[channel]-XHV[channel], YRO[channel]-YHV[channel], ZRO[channel]-ZHV[channel]).Unit();}
	double const GetCDCLength() const { return CDCLength; }
	double const ChannelYToZ(int channel, double y);
	double const ChannelZToY(int channel, double z);
	TVector2 const GetWireTrackIntersectionZY(std::shared_ptr<CDCLineCandidate> track, int channel);
	double const GetStereoAngle(int channel);
	void GetPOCA(TVector3 const& trkPos, TVector3 const& trkDir, int channel, TVector3& pocaT, TVector3& pocaW);
	void GetPOCA(TVector3 const& trkPos, TVector3 const& trkDir, TVector3 const& wirePos, TVector3 const& wireDir, TVector3& pocaT, TVector3& pocaW);
	double const GetDOCA(TVector3 const& trkPos, TVector3 const& trkDir, int channel);

    int const GetWireID(int layerID, int cellID) const;
    int GetNumberOfWiresPerLayer(int layerID) const {
        return GetMaxNumWiresOnLayerID_all(layerID*2) / 2;
    }
    inline int GetMaxNumWiresOnLayerID_all(int const layerID_all) const {
        if(layerID_all == 0) return 396;
        else return (layerID_all - 1) / 2 * 12 + 396;
    }
    bool IsInsideDeadTriangle(TVector3 const& local, int const wireIDClock, int const wireIDCounterClock) const;
    bool IsInsidePolygon(std::vector<TVector2> const& vertexs, TVector2 const& point) const;
    void GetAdjacentWires(int layerID_all, int localWireID_all, int& localWireID_all_clock, int& localWireID_all_counterClock) const;
    int const FindCloestOuterFieldWire(int layerID_all, int localWireID_all, double z) const;
    TVector2 LocalPositionToCellPositionXY(TVector3 const& localPos, int const channel) const;

    double GetCellShift(double const z, int const channel) const;
    TVector2 GetWirePositionXY(int const wire, double const z) const;
    TVector2 GetFieldWirePositionXY(int const layerID_all, int const localWireID_all, double const z) const;

private:
	CDCGeom();
	CDCGeom(const CDCGeom& src);
	CDCGeom& operator=(const CDCGeom& rhs);

    CDCWireInfo const& GetWireInfo(int layerID_all, int localWireID_all) const;
    CDCWireInfo const& GetWireInfo(int wire) const;
	int layerMap[4992] = {-1};
	int cellMap[4992] = {-1};
	double XRO[4992] = {0};
	double YRO[4992] = {0};
	double ZRO[4992] = {0};
	double XHV[4992] = {0};
	double YHV[4992] = {0};
	double ZHV[4992] = {0};
	double CDCLength = 1600.;

	static CDCGeom* fCDCGeom;
	std::string fMapPath = "/Users/siyuan/Physics/comet/crt/tracking/ch_map/ch_map.root";

    std::map<std::pair<int,int>, CDCWireInfo> fWireInfoMap;
    std::map<int, int> fChannelToWireMap;
    std::map<int, int> fWireToChannelMap;
    std::map<int, std::pair<int, int>> fLayerWireMap; //wireID to layerID_all, localWireID_all
};

#endif
