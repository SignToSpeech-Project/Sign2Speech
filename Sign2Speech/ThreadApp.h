#pragma once
#include <mutex>
#include "stdafx.h"

using namespace std;

class ThreadApp {

protected:
	mutex*mBufferR;
	mutex * mBufferW;
	mutex* mSymbolSent;
	bool* symbolSent;
	bool* program_on;
	vector<long>* bufferRead;
	vector<vector<pair<string, long>>>* bufferWrite;

public :
	ThreadApp(mutex *mBR, mutex *mBW, mutex* mSS, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, bool *ss);
	virtual void run() = 0 ;
};