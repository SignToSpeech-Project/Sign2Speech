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

	long lastSymbolRead = -1;

	int argc;
	const char** argv;
public:
	ThreadHandTools(mutex* mP, mutex *mBR, mutex *mBW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, int ac, const char* av[]);

	static WebSocket::pointer webSock;

	bool static CtrlHandler(DWORD fdwCtrlType, bool g_stop, ConsoleTools *ct);
	void static handle_message(const std::string & message);

	void run();	
};