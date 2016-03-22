#ifndef SIGN2SPEECH_H
#define SIGN2SPEECH_H

#include <QtWidgets/QMainWindow>
#include <mutex>
#include "ui_sign2speech.h"

using namespace std;

class Sign2Speech : public QMainWindow
{
	Q_OBJECT

public:
	Sign2Speech(QWidget *parent = 0);
	~Sign2Speech();

	void appendText(QString text);
	void closeEvent(QCloseEvent *event);
	void bindProgramState(mutex *mP, bool *pg);
private:
	Ui::Sign2SpeechClass ui;

	mutex *mProgram_on;
	bool *program_on;
};

#endif // SIGN2SPEECH_H
