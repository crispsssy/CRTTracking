#ifndef _TRACKFITHANDLER_HXX_
#define _TRACKFITHANDLER_HXX_

#include <iostream>
#include <map>
#include <TVector3.h>
#include <TVector2.h>
#include <TMath.h>
#include <TMinuit.h>
#include "CDCLineCandidate.hxx"
#include "CDCHit.hxx"
#include "CDCGeom.hxx"
#include "TrackFitMinimizer.hxx"
#include "RuntimeParameter.hxx"

class TrackFitHandler{
public:
    static TrackFitHandler& Get();
    CDCLineCandidateContainer* Find3DTracks(CDCLineCandidateContainer* lines);

private:
    TrackFitHandler();
    TrackFitHandler(TrackFitHandler const& src);
    TrackFitHandler& operator=(TrackFitHandler const& rhs);

    static TrackFitHandler* fTrackFitHandler;

    bool IsGoodPair(CDCLineCandidate* lineOdd, CDCLineCandidate* lineEven);
    CDCLineCandidate* FindInitialTrack(CDCLineCandidate* lineOdd, CDCLineCandidate* lineEven);

    double fMaxDistanceEO = 300; //mm distance between even and odd layer lines
    double fMaxPhiDiff = 1.; //rad
};

#endif
