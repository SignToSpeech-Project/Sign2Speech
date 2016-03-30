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
		printf("Connected to the WebSocket server (52.35.210.217)\n");
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
			printf("Failed Creating PXCSession\n");
			return;
		}

		ct.setSenseManager(ct.getSession()->CreateSenseManager());
		if (!(ct.getSenseManager()))
		{
			ct.releaseAll(); //TODO : Fonction dans HandTools.cpp . Attention, il faut lui passer des paramètres maintenant. A discuter
			printf("Failed Creating PXCSenseManager\n");
			return;
		}

		if ((ct.getSenseManager())->EnableHand() != PXC_STATUS_NO_ERROR)
		{
			ct.releaseAll();
			printf("Failed Enabling Hand Module\n");
			return;
		}

		g_handModule = (ct.getSenseManager())->QueryHand();
		if (!g_handModule)
		{
			ct.releaseAll();
			printf("Failed Creating PXCHandModule\n");
			return;
		}

		g_handDataOutput = g_handModule->CreateOutput();
		if (!g_handDataOutput)
		{
			ct.releaseAll();
			printf("Failed Creating PXCHandData\n");
			return;
		}

		g_handConfiguration = g_handModule->CreateActiveConfiguration();
		if (!g_handConfiguration)
		{
			ct.releaseAll();
			printf("Failed Creating PXCHandConfiguration\n");
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
			printf("\nPXCSenseManager Initializing OK\n========================\n");

			mProgram_on->lock();

			int currentGesture = 0;
			int firstFrame = 0;
			int currentGestComposee = 0;
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
								// 1) tableau de taille 3 contenant les vecteurs contenant les données obtenues par les 3 répétitions d'un même geste
								vector<Hand> repeatedGesture[3];
								// 2) le vecteur contenant la moyenne, frame par frame, du geste (de taille minNbFrame)
								vector<Hand> averageGesture;
								// 3) le vecteur de taille fournie par l'utilisateur (nbGestes) qui contient les différents gestes intermediaires
								//		et qui recomposent le geste final
								vector<Hand> completeGesture;

								if (firstFrame == 0) {
									start = time(0);
									//Reintialiser le vecteur de mouvement
								}
								//sauvegarder le geste dans un vecteur de mouvement

								if (difftime(start, time(0)) >= 3) {
									currentGesture++;
									firstFrame = 0;
									//sauvegarder le vecteur de mouvement dans le vecteur contenant les 3 gestes
								}


								if (currentGesture == 3) {
									//moyenne des 3 gestes du vecteur contenant les 3 gestes
									//Si c'est un geste static :sauvegarde la moyenne du geste composé dans unvecteur de geste composee
									//si c'est un geste dynamique : tu calcula la trajectoire aussi avant de sauvegarder le geste composé et sa trajectoire
									currentGesture = 0;
									//cout confirmation
									currentGestComposee++;
									//Si currentGestComposee == nbGestePRevu
										//Sauvegarder les gestes composees
										//currentGestComposee = 0;
										//learning = false;
								}
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
			printf("Failed Initializing PXCSenseManager\n");
			return;
		}
	}
	else {
		printf("Unable to connect to the WebSocket server (52.35.210.217)\n");
	}

	ct.releaseAll();

	delete ThreadHandTools::webSock;
#ifdef _WIN32
	WSACleanup();
#endif
}