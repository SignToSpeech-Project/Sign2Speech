#include "stdafx.h"
#include <thread>
#include <mutex>
#include <stdio.h>
#include "Dictionary.h"
#include "ThreadDictionary.h"
#include "ThreadHandTools.h"

#include "wingetopt.h"


bool program_on = true; // =off : End of the program

std::mutex mProgram_on;
std::mutex mBufferR; //Buffer of which symbols the user is currently doing (Default mod of the program)
std::mutex mBufferW; //Buffer of symbols chain you need to add to the dictionary ( Learning mod ON)

vector<long> bufferRead;
vector<vector<pair<string, long>>> bufferWrite;

char *address = "localhost:9000";
char *room = "test";


bool waitForClosure = true;

//Thread managing the Dictionary
void threadDico() {
	ThreadDictionary d(&mProgram_on, &mBufferR, &mBufferW, &program_on, &bufferRead, &bufferWrite);
	d.run();
}


//Thread managing the camera and the gestures recognization
void threadHandTool() {
	ThreadHandTools d(&mProgram_on, &mBufferR, &mBufferW, &program_on, &bufferRead, &bufferWrite, address, room);
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
		while (waitForClosure);
		return true;

	default:
		return FALSE;
	}
}

void howToUse() {
	printf("Sign2Speech.exe - Run the acquisition software\n");
	printf("\t-a\tDefine the host address otherwise use default address: \"localhost:9000\"\n");
	printf("\t\tThe address should be defined as: IPv4_Address[:port]\n");
	printf("\t-d\tRun the software in debug mode\n");
	printf("\t-h\tRun help\n");
	printf("\t-r\tDefine the room name otherwise use default  room name: \"test\"\n");
}

bool runCommandParameters(int argc, char** argv) {
	int c;
	int errflg = 0;

	//Debugger::chandle = GetStdHandle(STD_OUTPUT_HANDLE);
	while ((c = getopt(argc, argv, "a:dhr:")) != -1){
		switch (c) {
		case 'a':
			address = optarg;
			break;
		case 'd':
			// run in debug mode
			Debugger::DEBUG_MODE = true;
			break;
		case 'h':
			errflg++;
			break;
		case 'r':
			room = optarg;
			break;
		case '?':
			errflg++;
			break;
		}
	}
	if (errflg) {
		howToUse();
		return false;
	}
	return true;
}

void main(int argc, char** argv)
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	bool valid = runCommandParameters(argc, argv);

	if (valid) {
		std::thread tHandTools(threadHandTool);
		std::thread tDico(threadDico);

		tHandTools.join();
		tDico.join();

		waitForClosure = false;
	}
}



