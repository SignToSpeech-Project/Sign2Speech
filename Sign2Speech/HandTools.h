#include <mutex>
#include "stdafx.h"

#ifndef DEF_HT
#define DEF_HT

#define MAXFRAME 59
#define DEBUG true
#define NBMETERS_STATIC 0.01
#define ERR_STRAIGHT_HOR 0.08
#define ERR_STRAIGHT_VER 0.08
#define ERR_STRAIGHT 0.10
#define VALID_HOR 0.03
#define ERR_VERT 0.06
#define VALID_VER 0.03
#define ERR_HOR 0.06
#define NBMETERS_FULLELLIPSE 0.06
#define ERR_ELLIPSE 0.20

using namespace std;
using std::cout;

class HandTools {

private :

	std::wstring g_sequencePath;

	int nbReadFrameRight = 0;
	int nbReadFrameLeft = 0;
	PXCHandData::FingerData rightHandData[1000][5];
	PXCHandData::FingerData leftHandData[1000][5];
	PXCPoint3DF32 massCenterCoordinates[1000];
	const uint32_t unite = 0b1;

	uint64_t frameCounter = 0;

	uint32_t fist = 0b0;
	uint32_t victory = 0b101000;
	uint32_t metal = 0b1000001000;

	void printBinary(uint32_t a, int nbBits);

	int nbGesture = 0;
	int firstFrame = 0;
	int nbFrame = 0;
	int currentGestComposee = 0;
	// 3) le vecteur de taille fournie par l'utilisateur (nbGestes) qui contient les différents gestes intermediaires
	//		et qui recomposent le geste final
	vector<long> completeGesture;

	PXCHandData::FingerData handData[1000][5];

	time_t start;

	bool learning = false;

	vector<uint8_t> trajectories;

	int nbMotCompose;

public :

	void setNbMotCompose(int nbmc) {
		nbMotCompose = nbmc;
	}

	bool getLearning() { return learning; }
	int calculateHammingDistance(uint32_t a, uint32_t b, int nBit, int step);

	boolean isGesture(uint32_t gesture, uint32_t ref, int distMax, int maxApproximateFinger);

	vector<uint32_t> removeOutValues(vector<uint32_t> v);
	
	uint32_t calculateAverage(PXCHandData::FingerData handData[1000][5], int length);

	long analyseGesture(PXCHandData::IHand *hand);

	uint8_t analyseMovement(int nbFrame);

	bool isStatic(uint8_t *out, int nbFrame);

	bool isHorizontal(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf);

	bool isVertical(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf);

	bool isStraight(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf, uint8_t *out);

	bool isElliptic(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf, uint8_t *out, int nbFrame);

	void printFold(PXCHandData::IHand *hand);

	long analyseXGestures(PXCHandData::IHand* hand);

	uint8_t averageTrajectory(vector<uint8_t> trajectories);
};


#endif