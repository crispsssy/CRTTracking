#include <iostream>
#include <TApplication.h>
#include "ReFitLoop.hxx"
#include "RuntimeParameter.hxx"

int main(int argc, char** argv){
    if(argc < 5){
        std::cerr<<"./run <in_file> <out_path> <startEve> <numEve>"<<std::endl;
        exit(-1);
    }
    std::string f_in_path = argv[1];
    std::string f_out_path = argv[2];
    int startEvent = atoi(argv[3]);
    int numEvent = atoi(argv[4]);

    TApplication app("app", &argc, argv);

    ReFitLoop(f_in_path, f_out_path, startEvent, numEvent);

    if(RuntimePar::runMode) app.Run();
    return 0;
}
