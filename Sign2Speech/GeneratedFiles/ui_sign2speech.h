/********************************************************************************
** Form generated from reading UI file 'sign2speech.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGN2SPEECH_H
#define UI_SIGN2SPEECH_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Sign2SpeechClass
{
public:
    QWidget *centralWidget;
    QOpenGLWidget *openGLWidget;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *plainTextEdit;
    QPushButton *pushButton;
    QMenuBar *menuBar;

    void setupUi(QMainWindow *Sign2SpeechClass)
    {
        if (Sign2SpeechClass->objectName().isEmpty())
            Sign2SpeechClass->setObjectName(QStringLiteral("Sign2SpeechClass"));
        Sign2SpeechClass->resize(839, 604);
        centralWidget = new QWidget(Sign2SpeechClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        openGLWidget = new QOpenGLWidget(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(130, 10, 551, 341));
        widget = new QWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 360, 821, 211));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        plainTextEdit = new QPlainTextEdit(widget);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);

        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout->addWidget(pushButton);

        Sign2SpeechClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Sign2SpeechClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 839, 21));
        Sign2SpeechClass->setMenuBar(menuBar);

        retranslateUi(Sign2SpeechClass);

        QMetaObject::connectSlotsByName(Sign2SpeechClass);
    } // setupUi

    void retranslateUi(QMainWindow *Sign2SpeechClass)
    {
        Sign2SpeechClass->setWindowTitle(QApplication::translate("Sign2SpeechClass", "Sign2Speech", 0));
        pushButton->setText(QApplication::translate("Sign2SpeechClass", "PushButton", 0));
    } // retranslateUi

};

namespace Ui {
    class Sign2SpeechClass: public Ui_Sign2SpeechClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGN2SPEECH_H
