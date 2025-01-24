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
    for(int iShift = 0; iShift < XT_nshift; ++iShift){
        double shift_cm = -0.45 + iShift * 0.01;
        std::string f_XT_name = f_XT_path + "/" + Form("x2t_shift%.2f_0.00T.root", shift_cm);
        std::unique_ptr<TFile> f_XT { TFile::Open(f_XT_name.c_str(), "READ") };
        if(!f_XT){
            std::cout<<"Failed to open XT file "<<f_XT_name<<std::endl; exit(1);
        }
        std::unique_ptr<TTree> t_XT{ f_XT->Get<TTree>("t") };
        t_XT->SetBranchAddress("x0", &x0);
        t_XT->SetBranchAddress("y0", &y0);
        t_XT->SetBranchAddress("t1", &t1);

        std::shared_ptr<TGraph2D> graph_xt = std::make_shared<TGraph2D>();
        for(int iEvent = 0; iEvent < t_XT->GetEntries(); ++iEvent){
            t_XT->GetEntry(iEvent);
            graph_xt->AddPoint(x0 * 10, y0 * 10, t1); //cm to mm
        }
        graph_xt->AddPoint(0., 0., 1e-9);
        graphs_x2t[iShift] = graph_xt;
    }
    std::cout<<"Successfully read XT files"<<std::endl;

    gFile = store;
}

double const CalibInfo::GetTAtR(double r){
	return r/0.02;
}

double const CalibInfo::GetTAtXYShift(double x, double y, double shift)
{
    double shift_cm = shift / 10;
    int index = (shift_cm + 0.45) / 0.01 + 0.5;
//    std::cout<<"shift is "<<shift<<" index is "<<index<<std::endl;
    auto itr = graphs_x2t.find(index);
    if(itr == graphs_x2t.end()){
        std::cout<<"xt map out of range, maybe it's out of wire length."<<std::endl;
        return 9999. * sqrt(x*x + y*y);
    }
    double t = itr->second->Interpolate(x, y);
    if(t == 0) t = 9999. * sqrt(x*x + y*y);
    return t;
}

double const CalibInfo::GetTimeResolution(double t) const
{
	return 10.;
}

double const CalibInfo::GetTimeResolution(double const x, double const y, double const shift) const
{
    double shift_cm = shift / 10;
    int index = (shift_cm + 0.45) / 0.01 + 0.5;
//    std::cout<<"shift is "<<shift<<" index is "<<index<<std::endl;
    auto itr = graphs_x2t.find(index);
    if(itr == graphs_x2t.end()){
        std::cout<<"xt map out of range, maybe it's out of wire length."<<std::endl;
        return sqrt(x*x + y*y) * 15.;
    }
    double t = itr->second->Interpolate(x, y);
    double sigma = sqrt(x*x + y*y) * 15.;
    if(t == 0) sigma = 200.;
    return sigma;
}
