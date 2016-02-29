#include "stdafx.h"
#include <thread>



PXCSession *g_session;
PXCSenseManager *g_senseManager;
PXCHandModule *g_handModule;
PXCHandData *g_handDataOutput;
PXCHandConfiguration *g_handConfiguration;

bool g_skeleton = false; // Writing skeleton data (22 joints) to console ouput
bool g_stop = false; // user closes application


void threadDico() {

}

void threadHandTools(int argc, const char* argv[]) {

	Definitions::appName = argv[0];

	//TODO : Instancier HandTools

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE); //TODO : Pas compris, CtrlHandler c'est une fonction de HandTools
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
		releaseAll(); //TODO : Fonction dans HandTools.cpp . Attention, il faut lui passer des paramètres maintenant. A discuter
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
						analyseGesture(hand); //TODO : Fonction dans HandTools
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


void main(int argc, const char* argv[])
{

	std::thread tHandTools(threadHandTools, argc, argv);
	std::thread tDico(threadDico);

	tHandTools.join();
	tDico.join();

	
}



