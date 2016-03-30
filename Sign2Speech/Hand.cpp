#include "Hand.h"

Hand::Hand() {

}

Hand::Hand(PXCHandData::IHand *hand)
{
	PXCHandData::FingerData fingerData;
	for (int f = 0; f < 5; f++) {
		if (hand->QueryFingerData((PXCHandData::FingerType)f, fingerData) == PXC_STATUS_NO_ERROR) {
			// fill the hand struct
			switch (f) {
			case 1:
				f1 = fingerData;
				break;
			case 2:
				f2 = fingerData;
				break;
			case 3:
				f3 = fingerData;
				break;
			case 4:
				f4 = fingerData;
				break;
			case 5:
				f5 = fingerData;
				break;
			default:
				break;
			}
		}
	}
}



Hand::~Hand()
{
}
