#include <iostream>
#include <filesystem>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH2.h>
#include <TF1.h>
#include <TApplication.h>
#include "CalibInfo.hxx"
#include "Utilities.hxx"

int main(int argc, char** argv){
    std::string filePath = argv[1];
    std::string outDir = argv[2];
    int const runNum = FindPar(filePath, "run");
    int const itr_index = FindPar(filePath, "itr") + 1;
//    int itr_index = FindPar(filePath, "itr");
    TApplication app("app", nullptr, nullptr);
    TCanvas* c = new TCanvas();
    TGraph* g_XTCorrection = new TGraph();
    TGraphErrors* g_timeResolutionGaus = new TGraphErrors();
    TGraphErrors* g_timeResolution68 = new TGraphErrors();
    g_timeResolutionGaus->SetName("g_timeResolutionGaus");
    g_timeResolution68->SetName("g_timeResolution68");
    std::unique_ptr<TFile> f_in{ TFile::Open(filePath.c_str(), "READ") };
    if(!f_in){
        std::string msg = "Can not open input file: " + filePath;
        throw std::runtime_error(msg);
    }
    TTree* t_in = f_in->Get<TTree>("t");
    
    //Draw histograms of residual
    double maxDOCA = 5.5;
    double dDOCA = 0.5;
    int nDOCA = maxDOCA / dDOCA;
    t_in->Draw(Form("residual:sqrt(posCell.X()*posCell.X()+posCell.Y()*posCell.Y())>>h_res(%d, 0, %f, 100, -50, 50)", nDOCA, maxDOCA), "chi2/ndf<5", "COLZ");
    TH2D* h_res = (TH2D*)gDirectory->Get("h_res");
    if(!h_res){
        std::string msg = "Can not retrieve residual histogram";
        throw std::runtime_error(msg);
    }
    h_res->GetXaxis()->SetTitle("DOCA [mm]");
    h_res->GetYaxis()->SetTitle("Time residual [ns]");
    h_res->SetTitle("Time Residual vs. DOCA");
    h_res->SetStats(0);
    h_res->Draw("COLZ");
    h_res->SetDirectory(0);
    f_in->Close();

    //Get MPV at each DOCA and fit with gaussian
    g_XTCorrection->AddPoint(0, 0); //Always do not correct at origin because drift time should be 0 here.
    for(int ibin = 1; ibin <= h_res->GetNbinsX(); ++ibin){
        double doca = h_res->GetXaxis()->GetBinCenter(ibin);
        std::unique_ptr<TH1> h_py{ h_res->ProjectionY("_py", ibin, ibin) };
        int maxBin = h_py->GetMaximumBin();
        double mpv = h_py->GetBinCenter(maxBin);

        //Fit by gauss
        h_py->Fit("gaus", "Q0", "", mpv - 20, mpv + 20);
        TF1* f_gaus = h_py->GetFunction("gaus");
        double mu = f_gaus->GetParameter(1);
        double sigma = f_gaus->GetParameter(2);

        //Calculate 68% interval
        double xp[2] = {0};
        double p[2] = {0.16, 0.84};
        h_py->GetQuantiles(2, xp, p);
        double sigma68 = (xp[1] - xp[0]) / 2;
//        std::cout<<"xp[1] : xp[0] : sigma68 = "<<xp[1]<<":"<<xp[0]<<":"<<sigma68<<std::endl;

        //Fill in graphs
        if(doca > 1.5) g_XTCorrection->AddPoint(doca, mu);
        g_timeResolutionGaus->AddPoint(doca, sigma);
        g_timeResolutionGaus->SetPointError(ibin - 1, 0.25, 0);
        g_timeResolution68->AddPoint(doca, sigma68);
        g_timeResolution68->SetPointError(ibin - 1, 0.25, 0);
    }
    g_XTCorrection->SetMarkerStyle(29);
    g_XTCorrection->SetMarkerColor(kRed);
    g_XTCorrection->Draw("PSAME");
    TF1* f_correction = new TF1("f", "[0] * x");
    g_XTCorrection->Fit(f_correction);
    gPad->Update();

    TCanvas* c_reso = new TCanvas();
    TH1F* frame = c_reso->DrawFrame(0, 0, 5.5, 15);
    frame->SetTitle("Time Resolution vs. DOCA");
    frame->GetXaxis()->SetTitle("DOCA [mm]");
    frame->GetYaxis()->SetTitle("Time Resolution [ns]");
    g_timeResolutionGaus->SetMarkerStyle(20);
//    g_timeResolutionGaus->SetMarkerSize(0.4);
    g_timeResolutionGaus->SetMarkerColor(kRed);
    g_timeResolutionGaus->SetTitle("Gaus fit near MPV");
    g_timeResolutionGaus->Draw("PSAME");
    g_timeResolution68->SetMarkerStyle(20);
//    g_timeResolution68->SetMarkerSize(0.4);
    g_timeResolution68->SetTitle("68%% interval");
    g_timeResolution68->Draw("PSAME");
    c_reso->BuildLegend();
    gPad->SetGrid();
    gPad->Update();

    //Update XT to output file
    std::string outPath = outDir + "/xt_run" + Form("%05d", runNum) + "/itr" + std::to_string(itr_index);
    std::filesystem::create_directories(outPath);
    std::string filePath_out = outPath + "/xt.root";
    std::unique_ptr<TFile> f_out{ TFile::Open(filePath_out.c_str(), "NEW") };
    if(!f_out){
        std::string msg = "Can not open output XT file: " + filePath_out;
        throw std::runtime_error(msg);
    }
    TGraph* g_xt_old = new TGraph();
    TGraph* g_xt_new = new TGraph();
    g_xt_new->SetName("g_XTSimple");
    double extrapolateDOCA = maxDOCA - dDOCA / 2;
    double resoCorrection = g_timeResolution68->Eval(extrapolateDOCA) - CalibInfo::Get().GetTimeResolution(extrapolateDOCA);
    for(int i = 0; i < 100; ++i){
        double doca = i * 0.1;
        double t_old = CalibInfo::Get().GetTAtR(doca);
        g_xt_old->AddPoint(doca, t_old);
        g_xt_new->AddPoint(doca, t_old + f_correction->Eval(doca));
        if(doca > extrapolateDOCA) g_timeResolution68->AddPoint(doca, CalibInfo::Get().GetTimeResolution(doca) + resoCorrection);
    }

    TCanvas* c_xt = new TCanvas();
    TH1F* frame_xt = c_xt->DrawFrame(0, 0, 8, 800);
    frame_xt->GetXaxis()->SetTitle("DOCA [mm]");
    frame_xt->GetYaxis()->SetTitle("Drift Time [ns]");
    frame_xt->SetTitle("XT");
    g_xt_old->SetTitle("Old XT");
    g_xt_old->SetMarkerStyle(20);
    g_xt_old->Draw("PSAME");
    g_xt_new->SetMarkerStyle(20);
    g_xt_new->SetMarkerColor(kRed);
    g_xt_new->SetTitle("New XT");
    g_xt_new->Draw("PSAME");
    c_xt->BuildLegend();
    gPad->SetGrid();
    gPad->Update();

    f_out->cd();
    g_xt_new->Write();
    g_timeResolutionGaus->Write();
    g_timeResolution68->Write();
    f_out->Close();

    app.Run();
    return 0;
}
