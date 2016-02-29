#include "stdafx.h"
#include "ConsoleTools.h"

void ConsoleTools::releaseAll()
{
	if (g_senseManager)
	{
		g_senseManager->Close();
		g_senseManager->Release();
		g_senseManager = NULL;
	}
	if (g_session)
	{
		g_session->Release();
		g_session = NULL;
	}
}


ConsoleTools::~ConsoleTools()
{
}
