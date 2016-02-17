#include "stdafx.h"

#define MAXFRAME 21

using namespace std;
using std::cout;

bool g_live = false; // true - Working in live camera mode, false - sequence mode
bool g_gestures = false; // Writing gesture data to console ouput
bool g_alerts = false; // Writing alerts data to console ouput
bool g_skeleton = false; // Writing skeleton data (22 joints) to console ouput
bool g_default = false; // Writing hand type to console ouput
bool g_stop = false; // user closes application

std::wstring g_sequencePath;

PXCSession *g_session;
PXCSenseManager *g_senseManager;
PXCHandModule *g_handModule;
PXCHandData *g_handDataOutput;
PXCHandConfiguration *g_handConfiguration;

int nbReadFrameRight = 0;
int nbReadFrameLeft = 0;
uint32_t rightHandData[MAXFRAME];
uint32_t leftHandData[MAXFRAME];

void releaseAll();

void analyseGesture(PXCHandData::IHand *hand) {
	PXCHandData::BodySideType side = hand->QueryBodySide();
	int *nbReadFrame;
	uint32_t *handData;
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
		uint32_t finger = 0x0;
		for (int f = 0; f < 5; f++) {
			if (hand->QueryFingerData((PXCHandData::FingerType)f, fingerData) == PXC_STATUS_NO_ERROR) {
				if (fingerData.foldedness > 66) {
					finger |= (0x1 << (2 * f));
				}
				else if (fingerData.foldedness <= 66 && fingerData.foldedness > 33) {
					finger |= (0x1 << (2 * f + 1));
				}
			}
		}
		handData[*nbReadFrame] = finger;
		(*nbReadFrame)++;
	}
	else {
		*nbReadFrame = 0;

		// calculate the gesture average
		uint32_t average = 0x0;
		uint32_t unite = 0x1;
		for (int b = 0; b < 10; b++) {
			int count0 = 0;
			int count1 = 0;
			for (int i = 0; i < MAXFRAME; i++) {
				if (((handData[i] >> b) & 0x1) == 0x0) {
					count0++;
				}
				else {
					count1++;
				}
			}
			if (count0 < count1) {
				average |= (unite << b);
			}
		}

		for (int b = 9; b >= 0; b--) {
			std::printf("%d", (average >> b) & 0x1);
		}
		std::printf("\n");

		// calculate the Hamming distance
		uint32_t fist = 0b0;
		int distance = 0;
		for (int b = 0; b < 10; b++) {
			if (((average >> b) & 0x1) != ((fist >> b) & 0x1)) {
				distance++;
			}
		}

		if (distance < 2) {
			std::printf("\t\t\t %s FIST %d \n", sideStr.c_str(), distance);
		}

		// calculate the Hamming distance
		uint32_t victory = 0b010100;
		distance = 0;
		for (int b = 0; b < 10; b++) {
			if (((average >> b) & 0x1) != ((victory >> b) & 0x1)) {
				distance++;
			}
		}

		if (distance < 2) {
			std::printf("\t\t\t %s VICTORY %d\n", sideStr.c_str(), distance);
		}
	}

	//switch (side)
	//{
	//case PXCHandData::BodySideType::BODY_SIDE_LEFT:
	//	if (nbReadFrameLeft < MAXFRAME) {
	//		// add a new entry into the table
	//		PXCHandData::FingerData fingerData;
	//		uint32_t finger = 0x0;
	//		for (int f = 0; f < 5; f++) {
	//			if (hand->QueryFingerData((PXCHandData::FingerType)f, fingerData) == PXC_STATUS_NO_ERROR) {
	//				if (fingerData.foldedness > 66) {
	//					finger |= (0x1 << (2 * f));
	//				}
	//				else if (fingerData.foldedness <= 66 && fingerData.foldedness > 33) {
	//					finger |= (0x1 << (2 * f + 1));
	//				}
	//			}
	//		}
	//		leftHandData[nbReadFrameLeft] = finger;
	//		nbReadFrameLeft++;
	//	}
	//	else {
	//		nbReadFrameLeft = 0;

	//		// calculate the gesture average
	//		uint32_t average = 0x0;
	//		uint32_t unite = 0x1;
	//		for (int b = 0; b < 10; b++) {
	//			int count0 = 0;
	//			int count1 = 0;
	//			for (int i = 0; i < MAXFRAME; i++) {
	//				if (((leftHandData[i] >> b) & 0x1) == 0x0) {
	//					count0++;
	//				}
	//				else {
	//					count1++;
	//				}
	//			}
	//			if (count0 < count1) {
	//				average |= (unite << b);
	//			}
	//		}

	//		for (int b = 9; b >= 0; b--) {
	//			std::printf("%d", (average >> b) & 0x1);
	//		}
	//		std::printf("\n");

	//		// calculate the Hamming distance
	//		uint32_t fist = 0b0;
	//		int distance = 0;
	//		for (int b = 0; b < 10; b++) {
	//			if (((average >> b) & 0x1) != ((fist >> b) & 0x1)) {
	//				distance++;
	//			}
	//		}

	//		if (distance < 2) {
	//			std::printf("\t\t\t LEFT FIST %d \n", distance);
	//		}

	//		// calculate the Hamming distance
	//		uint32_t victory = 0b010100;
	//		distance = 0;
	//		for (int b = 0; b < 10; b++) {
	//			if (((average >> b) & 0x1) != ((victory >> b) & 0x1)) {
	//				distance++;
	//			}
	//		}

	//		if (distance < 2) {
	//			std::printf("\t\t\t LEFT VICTORY %d\n", distance);
	//		}
	//	}
	//	break;
	//case PXCHandData::BodySideType::BODY_SIDE_RIGHT:
	//	if (nbReadFrameRight < MAXFRAME) {
	//		// add a new entry into the table
	//		PXCHandData::FingerData fingerData;
	//		uint32_t finger = 0x0;
	//		for (int f = 0; f < 5; f++) {
	//			if (hand->QueryFingerData((PXCHandData::FingerType)f, fingerData) == PXC_STATUS_NO_ERROR) {
	//				if (fingerData.foldedness > 66) {
	//					finger |= (1 << (2 * f));
	//				}
	//				else if (fingerData.foldedness <= 66 && fingerData.foldedness > 33) {
	//					finger |= (1 << (2 * f + 1));
	//				}
	//			}
	//		}
	//		rightHandData[nbReadFrameRight] = finger;
	//		nbReadFrameRight++;
	//	}
	//	else {
	//		nbReadFrameRight = 0;

	//		// calculate the gesture average
	//		uint32_t average = 0x0;
	//		uint32_t unite = 0x1;
	//		for (int b = 0 ; b < 10; b++) {
	//			int count0 = 0;
	//			int count1 = 0;
	//			for (int i = 0; i < MAXFRAME; i++) {
	//				if (((rightHandData[i] >> b) & 0x1) == 0x0) {
	//					count0++;
	//				}
	//				else {
	//					count1++;
	//				}
	//			}
	//			if (count0 < count1) {
	//				average |= (unite << b);
	//			}
	//		}

	//		for (int b = 9; b >= 0; b--) {
	//			std::printf("%d", (average >> b) & 0x1);
	//		}
	//		std::printf("\n");

	//		// calculate the Hamming distance
	//		uint32_t fist = 0b0;
	//		int distance = 0;
	//		for (int b = 0; b < 10; b++) {
	//			if (((average >> b) & 0x1) != ((fist >> b) & 0x1)) {
	//				distance++;
	//			}
	//		}

	//		if (distance < 2) {
	//			std::printf("\t\t\t RIGHT FIST %d\n", distance);
	//		}

	//		// calculate the Hamming distance
	//		uint32_t victory = 0b010100;
	//		distance = 0;
	//		for (int b = 0; b < 10; b++) {
	//			if (((average >> b) & 0x1) != ((victory >> b) & 0x1)) {
	//				distance++;
	//			}
	//		}

	//		if (distance < 2) {
	//			std::printf("\t\t\t RIGHT VICTORY %d\n", distance);
	//		}
	//	}
	//	break;
	//default:
	//	break;
	//}
	return;
}

boolean isFist(PXCHandData::IHand *hand) {
	PXCHandData::JointData jointData;
	PXCHandData::FingerData fingerData;
	for (int f = 0; f < 5; f++) {
		if(hand->QueryFingerData((PXCHandData::FingerType)f, fingerData) == PXC_STATUS_NO_ERROR){
			std::printf("     %s)\tFoldedness: %d, Radius: %f \n", Definitions::FingerToString((PXCHandData::FingerType)f).c_str(), fingerData.foldedness, fingerData.radius);
		}
	}
	/*for (int j = 0; j < 22; ++j)
	{
		if (hand->QueryTrackedJoint((PXCHandData::JointType)j, jointData) == PXC_STATUS_NO_ERROR)
		{
			std::printf("     %s)\tX: %f, Y: %f, Z: %f \n", Definitions::JointToString((PXCHandData::JointType)j).c_str(), jointData.positionWorld.x, jointData.positionWorld.y, jointData.positionWorld.z);
		}
	}*/
	return true;
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
		if (strcmp(argv[i], "-live") == 0)
		{
			g_live = true;
		}

		if (strcmp(argv[i], "-seq") == 0)
		{
			g_live = false;
			std::string tmp(argv[i + 1]);
			i++;
			g_sequencePath.clear();
			g_sequencePath.assign(tmp.begin(), tmp.end());
			continue;
		}

		if (strcmp(argv[i], "-gestures") == 0)
		{
			std::printf("-Gestures Are Enabled-\n");
			g_handConfiguration->EnableAllGestures();
			g_gestures = true;
		}

		if (strcmp(argv[i], "-alerts") == 0)
		{
			std::printf("-Alerts Are Enabled-\n");
			g_handConfiguration->EnableAllAlerts();
			g_alerts = true;
		}

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

	// run sequences as fast as possible
	if (!g_live)
	{
		g_senseManager->QueryCaptureManager()->SetFileName(g_sequencePath.c_str(), false);
		g_senseManager->QueryCaptureManager()->SetRealtime(false);
	}
	if (g_handConfiguration)
	{
		g_handConfiguration->Release();
		g_handConfiguration = NULL;
	}

	g_default = !(g_alerts && g_gestures && g_skeleton);
	pxcI32 numOfHands = 0;

	// First Initializing the sense manager
	if (g_senseManager->Init() == PXC_STATUS_NO_ERROR)
	{
		std::printf("\nPXCSenseManager Initializing OK\n========================\n");

		if (g_default)
		{
			std::printf("Number of hands: %d\n", numOfHands);
		}

		// Acquiring frames from input device
		while (g_senseManager->AcquireFrame(true) == PXC_STATUS_NO_ERROR && !g_stop)
		{
			// Get current hand outputs
			if (g_handDataOutput->Update() == PXC_STATUS_NO_ERROR)
			{
				// Display alerts
				if (g_alerts)
				{
					PXCHandData::AlertData alertData;
					for (int i = 0; i < g_handDataOutput->QueryFiredAlertsNumber(); ++i)
					{
						if (g_handDataOutput->QueryFiredAlertData(i, alertData) == PXC_STATUS_NO_ERROR)
						{
							std::printf("%s was fired at frame %d \n", Definitions::AlertToString(alertData.label).c_str(), alertData.frameNumber);
						}
					}
				}

				// Display gestures
				if (g_gestures)
				{
					PXCHandData::GestureData gestureData;
					for (int i = 0; i < g_handDataOutput->QueryFiredGesturesNumber(); ++i)
					{
						if (g_handDataOutput->QueryFiredGestureData(i, gestureData) == PXC_STATUS_NO_ERROR)
						{
							std::wprintf(L"%s, Gesture: %s was fired at frame %d \n", Definitions::GestureStateToString(gestureData.state), gestureData.name, gestureData.frameNumber);
						}
					}
				}

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
						//isFist(hand);
						analyseGesture(hand);
					}
				}

				// Display number of hands
				if (g_default)
				{
					if (numOfHands != g_handDataOutput->QueryNumberOfHands())
					{
						numOfHands = g_handDataOutput->QueryNumberOfHands();
						std::printf("Number of hands: %d\n", numOfHands);
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

