#include "stdafx.h"
#include <thread>
#include <mutex>
#include <stdio.h>
#include "Dictionary.h"
#include "ThreadDictionary.h"
#include "ThreadHandTools.h"


bool program_on = true; // =off : End of the program

std::mutex mProgram_on;
std::mutex mBufferR; //Buffer of which symbols the user is currently doing (Default mod of the program)
std::mutex mBufferW; //Buffer of symbols chain you need to add to the dictionary ( Learning mod ON)

vector<long> bufferRead;
vector<vector<pair<string, long>>> bufferWrite;


bool attendre = true;

//Thread managing the Dictionary
void threadDico() {
	ThreadDictionary d(&mProgram_on, &mBufferR, &mBufferW, &program_on, &bufferRead, &bufferWrite);
	d.run();
}


//Thread managing the camera and the gestures recognization
void threadHandTool(int argc, char** argv) {
	ThreadHandTools d(&mProgram_on, &mBufferR, &mBufferW, &program_on, &bufferRead, &bufferWrite, argc, argv);
	d.run();
}

bool CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:

		// confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		mProgram_on.lock();
		program_on = false;
		mProgram_on.unlock();
		while (attendre);
		return true;

	default:
		return FALSE;
	}
}

void main(int argc, char** argv)
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
	std::thread tHandTools(threadHandTool, argc, argv);
	std::thread tDico(threadDico);

	tHandTools.join();
	tDico.join();

	bool attendre = false;

}



