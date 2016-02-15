#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <utility> // std::pair

using std::cout;
using namespace rapidjson;
using namespace std;	


class Parser{
	private :
		const char* path;
	public :

		Parser(char * str);
		vector< vector< pair<string, long> > > ReadJsonFile ();
	};