#include "ThreadApp.h"

ThreadApp::ThreadApp(mutex* mP, mutex *mBR, mutex *mBW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW) {
	this->mBufferR = mBR;
	this->mBufferW = mBW;
	this->mProgram_on = mP;
	this->program_on = pg;
	this->bufferRead = bR;
	this->bufferWrite = bW;
}