#pragma once
#include <mutex>
#include "stdafx.h"

using namespace std;

class ThreadApp {

protected:
	mutex* mProgram_on;
	mutex*mBufferR;
	mutex * mBufferW;
	static bool* program_on;
	vector<long>* bufferRead;
	vector<vector<pair<string, long>>>* bufferWrite;

public :
	ThreadApp(mutex* mP, mutex *mBR, mutex *mBW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW);
	virtual void run() = 0 ;
};