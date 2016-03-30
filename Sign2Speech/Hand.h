#pragma once

#include "stdafx.h"

using namespace std;
using std::cout;

class Hand
{

private:

	PXCHandData::FingerData f1;
	PXCHandData::FingerData f2;
	PXCHandData::FingerData f3;
	PXCHandData::FingerData f4;
	PXCHandData::FingerData f5;

public:

	Hand();
	Hand(PXCHandData::IHand *hand);

	PXCHandData::FingerData getF1() {
		return f1;
	}
	PXCHandData::FingerData getF2() {
		return f2;
	}
	PXCHandData::FingerData getF3() {
		return f3;
	}
	PXCHandData::FingerData getF4() {
		return f4;
	}
	PXCHandData::FingerData getF5() {
		return f5;
	}

	void setF1(PXCHandData::FingerData newF1) {
		f1 = newF1;
	}
	void setF2(PXCHandData::FingerData newF2) {
		f2 = newF2;
	}
	void setF3(PXCHandData::FingerData newF3) {
		f3 = newF3;
	}
	void setF4(PXCHandData::FingerData newF4) {
		f4 = newF4;
	}
	void setF5(PXCHandData::FingerData newF5) {
		f5 = newF5;
	}

	~Hand();
};

