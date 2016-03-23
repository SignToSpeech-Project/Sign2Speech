#include <algorithm>

#include "HandTools.h"
#include "ThreadHandTools.h"


HandTools::HandTools(Sign2Speech *w, mutex *mSW) {
	win = w;
	mStdW = mSW;
}

void HandTools::printBinary(uint32_t a, int nbBits) {
	for (int b = nbBits; b >= 0; b--) {
		printf("%d", (a >> b) & 0x1);
	}
}

void HandTools::writeMessage(QString string) {
	mStdW->lock();
	//win->appendText(string);
	mStdW->unlock();
}

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

vector<uint32_t> HandTools::removeOutValues(vector<uint32_t> v) {
	//Median 
	std::sort(v.begin(), v.end());
	int t = v.size();
	float median;
	if ((t % 2) == 0) {
		int rank1 = (t / 2) - 1;
		int rank2 = t / 2;
		median = (v.at(rank1) + v.at(rank2)) / 2.0;
	}
	else {
		int rank = t / 2;
		median = v.at(rank);
	}

	//quartet1 and quartet3
	float quartile1;
	if ((t % 4) == 0) {
		int rank1 = (t / 4.0) - 1.0;
		int rank2 = t / 4.0;
		quartile1 = (v.at(rank1) + v.at(rank2)) / 2.0;
	}
	else {
		int rank = t / 4.0;
		quartile1 = v.at(rank);
	}

	float quartile3;
	if ((t % 4) == 0) {
		int rank1 = (12 / (4.0 / 3.0)) - 1;
		int rank2 = 12 / (4.0 / 3.0);
		quartile3 = (v.at(rank1) + v.at(rank2)) / 2.0;
	}
	else {
		int rank = t / (4.0 / 3.0);
		quartile3 = v.at(rank);
	}

	//Interquartile range
	float  interquartileRange = quartile3 - quartile1;

	//inner fences
	float innerFenceMin = quartile1 - 1.5 * interquartileRange;
	float innerFenceSup = quartile3 + 1.5 * interquartileRange;

	//outer fences
	float outerFenceMin = quartile1 - 3 * interquartileRange;
	float outerFenceSup = quartile3 + 3 * interquartileRange;

	vector<uint32_t> result;

	//removing strange values : use outerFence or innerFence, your wish. At this time we use outer fences
	for (vector<uint32_t>::iterator it = v.begin(); it != v.end(); ++it)
	{
		if ((outerFenceMin <= (*it)) && ((*it) <= outerFenceSup))	result.push_back((*it));
	}

	return result;
}



uint32_t HandTools::calculateAverage(PXCHandData::FingerData handData[MAXFRAME][5]) {
	uint32_t avg = 0x0;

	// calculate the gesture average
	for (int f = 0; f < 5; f++) {
		uint32_t sumFold = 0;
		vector<uint32_t> v;
		for (int i = 0; i < MAXFRAME; i++) {
			v.push_back(handData[i][f].foldedness);
		}

		vector<uint32_t> vResult = this->removeOutValues(v);

		for (vector<uint32_t>::iterator it = vResult.begin(); it != vResult.end(); ++it) {
			sumFold += (*it);
		}
		sumFold /= vResult.size();

		if (sumFold < 50) {
			if (sumFold < 25) {
				avg |= (0b00 << (2 * f));
			}
			else {
				avg |= (0b01 << (2 * f));
			}
		}
		else if (sumFold >= 50) {
			if (sumFold < 75) {
				avg |= (0b10 << (2 * f));
			}
			else {
				avg |= (0b11 << (2 * f));
			}
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
		// add the coordinates of the mass center into the table
		massCenterCoordinates[*nbReadFrame] = hand->QueryMassCenterWorld();
		(*nbReadFrame)++;
	}
	else {
		*nbReadFrame = 0;

		uint32_t average = calculateAverage(handData);
		uint8_t movement = analyseMovement();
		average |= movement << 10;

		printf("[%ld]\t", frameCounter);
		printBinary(average, 17);
		printf("\n");
		//std::printf("\n");

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
			//std::printf("     %s)\tFoldedness: %d, Radius: %f \n", Definitions::FingerToString((PXCHandData::FingerType)f).c_str(), fingerData.foldedness, fingerData.radius);
			printf("     %s)\tFoldedness: %d, Radius: %f \n", Definitions::FingerToString((PXCHandData::FingerType)f).c_str(), fingerData.foldedness, fingerData.radius);
		}
	}
}


/* Analyse the movement of the gesture (straight, circular or elliptic) from all the points that compose the gesture */
uint8_t HandTools::analyseMovement() {

	// coordinates of the first point of the movement
	PXCPoint3DF32 p0 = massCenterCoordinates[0];

	// coordinates of the middle point of the movement
	PXCPoint3DF32 pm = massCenterCoordinates[(int)(MAXFRAME/2)];

	// coordinates of the last point of the movement
	PXCPoint3DF32 pf = massCenterCoordinates[MAXFRAME - 1];

	uint8_t temp = 0b0 ;

	if (isStatic(&temp)) {
		return temp;
	}
	else if (isStraight(p0, pm, pf, &temp)) {
		// do things with temp and symb
		return temp;
	}
	else if (isElliptic(p0, pm, pf, &temp)) {
		// do things with temp and symb
		return temp;
	}
	return 0;
}

bool HandTools::isStatic(uint8_t *out) {
	int i;
	int cpt = 0;
	PXCPoint3DF32 p0 = massCenterCoordinates[0];
	PXCPoint3DF32 p_current;

	for (i = 1; i < MAXFRAME; i++) {
		p_current = massCenterCoordinates[i];
		if ((abs(p0.x - p_current.x) <= NBMETERS_STATIC) && (abs(p0.y - p_current.y) <= NBMETERS_STATIC)) {
			cpt++;
		}
		p0 = p_current;
	}
	//cout << cpt << endl;
	printf(cpt + "\n");
	if (cpt > (float)MAXFRAME*0.92) {
		//cout << "STATIQUE" << endl;
		printf("STATIQUE\n");
		return true;
	}
	else {
		return false;
	}
}

// horizontal movement detected
bool HandTools::isHorizontal(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf) {
	if ((abs(p0.x - pf.x) > VALID_HOR) && (abs(p0.y - pf.y) <= ERR_VERT)) {
		//cout << "STRAIGHT HORIZONTAL" << endl;
		printf("STRAIGHT HORIZONTAL\n");
		return true;
	}
	return false;
}

// vertical movement detected
bool HandTools::isVertical(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf) {
	if ((abs(p0.y - pf.y) > VALID_VER) && (abs(p0.x - pf.x) <= ERR_HOR)) {
		//cout << "STRAIGHT VERTICAL" << endl;
		printf("STRAIGHT VERTICAL\n");
		return true;
	}
	return false;
}

// straight (line) movement detected
bool HandTools::isStraight(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf, uint8_t *out) {

	// y = ax + b ; a is the slope, b is the intercept
	float a, b;
	if ((pf.x - p0.x) == 0) {
		a = 0.0;
	}
	else {
		a = (pf.y - p0.y) / (pf.x - p0.x);
	}
	b = p0.y - a*p0.x;

	// does it respect the equation of a straight line & is it horizontal?
	if (abs(pm.y - (a*pm.x + b)) < ERR_STRAIGHT_HOR && isHorizontal(p0, pm, pf)) {
		// which direction: left or right?
		if (p0.x - pf.x > 0) {
			//cout << "    RIGHT" << endl;
			printf("\tRIGHT\n");
			*out |= 0b1;
			printBinary(*out, 7);
			//std::printf("\n");
			printf("\n");
			return true;
		}
		else {
			//cout << "    LEFT" << endl;
			printf("\tLEFT\n");
			*out |= (0b1 << 1);
			printBinary(*out, 7);
			//std::printf("\n");
			printf("\n");
			return true;
		}
	}
	else if (abs(pm.y - (a*pm.x + b)) < ERR_STRAIGHT_VER && isVertical(p0, pm, pf)) {
		// which direction: top or bottom?
		if (p0.y - pf.y > 0) {
			//cout << "    BOTTOM" << endl;
			printf("\tBOTTOM\n");
			*out |= (0b1 << 3);
			printBinary(*out, 7);
			//std::printf("\n");
			printf("\n");
			return true;
		}
		else {
			//cout << "    TOP" << endl;
			printf("\tTOP\n");
			*out |= (0b1 << 2);
			printBinary(*out, 7);
			//std::printf("\n");
			printf("\n");
			return true;
		}
	}
	// not horizontal nor vertical: it's a normal straight line!
	else if (abs(pm.y - (a*pm.x + b)) < ERR_STRAIGHT) {
		// if p0.y < pf.y -> to the top
		if (p0.y < pf.y) {
			*out |= (0b1 << 2);
			if (p0.x - pf.x > 0) {
				//cout << "\t\tNORMAL TOP RIGHT" << endl;
				printf("\tNORMAL TOP RIGHT\n");
				*out |= 0b1;
				printBinary(*out, 7);
				//std::printf("\n");
				printf("\n");
				return true;
			}
			else {
				//cout << "\t\tNORMAL TOP LEFT" << endl;
				printf("\tNORMAL TOP LEFT\n");
				*out |= (0b1 << 1);
				printBinary(*out, 7);
				//std::printf("\n");
				printf("\n");
				return true;
			}
		}
		// if p0.y > pf.y -> to the bottom
		else if (p0.y > pf.y) {
			*out |= (0b1 << 3);
			if (p0.x - pf.x > 0) {
				//cout << "\t\tNORMAL BOTTOM RIGHT" << endl;
				printf("\tNORMAL BOTTOM RIGHT\n");
				*out |= 0b1;
				printBinary(*out, 7);
				//std::printf("\n");
				printf("\n");
				return true;
			}
			else {
				//cout << "\t\tNORMAL BOTTOM LEFT" << endl;
				printf("\tNORMAL BOTTOM LEFT\n");
				*out |= (0b1 << 1);
				printBinary(*out, 7);
				//std::printf("\n");
				printf("\n");
				return true;
			}
		}
	} // end if "it respects the equation of a straight line"
	else {
		return false;
	}
}
bool HandTools::isElliptic(PXCPoint3DF32 p0, PXCPoint3DF32 pm, PXCPoint3DF32 pf, uint8_t *out) {
	// center coordinates pc(xc, yc)
	PXCPoint3DF32 pc;

	// NOT FULL ELLIPSE if the distance between first and last point > NBMETERS_FULLELLIPSE
	if (sqrt(pow(pf.x - p0.x, 2) + pow(pf.y - p0.y, 2)) > NBMETERS_FULLELLIPSE) {
		pc.x = (pf.x + p0.x) / 2.0;
		pc.y = (pf.y + p0.y) / 2.0;

		// a is the distance between pf(pf.x, pf.y) and pc(pc.x, pc.y)
		float a = sqrt(pow(pf.x - pc.x, 2) + pow(pf.x - pc.y, 2));
		// b is the distance between pm(pm.x, pm.y) and pc(pc.x, pc.y)
		float b = sqrt(pow(pm.x - pc.x, 2) + pow(pm.y - pc.y, 2));

		// chose 1 point, different from p0, pm and pf, to see if it respects the ellipse eq
		PXCPoint3DF32 p1 = massCenterCoordinates[(int)(MAXFRAME / 3)];
		if ((abs(pow(p1.x - pc.x, 2) / (a*a) + pow(p1.y - pc.y, 2) / (b*b)) - 1) <= ERR_ELLIPSE) {
			//cout << "NOT FULL ELLIPSE" << endl;
			printf("NOT FULL ELLIPSE\n");
			*out |= (0b1 << 7);
			return true;
		}
		else {
			return false;
		}
	}

	// FULL ELLIPSE if distance between first and last point <= NBMETERS_FULLECLIPSE
	else if (sqrt(pow(pf.x - p0.x, 2) + pow(pf.y - p0.y, 2)) <= NBMETERS_FULLELLIPSE) {
		// pm is the point in the middle of the point list. Therefore, it is p0's symetrical to the center 
		pc.x = (pm.x + p0.x) / 2.0;
		pc.y = (pm.y + p0.y) / 2.0;

		// a is the distance between pf(pf.x, pf.y) and pc(pc.x, pc.y)
		float a = sqrt(pow(pf.x - pc.x, 2) + pow(pf.x - pc.y, 2));
		
		// pm2 is the point at the first quarter of the point list (the middle of the first half of the list)
		PXCPoint3DF32 pm2 = massCenterCoordinates[(int)(MAXFRAME / 4)];

		// b is the distance between pm2(pm2.x, pm2.y) and pc(pc.x, pc.y)
		float b = sqrt(pow(pm2.x - pc.x, 2) + pow(pm2.y - pc.y, 2));

		// chose 2 points, different from p0, pm, pm2 and pf, to see if they respect the ellipse eq
		PXCPoint3DF32 p1 = massCenterCoordinates[(int)(MAXFRAME / 3)];
		PXCPoint3DF32 p2 = massCenterCoordinates[(int)(2*MAXFRAME / 3)];
		if ( ((abs(pow(p1.x - pc.x, 2) / (a*a) + pow(p1.y - pc.y, 2) / (b*b)) - 1) < ERR_ELLIPSE)
		&& ((abs(pow(p2.x - pc.x, 2) / (a*a) + pow(p2.y - pc.y, 2) / (b*b)) - 1) < ERR_ELLIPSE) ) {
			//cout << "FULL ELLIPSE" << endl;
			printf("FULL ELLIPSE\n");
			*out |= (0b1 << 6);
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}