#ifndef _CDCHIT_HXX_
#define _CDCHIT_HXX_

#include <iostream>
#include <vector>

class CDCHit
{
public:
	CDCHit() {}
	CDCHit(int channel){ fChannelID = channel; }
	~CDCHit() {}

	void SetHit(int fLayerID, int fCellID);
	void InsertDriftTime(double driftTime){ fDriftTime.push_back(driftTime); }
	void SetDOCA(double DOCA){ fDOCA = DOCA; }
	int const GetChannelID() const { return fChannelID; }
	double const GetDriftTime(int index) const { return fDriftTime.at(index); }
	double const GetDOCA() const { return fDOCA; }

	void ls() const;

private:
	int fChannelID = -999;
	std::vector<double> fDriftTime;
	double fDOCA = 0.;
};

class CDCHitContainer : public std::vector<CDCHit*>
{
public:

	CDCHitContainer() {}
	~CDCHitContainer() {}
	void ls() const; 

};

#endif
