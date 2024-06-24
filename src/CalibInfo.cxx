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
CalibInfo* CalibInfo::fCalibInfo = nullptr;

CalibInfo& CalibInfo::Get(){
	if(!fCalibInfo){
		fCalibInfo = new CalibInfo();
	}
	return *fCalibInfo;
}
/*
void CalibInfo::ReadRTTable(){
	auto store = gFile;
	TFile* f_RT = new TFile(f_RT_path, "READ");
	func_RT = (TF1*)f_RT->Get("func_RT"); //FIXME
}
*/
double const CalibInfo::GetTAtR(double r){
	return r/0.02;
}

double const CalibInfo::GetTimeResolution(double t){
	return 10.;
}
