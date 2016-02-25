#ifndef PARSER_HEADER
#define PARSER_HEADER

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <utility> // std::pair
#include <fstream>
#include <iomanip>
#include <sstream>
#include "include\rapidjson\document.h"
#include "include\rapidjson\writer.h"
#include "include\rapidjson\filereadstream.h"
#include "include\rapidjson\filewritestream.h"
#include "include\rapidjson\stringbuffer.h"


using std::cout;
using namespace rapidjson;
using namespace std;


class Parser{
	private :
		const char* path;
		string long_to_hex(long l);

	public :
		// PARSER CONSTRUCTOR
		// param : str = name of the file to parse
		Parser(char * str);

		// READJSONFILE
		// returns all the vectors in a vector of (vector of pairs), produced by the parsing
		vector< vector< pair<string, long> > > ReadJsonFile ();

		//WRITEJSONFILE
		// creates a json file from a vector of (vector of pairs)
		void WriteJsonFile(vector <vector <pair <string, long> > > vect);

		// INSERTWORDINJSON
		// inserts pairs of <string, long> that make a word in a json file
		void InsertWordInJson(vector<pair<string, long>> vect, fstream *outputFile);
	};

#endif