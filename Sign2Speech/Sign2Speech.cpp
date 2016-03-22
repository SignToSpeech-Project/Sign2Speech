#include "sign2speech.h"

Sign2Speech::Sign2Speech(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

Sign2Speech::~Sign2Speech()
{

}

void Sign2Speech::closeEvent(QCloseEvent *event) {
	mProgram_on->lock();
	*program_on = false;
	mProgram_on->unlock();
	event->accept();
}

void Sign2Speech::bindProgramState(mutex *mP, bool *pg) {
	mProgram_on = mP;
	program_on = pg;
}

void Sign2Speech::appendText(QString text) {
	ui.plainTextEdit->appendPlainText(text);
}
