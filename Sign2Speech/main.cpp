#include "sign2speech.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Sign2Speech w;
	w.show();
	w.startThreads(argc, argv);
	//w.bindProgramState(&mProgram_on, &program_on);
	
	bool attendre = false;
	a.exec();
}
