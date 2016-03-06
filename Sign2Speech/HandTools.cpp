#include "HandTools.h"
#include "ThreadHandTools.h"



int HandTools::calculateHammingDistance(uint32_t a, uint32_t b, int nBit, int step) {
	int dist = 0;
	uint32_t one = (~(0b0)) >> (32 - step);
	for (int i = 0; i < nBit / step; i++) {
		if (((a >> (step * i)) & one) != ((b >> (step * i)) & one)) {
			dist++;
		}
	}
	return dist;
}

boolean HandTools::isGesture(uint32_t gesture, uint32_t ref, int distMax, int maxApproximateFinger) {
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

uint32_t HandTools::calculateAverage(PXCHandData::FingerData handData[MAXFRAME][5]) {
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

long HandTools::analyseGesture(PXCHandData::IHand *hand) {
	// increment frame number
	frameCounter++;

	PXCHandData::BodySideType side = hand->QueryBodySide();
	int *nbReadFrame;
	PXCHandData::FingerData(*handData)[5];
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

		uint32_t average = calculateAverage(handData);

		std::printf("[%ld]\t", frameCounter);
		for (int b = 9; b >= 0; b--) {
			std::printf("%d", (average >> b) & 0x1);
		}
		std::printf("\n");

		/*if (isGesture(average, fist, 1, 2)) {
			std::printf("[%ld]\t\t %s FIST\n", frameCounter, sideStr.c_str());
			ThreadHandTools::webSock->send("{\"content\":\"Fist\"}");
			if (ThreadHandTools::webSock->getReadyState() != WebSocket::CLOSED) {
				ThreadHandTools::webSock->poll();
				ThreadHandTools::webSock->dispatch(ThreadHandTools::handle_message);
			}
		}

		if (isGesture(average, victory, 1, 2)) {
			std::printf("[%ld]\t\t %s VICTORY\n", frameCounter, sideStr.c_str());
			ThreadHandTools::webSock->send("{\"content\":\"Victory\"}");
			if (ThreadHandTools::webSock->getReadyState() != WebSocket::CLOSED) {
				ThreadHandTools::webSock->poll();
				ThreadHandTools::webSock->dispatch(ThreadHandTools::handle_message);
			}
		}

		if (isGesture(average, metal, 1, 2)) {
			std::printf("[%ld]\t\t %s METAL\n", frameCounter, sideStr.c_str());
			ThreadHandTools::webSock->send("{\"content\":\"Metal\"}");
			if (ThreadHandTools::webSock->getReadyState() != WebSocket::CLOSED) {
				ThreadHandTools::webSock->poll();
				ThreadHandTools::webSock->dispatch(ThreadHandTools::handle_message);
			}
		}*/
		//TODO : déplacer les envois au serveur/application de sous titrage dans les TODO affichage de la classe threaddictionary
		return average;
	}
	return -1;
}

void HandTools::printFold(PXCHandData::IHand *hand) {
	PXCHandData::FingerData fingerData;
	for (int f = 0; f < 5; f++) {
		if (hand->QueryFingerData((PXCHandData::FingerType)f, fingerData) == PXC_STATUS_NO_ERROR) {
			std::printf("     %s)\tFoldedness: %d, Radius: %f \n", Definitions::FingerToString((PXCHandData::FingerType)f).c_str(), fingerData.foldedness, fingerData.radius);
		}
	}
}
/*
bool HandTools::CtrlHandler(DWORD fdwCtrlType, bool g_stop, PXCSenseManager *g_senseManager, PXCSession *g_session)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:

		// confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		g_stop = true;
		Sleep(1000);
		releaseAll(g_senseManager, g_session);
		return(TRUE);

	default:
		return FALSE;
	}
}

void HandTools::releaseAll(PXCSenseManager *g_senseManager, PXCSession *g_session)
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
}*/