#include "ThreadHandTools.h"

WebSocket::pointer ThreadHandTools::webSock = NULL;

ThreadHandTools::ThreadHandTools(mutex *mBR, mutex *mBW, mutex *mSS, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, char *ad, char *r, bool *sS) : ThreadApp(mBR, mBW, mSS, pg, bR, bW, sS) {
	address = ad;
	room = r;
}

void ThreadHandTools::handle_message(const std::string & message) {
	std::stringstream out;
	out << ">>> " << message;
	Debugger::debug(out.str());
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

			int currentGesture = 0;
			int firstFrame = 0;
			int currentGestComposee = 0;
			// 3) le vecteur de taille fournie par l'utilisateur (nbGestes) qui contient les différents gestes intermediaires
			//		et qui recomposent le geste final
			vector<long> completeGesture;

			time_t start;
			int cpt_Geasture = 1;

			Debugger::info("Voulez vous ajouter des mots au dictionnaire ? o/n");
			string answer;
			cin >> answer;
			if ((answer == "o") || (answer == "O")) {

				learning = true;
				int nbmc;
				string nmc;
				Debugger::info("Veuillez entrer la signification du geste :");
				cin >> nmc;
				Debugger::info("Veuillez entrer le nombre de gestes a faire :");
				cin >> nbmc;

				nbMotCompose = nbmc;
				nomMotCompose = nmc;

				h.learningMode(nbmc);

				Debugger::info("Le learning commence dans 5 secondes, preparez-vous : ");
				for (int i = 5; i > 0; i--) {
					Debugger::info(to_string(i));
					Sleep(1000);
				}
				Debugger::info("----------------------VEUILLEZ FAIRE VOTRE PREMIER GESTE DURANT 3 SECONDES----------------------");

			}

			// Acquiring frames from input device
			while ((ct.getSenseManager())->AcquireFrame(true) == PXC_STATUS_NO_ERROR && (*program_on))
			{
				if (*symbolSent) {
					Sleep(500);
					mSymbolSent->lock();
					*symbolSent = false;
					mSymbolSent->unlock();
				}

				// Get current hand outputs
				if (g_handDataOutput->Update() == PXC_STATUS_NO_ERROR)
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
							if (symbol != -1) {
								if (!h.getLearning()) {
									learning = false;
									pair<string, long> temp(nomMotCompose, symbol);
									learningGest.push_back(temp);
									mBufferW->lock();
									bufferWrite->push_back(learningGest);
									mBufferW->unlock();
									learningGest.clear();

									Debugger::info("------------------------MOT ENREGISTRE------------------------");
									Sleep(2000);
									Debugger::info("Voulez vous entrer un nouveau mot ? o/n");
									string answer;
									cin >> answer;
									if ((answer == "o") || (answer == "O")) {

										learning = true;
										int nbmc;
										string nmc;
										Debugger::info("Veuillez entrer la signification du geste :");
										cin >> nmc;
										Debugger::info("Veuillez entrer le nombre de gestes a faire :");
										cin >> nbmc;

										nbMotCompose = nbmc;
										nomMotCompose = nmc;

										h.learningMode(nbmc);

									}
									else Debugger::info("------------------------PASSAGE EN MODE RECONNAISSANCE DE GESTES------------------------");
								}
								else {
									pair<string, long> temp("", symbol);
									learningGest.push_back(temp);
									cpt_Geasture++;
									string msg = "------------------------PREPAREZ VOUS POUR VOTRE GESTE NUMERO " + to_string(cpt_Geasture) + " DANS 5 SECONDES------------------------";
									Debugger::info(msg);
									for (int i = 5; i > 0; i--) {
										Debugger::info(to_string(i));
										Sleep(1000);
									}
									Debugger::info("-------------------------------Faites votre geste MAINTENANT pendant 3 secondes-------------------------------");
								}
							}

							//learning = false;
						}
					}

				} // end if update
				(ct.getSenseManager())->ReleaseFrame();
			} // end while acquire frame
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