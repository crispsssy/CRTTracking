#ifndef _CDCWIREINFO_HXX_
#define _CDCWIREINFO_HXX_

#include <TVector3.h>

class CDCWireInfo{
public:
    CDCWireInfo(int wireID, int layerID, int cellID, int channelID, int boardID, int layerID_all, int localWireID_all, bool isSenseWire, double xRO, double yRO, double zRO, double xHV, double yHV, double zHV);
    ~CDCWireInfo(){}

    int GetWireID() const { return fWireID; }
    int GetLayerID() const { return fLayerID; }
    int GetCellID() const { return fCellID; }
    int GetChannelID() const { return fChannelID; }
    int GetBoardID() const { return fBoardID; }
    TVector3 const& GetPosition() const { return fPosition; }
    TVector3 const& GetROPos() const { return fROPos; }
    TVector3 const& GetHVPos() const { return fHVPos; }

private:
    int fWireID;
    int fLayerID;
    int fCellID;
    int fChannelID;
    int fBoardID;
    int fLayerID_all;
    int fLocalWireID_all;
    bool fIsSenseWire;
    double fLength;
    TVector3 fPosition;
    TVector3 fROPos;
    TVector3 fHVPos;

};

#endif
