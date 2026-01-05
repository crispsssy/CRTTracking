#ifndef _REFITLOOP_HXX_
#define _REFITLOOP_HXX_

#include <iostream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include "CDCHit.hxx"
#include "CDCLineCandidate.hxx"
#include "TrackFitHandler.hxx"
#include "Utilities.hxx"

void ReFitLoop(std::string const& f_in_path, std::string const& f_out_path, int const startEvent, int const numEvent);

#endif
