#ifndef _CDCGeom_HXX_
#define _CDCGeom_HXX_

#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TVector2.h>
#include "CDCLineCandidate.hxx"

class CDCGeom
{
public:
	~CDCGeom();
	static CDCGeom& Get();

	void ReadChMap();
	int const ChannelToLayer(int channel) const { return layerMap[channel]; }
	int const ChannelToCell(int channel) const { return cellMap[channel]; }
	int const ChannelToMaximumX(int channel) const { return XRO[channel]; }
	int const ChannelToMinimumX(int channel) const { return XHV[channel]; }
	int const ChannelToMaximumY(int channel) const { return YRO[channel]; }
	int const ChannelToMinimumY(int channel) const { return YHV[channel]; }
	int const ChannelToMaximumZ(int channel) const { return ZRO[channel]; }
	int const ChannelToMinimumZ(int channel) const { return ZHV[channel]; }
	TVector2 const ChannelToROPos(int channel) const { return TVector2(XRO[channel], YRO[channel]); }
	TVector2 const ChannelToHVPos(int channel) const { return TVector2(XHV[channel], YHV[channel]); }
	TVector2 const ChannelToZ0Pos(int channel) const { return TVector2( (XRO[channel]+XHV[channel])/2, (YRO[channel]+YHV[channel])/2 ); }
	TVector3 const ChannelToWireDir(int channel) const { return TVector3(XRO[channel]-XHV[channel], YRO[channel]-YHV[channel], ZRO[channel]-ZHV[channel]).Unit();}
	double const GetCDCLength() const { return CDCLength; }
	double const ChannelYToZ(int channel, double y);
	double const ChannelZToY(int channel, double z);
	TVector2 const GetWireTrackIntersectionZY(CDCLineCandidate* track, int channel);
	double const GetStereoAngle(int channel);
	void GetPOCA(TVector3 const& trkPos, TVector3 const& trkDir, int channel, TVector3& pocaT, TVector3& pocaW);
	void GetPOCA(TVector3 const& trkPos, TVector3 const& trkDir, TVector3 const& wirePos, TVector3 const& wireDir, TVector3& pocaT, TVector3& pocaW);
	double const GetDOCA(TVector3 const& trkPos, TVector3 const& trkDir, int channel);

private:
	CDCGeom();
	CDCGeom(const CDCGeom& src);
	CDCGeom& operator=(const CDCGeom& rhs);
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
};

#endif
