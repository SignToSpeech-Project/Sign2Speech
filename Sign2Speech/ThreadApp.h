#pragma once
#include <mutex>
#include "stdafx.h"

using namespace std;

//General thread for the Application. 
//Starting with the run function

class ThreadApp {

protected:

	mutex*mBufferR; //Mutex for the Reading buffer
	mutex * mBufferW; //Mutex for the Writting buffer
	mutex* mSymbolSent; //Mutex for the boolean SymbolSent 
	bool* symbolSent; //To know if the Dictionnary sent a word on WebRTC
	bool* program_on; //To stop the thread
	vector<long>* bufferRead; //Buffer of words that will be sent on WebRTC
	vector<vector<pair<string, long>>>* bufferWrite; //Buffer of words that need to be added in the Dictionnary

public :
	ThreadApp(mutex *mBR, mutex *mBW, mutex* mSS, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, bool *ss);
	
	//Start the thread
	virtual void run() = 0 ;
};