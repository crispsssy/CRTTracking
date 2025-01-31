#include "CalibInfo.hxx"

using RuntimePar::XTMode;
using RuntimePar::XT_reso_x;
using RuntimePar::XT_reso_y;
using RuntimePar::XT_nshift;

CalibInfo* CalibInfo::fCalibInfo = nullptr;

CalibInfo& CalibInfo::Get(){
	if(!fCalibInfo){
		fCalibInfo = new CalibInfo();
	}
	return *fCalibInfo;
}

CalibInfo::CalibInfo()
{
    ReadXTTable();
}


void CalibInfo::ReadXTTable(){
    std::cout<<"Start reading XT files"<<std::endl;
	auto store = gFile;

    double x0, y0, t1;
#ifdef XT_PATH
    std::string f_XT_path(XT_PATH);
#else
    std::cout<<"XT file path is not defined"<<std::endl;
    std::string f_XT_path;
#endif
    std::string f_XT_name = f_XT_path + "/x2t_0.00T_1800V.root";
    std::unique_ptr<TFile> f_XT { TFile::Open(f_XT_name.c_str(), "READ") };
    if(!f_XT){
        std::cout<<"Failed to open XT file "<<f_XT_name<<std::endl; exit(1);
    }
    for(int iShift = 0; iShift < XT_nshift; ++iShift){
        double shift = -4.5 + iShift * 0.1;

        TGraph2D* graph_mean = (TGraph2D*)f_XT->Get(Form("driftTime_mean_%.2f", shift));
        TGraph2D* graph_std = (TGraph2D*)f_XT->Get(Form("driftTime_std_%.2f", shift));
        if(!graph_mean || !graph_std){
            std::cerr<<"Graph can not found: "<<Form("driftTime_mean_%.2f", shift)<<std::endl;
            exit(1);
        }
        std::shared_ptr<TGraph2D> graph_xt_mean = std::make_shared<TGraph2D>(*graph_mean);
        std::shared_ptr<TGraph2D> graph_xt_std = std::make_shared<TGraph2D>(*graph_std);
        graph_xt_mean->SetDirectory(0);
        graph_xt_std->SetDirectory(0);
        graphs_x2t_mean[iShift] = graph_xt_mean;
        graphs_x2t_std[iShift] = graph_xt_std;
    }
    std::cout<<"Successfully read XT files"<<std::endl;

    gFile = store;
}

double const CalibInfo::GetTAtR(double r){
	return r/0.02;
}

double const CalibInfo::GetTAtXYShift(double x, double y, double shift)
{
    double r = sqrt(x*x + y*y);
    int index = (shift + 4.5) / 0.1 + 0.5; //0.5 for rounding
//    std::cout<<"shift is "<<shift<<" index is "<<index<<std::endl;
    auto itr = graphs_x2t_mean.find(index);
    if(itr == graphs_x2t_mean.end()){
        std::cout<<"xt map out of range, maybe it's out of wire length."<<std::endl;
        return r / 14.14 * 5000.;
    }
    if(x > 15 || x < -15 || y > 10 || y < -10){
        return r / 14.14 * 5000.;
    }
    double t = itr->second->Interpolate(x, y);
    return t;
}

double const CalibInfo::GetTimeResolution(double t) const
{
	return 10.;
}

double const CalibInfo::GetTimeResolution(double const x, double const y, double const shift) const
{
    int index = (shift + 4.5) / 0.1 + 0.5; //0.5 for rounding
    auto itr = graphs_x2t_std.find(index);
    if(itr == graphs_x2t_std.end()){
        return 100.;
    }
    if(x > 15 || x < -15 || y > 10 || y < -10){
        return 100.;
    }
    double sigma = itr->second->Interpolate(x, y);
    return sigma;
}
