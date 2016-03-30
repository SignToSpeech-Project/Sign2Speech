#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

class Debugger {

public: 
	static int DEBUG_MODE;

	static void debug(string m) {
		if (DEBUG_MODE) {
			cout << m << endl;
		}
	}

	static void info(string m) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
		cout << m << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
	}

	static void error(string m) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C);
		cout << m << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
	}
};