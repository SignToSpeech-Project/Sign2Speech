#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <utility> // std::pair
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

	public :
		// PARSER CONSTRUCTOR
		// param : str = name of the file to parse
		Parser(char * str);

		// READJSONFILE
		// returns all the vectors in a vector of (vector of pairs), produced by the parsing
		vector< vector< pair<string, long> > > ReadJsonFile ();
	};