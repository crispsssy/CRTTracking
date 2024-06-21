#include "CalibInfo.hxx"

using RuntimePar::XTMode;
/*
CalibInfo::CalibInfo(){
	if(XTMode == "RT"){
		ReadRTTable();
	}
	else if(XTMode == "RZT"){
		ReadRZTTable();
	}
	else if(XTMode == "XYZT"){
		ReadXYZTTable();
	}
}
*/
CalibInfo* CalibInfo::fCalibInfo = new CalibInfo();

CalibInfo& CalibInfo::Get(){
	if(!fCalibInfo){
		fCalibInfo = new CalibInfo();
	}
	return *fCalibInfo;
}

double const CalibInfo::GetRAtT(double t){
	return 0.02 * t;
}

double const CalibInfo::GetSpatialResolution(double r){
	return 0.2;
}
