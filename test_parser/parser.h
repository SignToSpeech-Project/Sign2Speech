#include "include\rapidjson\document.h"
#include "include\rapidjson\writer.h"
#include "include\rapidjson\filereadstream.h"
#include "include\rapidjson\filewritestream.h"
#include "include\rapidjson\stringbuffer.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <utility> // std::pair

using std::cout;
using namespace rapidjson;
using namespace std;

class Parser{

	public:

	int nbWords;
	
	Parser();
	static int getNbWords(Document d);
};