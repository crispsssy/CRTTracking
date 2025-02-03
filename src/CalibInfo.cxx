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
        fGraphs_x2t_mean[iShift] = graph_xt_mean;
        fGraphs_x2t_std[iShift] = graph_xt_std;
    }
    std::cout<<"Successfully read XT files"<<std::endl;
    GenerateSimpleXT();

    gFile = store;
}

void CalibInfo::GenerateSimpleXT(){
    //Use shift = 2.3 mm and phi~ = 22.5 degrees as the simple XT and sigma_t.
    //Extrapolate XT from doca = 9.4 mm and extrapolate sigma_t from doca = 7.4 mm
    fSimpleXTGraph = new TGraph();
    fSimpleResoGraph = new TGraph();
    double const shift = 2.3;
    double const angle = 0.393; //22.5 degrees
    fSimpleXTFunc = new TF1("fSimpleXT", "[0] + [1] * x", (fMaxDocaIndexXT-1) * fdDoca, 1e3);
    fSimpleResoFunc = new TF1("fSimpleReso", "[0] + [1] * x", (fMaxDocaIndexReso-1) * fdDoca, 1e3);
    for(int iDoca = 0; iDoca < fMaxDocaIndexXT+1; ++iDoca){
        double doca = iDoca * fdDoca;
        fSimpleXTGraph->AddPoint(doca, GetTAtXYShift(doca * cos(angle), doca * sin(angle), shift));
        if(iDoca < fMaxDocaIndexReso+1){
            fSimpleResoGraph->AddPoint(doca, GetTimeResolution(doca * cos(angle), doca * sin(angle), shift));
        }
    }
    //Set parameters of linear function
    double x1_xt = (fMaxDocaIndexXT-1) * fdDoca;
    double x2_xt = fMaxDocaIndexXT * fdDoca;
    double y1_xt = fSimpleXTGraph->Eval(x1_xt);
    double y2_xt = fSimpleXTGraph->Eval(x2_xt);
    fSimpleXTFunc->SetParameter(1, (y2_xt - y1_xt) / (x2_xt - x1_xt));
    fSimpleXTFunc->SetParameter(0, y1_xt - fSimpleXTFunc->GetParameter(1) * x1_xt);
    double x1_reso = (fMaxDocaIndexReso-1) * fdDoca;
    double x2_reso = fMaxDocaIndexReso * fdDoca;
    double y1_reso = fSimpleResoGraph->Eval(x1_reso);
    double y2_reso = fSimpleResoGraph->Eval(x2_reso);
    fSimpleResoFunc->SetParameter(1, (y2_reso - y1_reso) / (x2_reso - x1_reso));
    fSimpleResoFunc->SetParameter(0, y1_reso - fSimpleResoFunc->GetParameter(1) * x1_reso);
    TFile f("graph.root", "RECREATE");
    fSimpleXTGraph->SetName("fSimpleXTGraph");
    fSimpleResoGraph->SetName("fSimpleResoGraph");
    fSimpleXTGraph->Write();
    fSimpleXTFunc->Write();
    fSimpleResoGraph->Write();
    fSimpleResoFunc->Write();
}


double const CalibInfo::GetTAtR(double r){
	static const double maxDoca = fMaxDocaIndexXT * fdDoca;
    if(r < maxDoca) return fSimpleXTGraph->Eval(r);
    else return fSimpleXTFunc->Eval(r);
}

double const CalibInfo::GetTAtXYShift(double x, double y, double shift)
{
    double r = sqrt(x*x + y*y);
    int index = (shift + 4.5) / 0.1 + 0.5; //0.5 for rounding
//    std::cout<<"shift is "<<shift<<" index is "<<index<<std::endl;
    auto itr = fGraphs_x2t_mean.find(index);
    if(itr == fGraphs_x2t_mean.end()){
        std::cout<<"xt map out of range, maybe it's out of wire length."<<std::endl;
        return r / 14.14 * 5000.;
    }
    if(x > 15 || x < -15 || y > 10 || y < -10){
        return r / 14.14 * 5000.;
    }
    double t = itr->second->Interpolate(x, y);
    return t;
}

double const CalibInfo::GetTimeResolution(double r) const
{
	static const double maxDoca = fMaxDocaIndexReso * fdDoca;
    if(r < maxDoca) return fSimpleResoGraph->Eval(r);
    else return fSimpleResoFunc->Eval(r);
}

double const CalibInfo::GetTimeResolution(double const x, double const y, double const shift) const
{
    int index = (shift + 4.5) / 0.1 + 0.5; //0.5 for rounding
    auto itr = fGraphs_x2t_std.find(index);
    if(itr == fGraphs_x2t_std.end()){
        return 100.;
    }
    if(x > 15 || x < -15 || y > 10 || y < -10){
        return 100.;
    }
    double sigma = itr->second->Interpolate(x, y);
    return sigma;
}
