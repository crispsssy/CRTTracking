#include "CDCWireInfo.hxx"

CDCWireInfo::CDCWireInfo(int wireID, int layerID, int cellID, int channelID, int boardID, int layerID_all, int localWireID_all, bool isSenseWire, double xRO, double yRO, double zRO, double xHV, double yHV, double zHV){
    fWireID = wireID;
    fLayerID = layerID;
    fCellID = cellID;
    fChannelID = channelID;
    fBoardID = boardID;
    fLayerID_all = layerID_all;
    fLocalWireID_all = localWireID_all;
    fIsSenseWire = isSenseWire;
    fPosition = TVector3((xRO - xHV)/2, (yRO - yHV)/2, (zRO - zHV)/2);
    fROPos = TVector3(xRO, yRO, zRO);
    fHVPos = TVector3(xHV, yHV, zHV);
    fLength = sqrt(pow(xRO - xHV, 2) + pow(yRO - yHV, 2) + pow(zRO - zHV, 2));
}
