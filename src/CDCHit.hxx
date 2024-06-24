#ifndef _CDCHIT_HXX_
#define _CDCHIT_HXX_

#include <iostream>
#include <vector>

class CDCHit
{
public:
	CDCHit() {}
	CDCHit(int channel){ fChannelID = channel; }
	CDCHit(int channel, double z){ fChannelID = channel, fZ = z; }
	~CDCHit() {}

	void InsertADC(short adc){ fADC.push_back(adc); }
	void InsertDriftTime(double driftTime){ fDriftTime.push_back(driftTime); }
	void SetDOCA(double DOCA){ fDOCA = DOCA; }
	void SetZ(double z){ fZ = z; }
	bool IncreaseTDCIndex();
	void InsertDriftDistance(double dis){ fDriftDistance.push_back(dis); }
	int const GetChannelID() const { return fChannelID; }
	double const GetDriftTime(int index) const { return fDriftTime.at(index); }
	std::vector<double> const GetDriftTime() const { return fDriftTime; }
	double const GetDOCA() const { return fDOCA; }
	double const GetZ() const { return fZ; }
	double const GetDriftDistance(int index) const { return fDriftDistance.at(index); }
	std::vector<double> const GetDriftDistance() const { return fDriftDistance; }

	void ls() const;

private:
	int fChannelID = -999;
	std::vector<short> fADC;
	std::vector<double> fDriftTime;
	std::vector<double> fDriftDistance;
	double fDOCA = 0.;
	double fZ = 0.;
	unsigned int fUseTDC = 0; //determine which TDC will be used for fitting
};

class CDCHitContainer : public std::vector<CDCHit*>
{
public:

	CDCHitContainer() {}
	~CDCHitContainer() {}
	void ls() const; 

};

#endif
