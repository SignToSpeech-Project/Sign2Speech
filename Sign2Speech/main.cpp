#include "sign2speech.h"
#include <QtWidgets/QApplication>

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
void threadDico(Sign2Speech *win) {
	ThreadDictionary d(&mProgram_on, &mBufferR, &mBufferW, &program_on, &bufferRead, &bufferWrite, win);
	d.run();
}


//Thread managing the camera and the gestures recognization
void threadHandTool(int argc, char* argv[], Sign2Speech *win) {
	ThreadHandTools d(&mProgram_on, &mBufferR, &mBufferW, &program_on, &bufferRead, &bufferWrite, argc, argv, win);
	d.run();
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Sign2Speech w;
	w.show();
	
	std::thread tHandTools(threadHandTool, argc, argv, &w);
	std::thread tDico(threadDico, &w);

	bool attendre = false;

	a.exec();

	tHandTools.join();
	tDico.join();
}
