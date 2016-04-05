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

//Start the thread
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

	if (ThreadHandTools::webSock != NULL) {
		Debugger::info("Connected to the WebSocket server " + webSocketAddress.str());

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

			int firstFrame = 0;




			time_t start;
			long lastSymbolRead = -1; //Used to know what was the last gesture the user did



			bool learning = false; //learning mode or not
			int cpt_Gesture = 1; //Used on learning mode to know which gesture we are doing at this time
			string nameCompoundWord; //Used on learning mode to know the name of the word the user wants to add			
			int nbCompoundWord; //Used on learning mode to know how many gestures need to be done to complete the user word
			vector<long> completeGesture; //Used on learning mode : Vector of the serie of gestures the user did

			Debugger::info("Do you want to add words to the Dictionary ? y/n");
			string answer;
			cin >> answer;
			if ((answer == "y") || (answer == "Y")) {

				learning = true;
				int nbmc;
				string nmc;
				Debugger::info("What is the meaning of your word ?");
				cin >> nmc;
				Debugger::info("How many gestures are required for this word ?");
				cin >> nbmc;

				nbCompoundWord = nbmc;
				nameCompoundWord = nmc;

				h.learningMode(nbmc);

				Debugger::info("Learning mode is starting in 5 secondes, be ready !");
				for (int i = 5; i > 0; i--) {
					Debugger::info(to_string(i));
					Sleep(1000);
				}
				Debugger::info("------------------------PLEASE, DO YOUR FIRST GESTURE IN 3 SECONDES------------------------");

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

						//RECOGNITION MODE-------------------------------------------------------------------------------
						if (learning == false) {
							long symbol = h.analyseGesture(hand);
							if ((symbol != -1) && (symbol != lastSymbolRead)) {
								mBufferR->lock();
								bufferRead->push_back(symbol);
								mBufferR->unlock();
								lastSymbolRead = symbol;  //Allow the user to keep for example his "fist" gesture during some seconds without changing the dictionary reading level
							}
						}

						//LEARNING MODE---------------------------------------------------------------------------------
						else if (learning == true) {
							long symbol = h.analyseXGestures(hand);
							if (symbol != -1) {
								if (!h.getLearning()) {
									learning = false;
									pair<string, long> temp(nameCompoundWord, symbol);
									learningGesture.push_back(temp);
									mBufferW->lock();
									bufferWrite->push_back(learningGesture);
									mBufferW->unlock();
									learningGesture.clear();

									Debugger::info("------------------------WORD SAVED------------------------");
									Sleep(2000);
									Debugger::info("Do you want to add a new word to the Dictionary ? y/n");
									string answer;
									cin >> answer;
									if ((answer == "y") || (answer == "Y")) {

										learning = true;
										int nbmc;
										string nmc;
										Debugger::info("What is the meaning of your word ?");
										cin >> nmc;
										Debugger::info("How many gestures are required for this word ?");
										cin >> nbmc;

										nbCompoundWord = nbmc;
										nameCompoundWord = nmc;

										h.learningMode(nbmc);

									}
									else Debugger::info("------------------------SWITCHING ON RECOGNITION MODE------------------------");
								}
								else {
									pair<string, long> temp("", symbol);
									learningGesture.push_back(temp);
									cpt_Gesture++;
									string msg = "------------------------BE READY FOR THE GESTURE NUMERO " + to_string(cpt_Gesture) + " IN 5 SECONDES------------------------";
									Debugger::info(msg);
									for (int i = 5; i > 0; i--) {
										Debugger::info(to_string(i));
										Sleep(1000);
									}
									Debugger::info("------------------------Do your gesture NOW during 3 secondes------------------------");
								}
							}

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