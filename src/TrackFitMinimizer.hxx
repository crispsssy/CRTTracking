#ifndef _TRACKFITMINIMIZER_HXX_
#define _TRACKFITMINIMIZER_HXX_

#include <iostream>
#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>
#include "CDCLineCandidate.hxx"
#include "CDCHit.hxx"
#include "CDCGeom.hxx"
#include "CalibInfo.hxx"
#include "RuntimeParameter.hxx"

class TrackFitMinimizer{
public:
	TrackFitMinimizer(CDCLineCandidate* track){ fTrack = track; }
	void TrackFitting();

private:
	double FittingFunctionRT(double const* pars);
	void UpdateTrack(double const* pars);

	CDCLineCandidate* fTrack;
};

#endif
