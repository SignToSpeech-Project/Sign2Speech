#pragma once
#include "ThreadApp.h"
#include "stdafx.h"
#include <Time.h>
#include "sign2speech.h"


class ThreadDictionary : public ThreadApp {

private:

	Sign2Speech *win;

	void writeMessage(QString string);
public:
	ThreadDictionary(mutex* mP, mutex *mBR, mutex *mBW, mutex *mSW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, Sign2Speech *w);

	void run();
};