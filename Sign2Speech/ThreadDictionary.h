#pragma once
#include "ThreadApp.h"
#include "stdafx.h"
#include <Time.h>


class ThreadDictionary : public ThreadApp {
public:
	ThreadDictionary(mutex *mBR, mutex *mBW, mutex *mSS, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, bool *sS);

	//Start the thread
	void run();
};