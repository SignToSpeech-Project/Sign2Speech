#include "stdafx.h"

#ifndef DEF_HT
#define DEF_HT

#define MAXFRAME 21
#define DEBUG true

using namespace std;
using std::cout;



class HandTools {

private :
	



	std::wstring g_sequencePath;



	int nbReadFrameRight = 0;
	int nbReadFrameLeft = 0;
	PXCHandData::FingerData rightHandData[MAXFRAME][5];
	PXCHandData::FingerData leftHandData[MAXFRAME][5];
	const uint32_t unite = 0b1;

	uint64_t frameCounter = 0;

	uint32_t fist = 0b0;
	uint32_t victory = 0b101000;
	uint32_t metal = 0b1000001000;

public :

	int calculateHammingDistance(uint32_t a, uint32_t b, int nBit, int step);

	boolean isGesture(uint32_t gesture, uint32_t ref, int distMax, int maxApproximateFinger);

	uint32_t calculateAverage(PXCHandData::FingerData handData[MAXFRAME][5]);

	void analyseGesture(PXCHandData::IHand *hand);

	void printFold(PXCHandData::IHand *hand);
};


#endif