#pragma once

#include "ThreadApp.h"
#include "ConsoleTools.h"
#include "stdafx.h"
#include "easywsclient.hpp"


using easywsclient::WebSocket;

class ThreadHandTools : public ThreadApp {
private :
	PXCHandModule *g_handModule;
	PXCHandData *g_handDataOutput;
	PXCHandConfiguration *g_handConfiguration;

	bool g_skeleton = false; // Writing skeleton data (22 joints) to console ouput
	bool g_stop = false; // user closes application

	bool learning = false; // learning mode or not
	string nomMotCompose;
	int nbMotCompose;
	long lastSymbolRead = -1;

	vector<pair<string, long>> learningGest;

	char *address;
	char *room;
public:
	ThreadHandTools(mutex* mP, mutex *mBR, mutex *mBW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, char *ad, char *r, bool l, string nmc, int nbmc);

	static WebSocket::pointer webSock;

	void static handle_message(const std::string & message);

	void run();	
};