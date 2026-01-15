#include <iostream>
#include <TApplication.h>
#include "EventLoop.hxx"
#include "RuntimeParameter.hxx"
#include "Utilities.hxx"

int main(int argc, char** argv){
	if(argc < 4){
		std::cerr<<"./run <in_file> <out_file> <startEve> <numEve>"<<std::endl;
		exit(-1);
	}
	std::string f_in_path = argv[1];
	std::string f_out_path = argv[2];
	int startEvent = atoi(argv[3]);
	int numEvent = atoi(argv[4]);
	std::string::size_type idx = f_in_path.find("run0");
	RuntimePar::f_in_path = f_in_path;
	RuntimePar::runNum = FindPar(f_in_path, "run");
	RuntimePar::startEvent = startEvent;

	TApplication app("app", &argc, argv);

	EventLoop(f_in_path, f_out_path, startEvent, numEvent);

	if(RuntimePar::runMode) app.Run();

	return 0;
}
