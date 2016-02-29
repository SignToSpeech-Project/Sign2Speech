#include "stdafx.h"
#include <thread>
#include <mutex>
#include "Dictionary.h"



PXCSession *g_session;
PXCSenseManager *g_senseManager;
PXCHandModule *g_handModule;
PXCHandData *g_handDataOutput;
PXCHandConfiguration *g_handConfiguration;

bool g_skeleton = false; // Writing skeleton data (22 joints) to console ouput
bool g_stop = false; // user closes application
bool program_on = true; // =off : End of the program

std::mutex mProgram_on; 
std::mutex mBufferR; //Buffer of which symbols the user is currently doing (Default mod of the program)
std::mutex mBufferW; //Buffer of symbols chain you need to add to the dictionary ( Learning mod ON)

vector<long> bufferRead;
vector<vector<pair<string, long>>> bufferWrite;



//TEMP

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

bool CtrlHandler(DWORD fdwCtrlType, bool g_stop)
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


//Thread managing the Dictionary
void threadDico() {
	/*
	//Initialisation Dico-------------------------------------------------------------------------
	Dictionary d;

	Parser p("examples/example.json");
	
	// vector of vector used to store all the pairs
	vector< vector< pair<string, long> > > res;
	
	// parse the json file
	cout << "Parsing Json file..." << endl;
	res = p.ReadJsonFile();
	cout << "Parsing > OK" << endl;
	cout << "" << endl;
	//
	// insert all the vectors of pairs in the dictionary
	for (vector<vector<pair<string, long>>>::iterator it = res.begin(); it != res.end(); ++it){
		d.insertList((*it));
	}

	mProgram_on.lock();
	while (program_on) {
		mProgram_on.unlock();

		mBufferW.lock();
		if (bufferWrite.size != 0) { //Update Dico
			for (vector<vector<pair<string, long>>>::iterator it = bufferWrite.begin(); it != bufferWrite.end(); ++it) {
				d.insertList((*it));
				bufferWrite.erase(it);
			}
		}
		mBufferW.unlock();

		mBufferR.lock();
		//TODO : Reset dico au bout d'un timeOut
		if (bufferRead.size != 0) { //Get CurrentSymbol
			for (vector<long>::iterator it = bufferRead.begin(); it != bufferRead.end(); ++it) {
				string currentSymbol = d.read(*it);
				if (currentSymbol != "0x0 : Not final word") {
					//TODO : Affichage à l'écran du mot correspondant
				}
				bufferRead.erase(it);
			}
		}
		mBufferR.unlock();

		mProgram_on.lock();
	}
	mProgram_on.unlock();

	//Sauvegarde Dico-------------------------------------------------------------------------

	cout << "Vector Dictionary creation..." << endl;
	vector<vector<pair<string, long>>> v = d.createVectorDictionary(); // Create vector of vectors of pairs from the dict
	cout << "Vector Dictionary > OK" << endl;
	cout << "" << endl;
	cout << "Writting Json file from vect..." << endl;
	p.WriteJsonFile(v);
	cout << "Json file > OK" << endl;
	cout << "" << endl;
	*/
}


//Thread managing the camera and the gestures recognization
void threadHandTools(int argc, const char* argv[]) {

	mProgram_on.lock();

	while (program_on) {

		mProgram_on.unlock();

		Definitions::appName = argv[0];

		//TODO : Instancier HandTools
		HandTools h;

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
		for (int i = 1; i < argc; i++)
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
							h.analyseGesture(hand); //TODO : Fonction dans HandTools
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

		mProgram_on.lock();
	}
	mProgram_on.unlock();
}


void main(int argc, const char* argv[])
{

	std::thread tHandTools(threadHandTools, argc, argv);
	std::thread tDico(threadDico);

	tHandTools.join();
	tDico.join();

	
}



