#include "ThreadHandTools.h"

WebSocket::pointer ThreadHandTools::webSock = NULL;

ThreadHandTools::ThreadHandTools(mutex* mP, mutex *mBR, mutex *mBW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, char *ad, char *r) : ThreadApp(mP, mBR, mBW, pg, bR, bW) {
	address = ad;
	room = r;
}

void ThreadHandTools::handle_message(const std::string & message) {
	printf(">>> %s\n", message.c_str());
	//if (message == "world") {
	//	ThreadHandTools::webSock->close();
	//}
}

void ThreadHandTools::run() {
	
	ConsoleTools ct;

#ifdef _WIN32
	INT rc;
	WSADATA wsaData;

	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rc) {
		printf("WSAStartup Failed.\n");
		return;
	}
#endif
	std::stringstream webSocketAddress;
	webSocketAddress << "ws://" << address << "/ws/subtitle/" << room;
	
	ThreadHandTools::webSock = WebSocket::from_url(webSocketAddress.str());
	//ThreadHandTools::webSock = WebSocket::from_url("ws://52.35.210.217/ws/subtitle/test");

	if (ThreadHandTools::webSock != NULL) {
		Debugger::info("Connected to the WebSocket server " + webSocketAddress.str());
		//assert(ThreadHandTools::webSock); //TODO : enlever le commentaire

		HandTools h;

		/*if (argc < 2)
		{
			Definitions::WriteHelpMessage();
			return;
		}*/

		// Setup
		ct.setSession(PXCSession::CreateInstance());
		if (!(ct.getSession()))
		{
			Debugger::error("Failed Creating PXCSession");
			return;
		}

		ct.setSenseManager(ct.getSession()->CreateSenseManager());
		if (!(ct.getSenseManager()))
		{
			ct.releaseAll();
			Debugger::error("Failed Creating PXCSenseManager");
			return;
		}

		if ((ct.getSenseManager())->EnableHand() != PXC_STATUS_NO_ERROR)
		{
			ct.releaseAll();
			Debugger::error("Failed Enabling Hand Module");
			return;
		}

		g_handModule = (ct.getSenseManager())->QueryHand();
		if (!g_handModule)
		{
			ct.releaseAll();
			Debugger::error("Failed Creating PXCHandModule");
			return;
		}

		g_handDataOutput = g_handModule->CreateOutput();
		if (!g_handDataOutput)
		{
			ct.releaseAll();
			Debugger::error("Failed Creating PXCHandData");
			return;
		}

		g_handConfiguration = g_handModule->CreateActiveConfiguration();
		if (!g_handConfiguration)
		{
			ct.releaseAll();
			Debugger::error("Failed Creating PXCHandConfiguration");
			return;
		}

		// Iterating input parameters
		//for (int i = 1; i < argc; i++)
		//{
		//	if (strcmp(argv[i], "-skeleton") == 0)
		//	{
		//		//std::printf("-Skeleton Information Enabled-\n");
		//		g_skeleton = true;
		//	}
		//}


		// TODO remove it
		g_skeleton = true;

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
		if ((ct.getSenseManager())->Init() == PXC_STATUS_NO_ERROR)
		{
			Debugger::info("\nPXCSenseManager Initializing OK\n========================\n");

			mProgram_on->lock();

			int currentGesture = 0;
			int firstFrame = 0;
			int currentGestComposee = 0;
			// 1) tableau de taille 3 contenant les vecteurs contenant les données obtenues par les 3 répétitions d'un même geste
			vector<Hand> repeatedGesture[3];
			// 2) le vecteur contenant la moyenne, frame par frame, du geste (de taille minNbFrame)
			vector<Hand> averageGesture;
			// 3) le vecteur de taille fournie par l'utilisateur (nbGestes) qui contient les différents gestes intermediaires
			//		et qui recomposent le geste final
			vector<long> completeGesture;

			time_t start;

			// Acquiring frames from input device
			while ((ct.getSenseManager())->AcquireFrame(true) == PXC_STATUS_NO_ERROR && (*program_on))
			{
				mProgram_on->unlock();
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

							// normal recognition mode
							if (learning == false) {
								long symbol = h.analyseGesture(hand);
								if ((symbol != -1) && (symbol != lastSymbolRead)) {
									mBufferR->lock();
									bufferRead->push_back(symbol);
									mBufferR->unlock();
									lastSymbolRead = symbol;  //Allow the user to keep for example his "fist" gesture during some seconds without changing the dictionary reading level
								}
							}
							// learning mode
							else if (learning == true) {
								long symbol = h.analyseXGestures(hand);

								//learning = false;
							}
						}
					}

				} // end if update
				(ct.getSenseManager())->ReleaseFrame();
				mProgram_on->lock();
			} // end while acquire frame
			mProgram_on->unlock();
		} // end if Init

		else
		{
			ct.releaseAll();
			Debugger::error("Failed Initializing PXCSenseManager");
			return;
		}
	}
	else {
		Debugger::error("Unable to connect to the WebSocket server " + webSocketAddress.str());
	}

	ct.releaseAll();

	delete ThreadHandTools::webSock;
#ifdef _WIN32
	WSACleanup();
#endif
}