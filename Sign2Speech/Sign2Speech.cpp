#include "sign2speech.h"

Sign2Speech::Sign2Speech(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

Sign2Speech::~Sign2Speech()
{

}

void Sign2Speech::appendText(char *text) {
	ui.plainTextEdit->appendPlainText(text);
}
