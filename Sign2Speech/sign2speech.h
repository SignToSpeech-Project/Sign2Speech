#ifndef SIGN2SPEECH_H
#define SIGN2SPEECH_H

#include <QtWidgets/QMainWindow>
#include "ui_sign2speech.h"

class Sign2Speech : public QMainWindow
{
	Q_OBJECT

public:
	Sign2Speech(QWidget *parent = 0);
	~Sign2Speech();

	void appendText(char *text);
private:
	Ui::Sign2SpeechClass ui;
};

#endif // SIGN2SPEECH_H
