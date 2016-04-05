#include <mutex>
#include "stdafx.h"

#ifndef DEF_HT
#define DEF_HT

#define MAXFRAME 59
#define DEBUG true

// Trajectory error thresholds
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
	int nbFrameRight = 0;
	int nbFrameLeft = 0;
	PXCHandData::FingerData rightHandData[1000][5];
	PXCHandData::FingerData leftHandData[1000][5];
	PXCHandData::FingerData handData[1000][5];
	const uint32_t unite = 0b1;

	uint64_t frameCounter = 0;

	void printBinary(uint32_t a, int nbBits); 

	time_t start;
	SYSTEMTIME gestureStart;

	// Variables for trajectory recognition
	PXCPoint3DF32 massCenterCoordinates[1000];
	vector<uint8_t> trajectories;

	// Variables for auto learning
	bool learning = false;
	int nbGesture = 0;
	int firstFrame = 0;
	int nbFrame = 0;
	int nbMassCenter = 0;
	int currentGestComposee = 0;
	int nbMotCompose;

public :

	void learningMode(int nbmc) {
		nbMotCompose = nbmc;
		nbGesture = 0;
		firstFrame = 0;
		nbFrame = 0;
		currentGestComposee = 0;
		nbMassCenter = 0;
		learning = true;
		trajectories.clear();
	}

	bool getLearning() { return learning; }

	int calculateHammingDistance(uint32_t a, uint32_t b, int nBit, int step);

	boolean isGesture(uint32_t gesture, uint32_t ref, int distMax, int maxApproximateFinger);

	vector<uint32_t> removeOutValues(vector<uint32_t> v);
	
	uint32_t calculateAverage(PXCHandData::FingerData handData[1000][5], int length);

	long analyseGesture(PXCHandData::IHand *hand);

	void printFold(PXCHandData::IHand *hand);

	/***********************************************/
	/* *************** Trajectories ************** */
	/***********************************************/

	// Analyse the movement of the gesture (straight or elliptic) from all the points that compose the gesture
	// Param: nbFrame is the number of captured frames
	// Returns 8 bits encoding the trajectory
	uint8_t analyseMovement(int nbFrame);

	// Checks if the movement is static
	// Params: out will contain the bits encoding the trajectory, nbFrame is the number of captured frames
	// Returns true if static
	bool isStatic(uint8_t *out, int nbFrame);

	// Checks if the movement is horizontal
	// Params: p0 is the first captured point, pf is the last one, pm is the middle one
	// Returns true if horizontal
	bool isHorizontal(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf);

	// Checks if the movement is vertical
	// Params: p0 is the first captured point, pf is the last one, pm is the middle one
	// Returns true if vertical
	bool isVertical(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf);

	// Checks if the movement is straight
	// Params: p0 is the first captured point, pf is the last one, pm is the middle one, out will contain the bits encoding the trajector
	// Returns true if straight
	bool isStraight(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf, uint8_t *out);

	// Checks if the movement is elliptic
	// Params: p0 is the first captured point, pf is the last one, pm is the middle one, out will contain the bits encoding the trajector,
	// nbFrame is the number of captured frames
	// Returns true if elliptic
	bool isElliptic(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf, uint8_t *out, int nbFrame);

	/***********************************************/
	/* ***************** Learning **************** */
	/***********************************************/

	// Analyse the serie of 3 repeated gestures to compute the average
	// Params: hand structure given by the SDK during the tracking process
	// Returns the encoding of the average gesture
	long analyseXGestures(PXCHandData::IHand* hand);

	// Computes the average trajectory of several gestures
	// Param : vector of trajectories (uint_8) to compute the average on
	// Returns the computed average trajectory
	uint8_t averageTrajectory(vector<uint8_t> trajectories);
};


#endif