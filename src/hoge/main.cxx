#include <TROOT.h>
#include <TInterpreter.h>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>


int main(){

	gInterpreter->GenerateDictionary("vector<TVector3>", "vector;TVector3.h");

std::vector<TVector3> fv;
std::vector<double> fd;
TFile* f_out = new TFile("some.root", "RECREATE");
TTree* t_out = new TTree("t", "t");

t_out->Branch("v", &fv);
t_out->Branch("d", &fd);

for(auto i : {1.,2.,3.}){
    fd.emplace_back(i);
    fv.emplace_back(i,i*2,i*3);
    t_out->Fill();
}

t_out->Write();
f_out->Close();
}
