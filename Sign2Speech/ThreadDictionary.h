#pragma once
#include "ThreadApp.h"
#include "stdafx.h"
#include <Time.h>
#include "sign2speech.h"


class ThreadDictionary : public ThreadApp {

private:

	Sign2Speech *win;

public:
	ThreadDictionary(mutex* mP, mutex *mBR, mutex *mBW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, Sign2Speech *w);

	void run();
};