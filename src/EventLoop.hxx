#ifndef _EVENTLOOP_HXX_
#define _EVENTLOOP_HXX_

#include <iostream>
#include <iomanip>
#include <vector>
#include <TSystem.h>
#include <TInterpreter.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TVector3.h>
#include <TMath.h>
#include "CDCHit.hxx"
#include "Preprocess.hxx"
#include "EventDisplay.hxx"
#include "CDCLineCandidate.hxx"
#include "HoughHandler.hxx"
#include "TrackFitHandler.hxx"

void EventLoop(std::string f_in_path, std::string f_out_path, int startEvent, int numEvent);

#endif
