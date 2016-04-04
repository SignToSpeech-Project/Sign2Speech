#include "ThreadApp.h"

ThreadApp::ThreadApp(mutex *mBR, mutex *mBW, mutex *mSS, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, bool *ss) {
	this->mBufferR = mBR;
	this->mBufferW = mBW;
	this->mSymbolSent = mSS;
	this->symbolSent = ss,
	this->program_on = pg;
	this->bufferRead = bR;
	this->bufferWrite = bW;
}