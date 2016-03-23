#include "sign2speech.h"

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
std::mutex mStdW;

std::thread tHandTools;
std::thread tDico;

vector<long> bufferRead;
vector<vector<pair<string, long>>> bufferWrite;


bool attendre = true;

//Thread managing the Dictionary
void threadDico(Sign2Speech *win) {
	ThreadDictionary d(&mProgram_on, &mBufferR, &mBufferW, &mStdW, &program_on, &bufferRead, &bufferWrite, win);
	d.run();
}


//Thread managing the camera and the gestures recognization
void threadHandTool(int argc, char* argv[], Sign2Speech *win) {
	ThreadHandTools d(&mProgram_on, &mBufferR, &mBufferW, &mStdW, &program_on, &bufferRead, &bufferWrite, argc, argv, win);
	d.run();
}

Sign2Speech::Sign2Speech(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.plainTextEdit->setReadOnly(true);
}

Sign2Speech::~Sign2Speech()
{

}

void Sign2Speech::closeEvent(QCloseEvent *event) {
	mProgram_on.lock();
	program_on = false;
	mProgram_on.unlock();

	tHandTools.join();
	tDico.join();

	event->accept();
}

void Sign2Speech::startThreads(int argc, char **argv) {
	tHandTools = std::thread(threadHandTool, argc, argv, this);
	tDico = std::thread(threadDico, this);
}

void Sign2Speech::appendText(QString string) {
	//ui.plainTextEdit->insertPlainText(string);
}
