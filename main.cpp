#include <windows.h>
#include <iostream>
/*
#include "pxcsensemanager.h"
#include "pxcmetadata.h"
#include "service/pxcsessionservice.h"

#include "pxchanddata.h"
#include "pxchandconfiguration.h"

#include "Definitions.h"

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

void releaseAll();

BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
  switch( fdwCtrlType ) 
  { 
    // Handle the CTRL-C signal. 
    case CTRL_C_EVENT: 

    // confirm that the user wants to exit. 
    case CTRL_CLOSE_EVENT: 
	  g_stop = true;	
	  Sleep(1000);
	  releaseAll();
      return( TRUE ); 
 
    default: 
      return FALSE; 
  } 
} */

#include "Dictionary.h"
#include "Parser.h"
#include <vector>

using std::string;
using std::vector;
using std::pair;
using std::cout;
using std::cin;

int main(int argc, const char* argv[])
{
	Dictionary d;
	Parser p("examples/example.json");

	// vector of vector used to store all the pairs
	vector< vector< pair<string, long> > > res;

	// parse the json file
	res = p.ReadJsonFile();

	// insert all the vectors of pairs in the dictionary
	for (vector<vector<pair<string, long>>>::iterator it = res.begin(); it != res.end(); ++it){
		d.insertList((*it));
	}

	//getsymbol
	string mot = d.read(1);
	mot = d.read(2);
	mot = d.read(3);
	mot = d.read(1);
	cout << mot << endl;

	vector<vector<pair<string, long>>> v = d.createVectorDictionary(); //Creation vecteur de vecteur de paires à partir du dictionnaire

	while (true);
	/*Dictionary d;

	//Première série pour faire un mot 
	pair<string, long> p1("jambon1", 1);
	pair<string, long> p2("jambon4", 4);
	pair<string, long> p3("jambon3", 3);
	pair<string, long> p4("jambon2", 2);

	//Deuxième série pour faire un mot
	pair<string, int> p5("jambon1", 1); //Je vérifie que ca ne duplique pas le jambon1 du premier vecteur
	pair<string, long> p6("saucisson1", 4); //p6 symbole = p2 symbole = 4. Cela va overwrite le "mot" lié au symbole 8 qui va devenir "saucisson1" à la place de jambon4
	pair<string, long> p7("saucisson2", 7);

	//Troisème mot
	pair<string, long> p8("autruche", 8);

	//Création des structures à envoyer au dictionnaire (un vecteur par série de mots/signes)
	vector<pair<string, long>> v = { p1,p2,p3,p4};

	vector<pair<string, long>> v2 = { p5,p6,p7};

	vector<pair<string, long>> v3 = { p8 };
	
	//On les insèrent au dico
	d.insertList(v);
	d.insertList(v2);
	d.insertList(v3);

	//Lire un symbole. 
	string temp = d.read(1); //Renvoie "" car 1 n'est pas un symbole final
	cout << "la valeur premiere lecture est : " << temp <<"\n";
	//Si le timer expire, alors on affiche le mot correspondant au dernier mot lu, soit le mot lié à symbol 1
	temp = d.refreshDictionary();
	cout << "la valeur deuxième lecture est : " << temp << "\n";

	temp = d.read(1); //Renvoie "" car 1 n'est pas un symbole final
	cout << "la valeur troisième lecture est : " << temp << "\n";
	temp = d.read(1); //Renvoie la valeur du symbol 1 car 1 n'as pas de fils avec le symbol 1, c'est donc un symbol 1
	cout << "la valeur quatrième lecture est : " << temp << "\n";
	temp = d.read(1);
	temp = d.read(4);
	temp = d.read(7);
	temp = d.refreshDictionary();
	cout << "la valeur cinquième lecture est : " << temp << "\n";

	temp = d.read(1);
	cout << "la valeur sixieme lecture est : " << temp << "\n";
	temp = d.read(4);
	cout << "la valeur septième lecture est : " << temp << "\n";
	temp = d.read(1);
	cout << "la valeur huitième lecture est : " << temp << "\n";

	temp = d.refreshDictionary();
	cout << "la valeur neuvième lecture est : " << temp << "\n";

	while (true);

	return 0;*/

	/*Definitions::appName = argv[0];

	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
	if(argc < 2)
	{
		Definitions::WriteHelpMessage();
		return;
	}

	// Setup
	g_session = PXCSession::CreateInstance();
	if(!g_session)
	{
		std::printf("Failed Creating PXCSession\n");
		return;
	}

	g_senseManager = g_session->CreateSenseManager();
	if(!g_senseManager)
	{
		releaseAll();
		std::printf("Failed Creating PXCSenseManager\n");
		return;
	}

	if(g_senseManager->EnableHand() != PXC_STATUS_NO_ERROR)
	{
		releaseAll();
		std::printf("Failed Enabling Hand Module\n");
		return;
	}

	g_handModule = g_senseManager->QueryHand();
	if(!g_handModule)
	{
		releaseAll();
		std::printf("Failed Creating PXCHandModule\n");
		return;
	}

	g_handDataOutput = g_handModule->CreateOutput();
	if(!g_handDataOutput)
	{
		releaseAll();
		std::printf("Failed Creating PXCHandData\n");
		return;
	}

	g_handConfiguration = g_handModule->CreateActiveConfiguration();
	if(!g_handConfiguration)
	{
		releaseAll();
		std::printf("Failed Creating PXCHandConfiguration\n");
		return;
	}

	// Iterating input parameters
	for (int i=1;i<argc;i++)
	{
		if (strcmp(argv[i],"-live")==0)
		{
			g_live=true;
		}

		if (strcmp(argv[i],"-seq")==0)
		{
			g_live=false;
			std::string tmp(argv[i+1]);
			i++;
			g_sequencePath.clear();
			g_sequencePath.assign(tmp.begin(),tmp.end());
			continue;
		}

		if (strcmp(argv[i],"-gestures")==0)
		{
			std::printf("-Gestures Are Enabled-\n");
			g_handConfiguration->EnableAllGestures();
			g_gestures = true;
		}

		if (strcmp(argv[i],"-alerts")==0)
		{
			std::printf("-Alerts Are Enabled-\n");
			g_handConfiguration->EnableAllAlerts();
			g_alerts = true;
		}

		if (strcmp(argv[i],"-skeleton")==0)
		{
			std::printf("-Skeleton Information Enabled-\n");
			g_skeleton = true;
		}
	}

	// Apply configuration setup
	g_handConfiguration->ApplyChanges();

	// run sequences as fast as possible
	if(!g_live)
	{
		g_senseManager->QueryCaptureManager()->SetFileName(g_sequencePath.c_str(),false);
		g_senseManager->QueryCaptureManager()->SetRealtime(false);
	}
	if(g_handConfiguration)
	{
		g_handConfiguration->Release();
		g_handConfiguration = NULL;
	}

	g_default = !(g_alerts && g_gestures && g_skeleton); 
	pxcI32 numOfHands = 0;

	// First Initializing the sense manager
	if(g_senseManager->Init() == PXC_STATUS_NO_ERROR)
	{
		std::printf("\nPXCSenseManager Initializing OK\n========================\n");

		if(g_default)
		{
			std::printf("Number of hands: %d\n",numOfHands);
		}

		// Acquiring frames from input device
		while(g_senseManager->AcquireFrame(true) == PXC_STATUS_NO_ERROR && !g_stop)
		{
			// Get current hand outputs
			if(g_handDataOutput->Update() == PXC_STATUS_NO_ERROR)
			{
				// Display alerts
				if(g_alerts)
				{
					PXCHandData::AlertData alertData;
					for(int i = 0 ; i < g_handDataOutput->QueryFiredAlertsNumber() ; ++i)
					{
						if(g_handDataOutput->QueryFiredAlertData(i,alertData) == PXC_STATUS_NO_ERROR)
						{
							std::printf("%s was fired at frame %d \n",Definitions::AlertToString(alertData.label).c_str(),alertData.frameNumber);
						}
					}	
				}

				// Display gestures
				if(g_gestures)
				{
					PXCHandData::GestureData gestureData;
					for(int i = 0 ; i < g_handDataOutput->QueryFiredGesturesNumber() ; ++i)
					{
						if(g_handDataOutput->QueryFiredGestureData(i,gestureData) == PXC_STATUS_NO_ERROR)
						{
							std::wprintf(L"%s, Gesture: %s was fired at frame %d \n",Definitions::GestureStateToString(gestureData.state),gestureData.name,gestureData.frameNumber);
						}
					}	
				}

				// Display joints
				if(g_skeleton)
				{
					PXCHandData::IHand *hand;
					PXCHandData::JointData jointData;
					for(int i = 0 ; i < g_handDataOutput->QueryNumberOfHands() ; ++i)
					{
						g_handDataOutput->QueryHandData(PXCHandData::ACCESS_ORDER_BY_TIME,i,hand);
						std::string handSide = "Unknown Hand";
						handSide = hand->QueryBodySide() == PXCHandData::BODY_SIDE_LEFT ? "Left Hand" : "Right Hand";

						std::printf("%s\n==============\n",handSide.c_str());
						for(int j = 0 ; j < 22 ; ++j)
						{
							if(hand->QueryTrackedJoint((PXCHandData::JointType)j,jointData) == PXC_STATUS_NO_ERROR)
							{
								std::printf("     %s)\tX: %f, Y: %f, Z: %f \n",Definitions::JointToString((PXCHandData::JointType)j).c_str(),jointData.positionWorld.x,jointData.positionWorld.y,jointData.positionWorld.z);
							}
						}
					}
				}

				// Display number of hands
				if(g_default)
				{
					if(numOfHands != g_handDataOutput->QueryNumberOfHands())
					{
						numOfHands = g_handDataOutput->QueryNumberOfHands();
						std::printf("Number of hands: %d\n",numOfHands);
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
	if(g_senseManager)
	{
		g_senseManager->Close();
		g_senseManager->Release();
		g_senseManager = NULL;
	}
	if(g_session)
	{
		g_session->Release();
		g_session = NULL;
	}*/
}

