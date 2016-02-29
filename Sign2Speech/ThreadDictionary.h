#pragma once
#include "ThreadApp.h"
#include "stdafx.h"


class ThreadDictionary : public ThreadApp {
public:
	ThreadDictionary(mutex* mP, mutex *mBR, mutex *mBW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW);

	void run();
};