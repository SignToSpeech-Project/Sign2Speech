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

void ParseInputStream(FILE* src, Document d){
	char readBuffer[65536];
	StringBuffer buffer;

	// parse input stream
	FileReadStream inputStream (src, readBuffer, sizeof(readBuffer));
	d.ParseStream(inputStream);

	fclose(src);

	// stringify the DOM
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    // print the output
	cout << buffer.GetString() << endl;
}

int main(){

	using namespace rapidjson;
	using namespace std;

	FILE* src = fopen("example.json", "rb");
	Document d, docTmp;
	//char readBuffer[65536];
	// StringBuffer buffer;
	int nbWords = 0;
	int i = 0;
	int j;
	string word = "";
	long gesture;
	string sGesture = "";

	/*// parse input stream
	FileReadStream inputStream (src, readBuffer, sizeof(readBuffer));
	d.ParseStream(inputStream);

	fclose(src);

	// stringify the DOM
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    // print the output
	cout << buffer.GetString() << endl;*/

	ParseInputStream(src, d);

	// count total number of words
	for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr){
		nbWords++;
	}
	cout << "Total number of words in the dictionary: " << nbWords << endl;

	vector< vector< pair<string, long> > > allVectors(nbWords); // used to store all the vectors (one vector of pairs for each word)
	
	for(Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr){
		cout << itr->name.GetString() << endl;

		// get saucisson / maison
		const Value& curWord = d[itr->name.GetString()];

		// create an array of pairs for the current word
		pair<string, long> pairs[curWord.Size()];
		vector< pair<string, long> > vect;
		
		//cout << "\t" << kTypeNames[curWord.GetType()] << endl;
		//cout << "\t\t" << kTypeNames[curWord[0].GetType()] << endl;

		// iterate through "array of objects"
		for (SizeType j = 0; j < curWord.Size(); j++){
			// get current json object
			const Value& c = curWord[j];
			for(Value::ConstMemberIterator itr1 = c.MemberBegin(); itr1 != c.MemberEnd(); ++itr1){
				// if the value is "word"
				if(strcmp(itr1->name.GetString(), "word") == 0){
					word = c[itr1->name.GetString()].GetString();
					cout << "\t\t\t" << itr1->name.GetString() << " " << word << endl;

				}
				// else : the value is  "gesture"
				else if (strcmp(itr1->name.GetString(), "gesture") == 0){
					sGesture = c[itr1->name.GetString()].GetString();
					gesture = strtol(sGesture.c_str(), nullptr, 0);
					cout << "\t\t\t" << itr1->name.GetString() << " " << gesture << endl;
				}
			}
			// create the pair
			pairs[j] = pair<string, long>(word, gesture);
			cout << "\t\t\t > Current pair: " << pairs[j].first << ", " << pairs[j].second << endl;
			vect.push_back(pairs[j]);

			cout << vect[j].second << endl;
		}
		cout << "curWord size : " << curWord.Size() << endl;

		// add the current vector to AllVectors
		allVectors.push_back(vect);
	}

	return 0;
}