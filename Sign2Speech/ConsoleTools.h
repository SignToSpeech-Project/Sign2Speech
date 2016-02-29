#pragma once
class ConsoleTools
{

private:
	PXCSession *g_session;
	PXCSenseManager *g_senseManager;

public:
	ConsoleTools();
	~ConsoleTools();
	void releaseAll();
	//bool CtrlHandler(DWORD fdwCtrlType, bool g_stop);

	PXCSession* getSession() { return g_session; }
	PXCSenseManager* getSenseManager(){ return g_senseManager; }

	void setSession(PXCSession* p) { this->g_session = p; }
	void setSenseManager(PXCSenseManager* sm) { this->g_senseManager = sm; }

};

