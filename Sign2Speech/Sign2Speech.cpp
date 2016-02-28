#include "stdafx.h"

#define MAXFRAME 21
#define DEBUG true

using namespace std;
using std::cout;

bool g_skeleton = false; // Writing skeleton data (22 joints) to console ouput
bool g_stop = false; // user closes application

std::wstring g_sequencePath;

PXCSession *g_session;
PXCSenseManager *g_senseManager;
PXCHandModule *g_handModule;
PXCHandData *g_handDataOutput;
PXCHandConfiguration *g_handConfiguration;

int nbReadFrameRight = 0;
int nbReadFrameLeft = 0;
PXCHandData::FingerData rightHandData[MAXFRAME][5];
PXCHandData::FingerData leftHandData[MAXFRAME][5];
const uint32_t unite = 0b1;

uint64_t frameCounter = 0;

void releaseAll();

uint32_t fist = 0b0;
uint32_t victory = 0b101000;
uint32_t metal = 0b1000001000;

int calculateHammingDistance(uint32_t a, uint32_t b, int nBit, int step) {
	int dist = 0;
	uint32_t one = (~(0b0)) >> (32 - step);
	for (int i = 0; i < nBit/step; i++) {
		if (((a >> (step * i)) & one) != ((b >> (step * i)) & one)) {
			dist++;
		}
	}
	return dist;
}

boolean isGesture(uint32_t gesture, uint32_t ref, int distMax, int maxApproximateFinger) {
	// if it's pinky or thumb wich are struggling to recognize there state avoid returning false


	// calculate the Hamming distance
	int dist = 0;
	int approximatefinger = 0;
	for (int b = 0; b < 5; b++) {
		uint8_t g = ((gesture >> (2 * b)) & 0b11);
		uint8_t r = ((ref >> (2 * b)) & 0b11);
		if (g != r) {
			/*if ((g == 0b01 && r == 0b00) || (g == 0b01 && r == 0b10) || (r == 0b01 && g == 0b00) || (r == 0b01 && g == 0b10)) {
				approximatefinger++;
			}
			else {*/
				dist++;
			/*}*/
		}
	}

	if (dist < distMax /*&& approximatefinger < maxApproximateFinger*/) {
		//std::printf("\t\t\t %s FIST %d \n", sideStr.c_str(), dist);
		if (DEBUG) {
			std::printf("[%ld] distance: %d\n", frameCounter, dist);
		}
		return true;
	}
	return false;
}

uint32_t calculateAvearge(PXCHandData::FingerData handData[MAXFRAME][5]) {
	uint32_t avg = 0x0;

	// calculate the gesture average
	for (int f = 0; f < 5; f++) {
		uint32_t sumFold = 0;
		for (int i = 0; i < MAXFRAME; i++) {
			sumFold += handData[i][f].foldedness;
		}
		sumFold /= MAXFRAME;
		if (sumFold > 66) {
			avg |= (0b10 << (2 * f));
		}
		else if (sumFold <= 66 && sumFold > 33) {
			avg |= (0b01 << (2 * f));
		}
	}
	return avg;
}

void analyseGesture(PXCHandData::IHand *hand) {
	// increment frame number
	frameCounter++;

	PXCHandData::BodySideType side = hand->QueryBodySide();
	int *nbReadFrame;
	PXCHandData::FingerData (*handData)[5];
	string sideStr = "";
	switch (side) {
	case PXCHandData::BodySideType::BODY_SIDE_LEFT:
		nbReadFrame = &nbReadFrameLeft;
		handData = leftHandData;
		sideStr = "LEFT";
		break;
	case PXCHandData::BodySideType::BODY_SIDE_RIGHT:
		nbReadFrame = &nbReadFrameRight;
		handData = rightHandData;
		sideStr = "RIGHT";
		break;
	default:
		nbReadFrame = &nbReadFrameRight;
		handData = rightHandData;
		sideStr = "RIGHT";
		break;
	}

	if (*nbReadFrame < MAXFRAME) {
		// add a new entry into the table
		PXCHandData::FingerData fingerData;
		for (int f = 0; f < 5; f++) {
			if (hand->QueryFingerData((PXCHandData::FingerType)f, fingerData) == PXC_STATUS_NO_ERROR) {
				handData[*nbReadFrame][f] = fingerData;
			}
		}
		(*nbReadFrame)++;
	}
	else {
		*nbReadFrame = 0;

		uint32_t average = calculateAvearge(handData);

		std::printf("[%ld]\t", frameCounter);
		for (int b = 9; b >= 0; b--) {
			std::printf("%d", (average >> b) & 0x1);
		}
		std::printf("\n");

		if (isGesture(average, fist, 1, 2)) {
			std::printf("[%ld]\t\t %s FIST\n", frameCounter, sideStr.c_str());
			if(webSock->getReadyState() != WebSocket::CLOSED) {
				webSock->send("{'content':'Fist'}");
			}
		}

		if (isGesture(average, victory, 1, 2)) {
			std::printf("[%ld]\t\t %s VICTORY\n", frameCounter, sideStr.c_str());
			if (webSock->getReadyState() != WebSocket::CLOSED) {
				webSock->send("{'content':'Victory'}");
			}
		}

		if (isGesture(average, metal, 1, 2)) {
			std::printf("[%ld]\t\t %s METAL\n", frameCounter, sideStr.c_str());
			if (webSock->getReadyState() != WebSocket::CLOSED) {
				webSock->send("{'content':'Metal'}");
			}
		}
	}
	return;
}

void printFold(PXCHandData::IHand *hand) {
	PXCHandData::FingerData fingerData;
	for (int f = 0; f < 5; f++) {
		if(hand->QueryFingerData((PXCHandData::FingerType)f, fingerData) == PXC_STATUS_NO_ERROR){
			std::printf("     %s)\tFoldedness: %d, Radius: %f \n", Definitions::FingerToString((PXCHandData::FingerType)f).c_str(), fingerData.foldedness, fingerData.radius);
		}
	}
}

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:

		// confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		g_stop = true;
		Sleep(1000);
		releaseAll();
		return(TRUE);

	default:
		return FALSE;
	}
}

void main(int argc, const char* argv[])
{
	Definitions::appName = argv[0];

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
	if (argc < 2)
	{
		Definitions::WriteHelpMessage();
		return;
	}

	// Setup
	g_session = PXCSession::CreateInstance();
	if (!g_session)
	{
		std::printf("Failed Creating PXCSession\n");
		return;
	}

	g_senseManager = g_session->CreateSenseManager();
	if (!g_senseManager)
	{
		releaseAll();
		std::printf("Failed Creating PXCSenseManager\n");
		return;
	}

	if (g_senseManager->EnableHand() != PXC_STATUS_NO_ERROR)
	{
		releaseAll();
		std::printf("Failed Enabling Hand Module\n");
		return;
	}

	g_handModule = g_senseManager->QueryHand();
	if (!g_handModule)
	{
		releaseAll();
		std::printf("Failed Creating PXCHandModule\n");
		return;
	}

	g_handDataOutput = g_handModule->CreateOutput();
	if (!g_handDataOutput)
	{
		releaseAll();
		std::printf("Failed Creating PXCHandData\n");
		return;
	}

	g_handConfiguration = g_handModule->CreateActiveConfiguration();
	if (!g_handConfiguration)
	{
		releaseAll();
		std::printf("Failed Creating PXCHandConfiguration\n");
		return;
	}

	// Iterating input parameters
	for (int i = 1; i<argc; i++)
	{
		if (strcmp(argv[i], "-skeleton") == 0)
		{
			std::printf("-Skeleton Information Enabled-\n");
			g_skeleton = true;
		}
	}

	g_handConfiguration->EnableStabilizer(true);
	g_handConfiguration->SetTrackingMode(PXCHandData::TRACKING_MODE_FULL_HAND);

	// Apply configuration setup
	g_handConfiguration->ApplyChanges();


	if (g_handConfiguration)
	{
		g_handConfiguration->Release();
		g_handConfiguration = NULL;
	}

	pxcI32 numOfHands = 0;

	// First Initializing the sense manager
	if (g_senseManager->Init() == PXC_STATUS_NO_ERROR)
	{
		std::printf("\nPXCSenseManager Initializing OK\n========================\n");

		// Acquiring frames from input device
		while (g_senseManager->AcquireFrame(true) == PXC_STATUS_NO_ERROR && !g_stop)
		{
			// Get current hand outputs
			if (g_handDataOutput->Update() == PXC_STATUS_NO_ERROR)
			{

				// Display joints
				if (g_skeleton)
				{
					PXCHandData::IHand *hand;
					for (int i = 0; i < g_handDataOutput->QueryNumberOfHands(); ++i)
					{
						g_handDataOutput->QueryHandData(PXCHandData::ACCESS_ORDER_BY_TIME, i, hand);
						std::string handSide = "Unknown Hand";
						handSide = hand->QueryBodySide() == PXCHandData::BODY_SIDE_LEFT ? "Left Hand" : "Right Hand";
						//std::printf("%s\n==============\n", handSide.c_str());
						//printFold(hand);
						analyseGesture(hand);
					}
				}

			} // end if update

			g_senseManager->ReleaseFrame();
		} // end while acquire frame
	} // end if Init

	else
	{
		releaseAll();
		std::printf("Failed Initializing PXCSenseManager\n");
		return;
	}

	releaseAll();
}

void releaseAll()
{
	if (g_senseManager)
	{
		g_senseManager->Close();
		g_senseManager->Release();
		g_senseManager = NULL;
	}
	if (g_session)
	{
		g_session->Release();
		g_session = NULL;
	}
}

