#include "ThreadApp.h"

ThreadApp::ThreadApp(mutex* mP, mutex *mBR, mutex *mBW, mutex *mSW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW) {
	this->mBufferR = mBR;
	this->mBufferW = mBW;
	this->mProgram_on = mP;
	this->mStdW = mSW;
	this->program_on = pg;
	this->bufferRead = bR;
	this->bufferWrite = bW;
}