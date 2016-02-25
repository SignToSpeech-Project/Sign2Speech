

#include "include\rapidjson\document.h"
#include "include\rapidjson\writer.h"
#include "include\rapidjson\filereadstream.h"
#include "include\rapidjson\filewritestream.h"
#include "include\rapidjson\stringbuffer.h"
#include "stdafx.h"


Parser::Parser(char * str) {
	this->path = str;
}

string Parser::long_to_hex(long l){
	stringstream sstream;
	sstream << "0x"
		<< std::setfill('0') << std::setw(sizeof(long) * 2)
		<< std::hex << l;
	return sstream.str();
}

vector< vector< pair<string, long> > > Parser::ReadJsonFile (){

	FILE* src = fopen(path, "rb");
	Document d, docTmp;
	char readBuffer[65536];
	StringBuffer buffer;
	int nbWords = 0;
	int i = 0;
	int size = 0;
	string word = "";
	long gesture;
	string sGesture = "";

	// parse input stream
	FileReadStream inputStream (src, readBuffer, sizeof(readBuffer));
	d.ParseStream(inputStream);

	fclose(src);

	// stringify the DOM
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    // print the output
	//cout << buffer.GetString() << endl;

	// count total number of words
	for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr){
		nbWords++;
	}
	//cout << "Total number of words in the dictionary: " << nbWords << endl;

	vector< vector< pair<string, long> > > allVectors(nbWords); // used to store all the vectors (one vector of pairs for each word)
	
	for(Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr){
		//cout << itr->name.GetString() << endl;

		// get saucisson / maison
		const Value& curWord = d[itr->name.GetString()];

		// create an array of pairs for the current word
		size = curWord.Size();
		vector< pair<string, long> > vect;

		// iterate through "array of objects"
		for (SizeType j = 0; j < curWord.Size(); j++){
			// get current json object
			const Value& c = curWord[j];
			for(Value::ConstMemberIterator itr1 = c.MemberBegin(); itr1 != c.MemberEnd(); ++itr1){
				// if the value is "word"
				if(strcmp(itr1->name.GetString(), "word") == 0){
					word = c[itr1->name.GetString()].GetString();
					//cout << "\t\t\t" << itr1->name.GetString() << " " << word << endl;

				}
				// else : the value is  "gesture"
				else if (strcmp(itr1->name.GetString(), "gesture") == 0){
					sGesture = c[itr1->name.GetString()].GetString();
					gesture = strtol(sGesture.c_str(), nullptr, 0);
					//cout << "\t\t\t" << itr1->name.GetString() << " " << gesture << endl;
				}
			}
			// create the pair
			//pairs[j] = pair<string, long>(word, gesture);
			pair<string, long> tmp = pair<string, long>(word, gesture);
			vect.push_back(tmp);
			//cout << "\t\t\t > Current pair: " << tmp.first << ", " << tmp.second << endl;
		}
		//cout << "curWord size : " << curWord.Size() << endl;

		// add the current vector to AllVectors
		allVectors.push_back(vect);
	}

	return allVectors;
}

void Parser::InsertWordInJson(vector<pair<string, long>> vec, fstream *file) {
	int size, i;
	string curWord;
	string gesture;

	// check if the file is open
	if (!(*file).is_open()) {
		perror("InsertWordInJson: The file is not open\n");
		return;
	}

	// write current word which is in the last object of the vector
	size = vec.size();
	curWord = vec.at(size - 1).first;
	*file << "\t\"" << curWord << "\": [" << endl;

	// iterate through the pairs of a word 
	for (i = 0; i < size; ++i) {
		// get "word"
		*file << "\t\t{" << endl;
		*file << "\t\t\t\"word\": ";
		*file << "\"" << vec.at(i).first << "\"," << endl;

		// get "gesture"
		*file << "\t\t\t\"gesture\": ";
		gesture = long_to_hex(vec.at(i).second);
		*file << "\"" << gesture << "\"" << endl;

		// print the comma only if it is the last pair
		if (i == size - 1) {
			*file << "\t\t}" << endl;
		}
		else {
			*file << "\t\t}," << endl;
		}
	}
}

void Parser::WriteJsonFile(vector <vector <pair <string, long> > > vect) {
	vector<vector<pair<string, long>>>::iterator it;
	vector < pair<string, long>>::iterator it2;
	fstream outputFile;
	int vectSize;
	int i = 0;

	// create the output json file if it doesn't exist
	outputFile.open("output.json", fstream::out);

	outputFile << "{" << endl;

	vectSize = vect.size();

	// iterating through the "big" vector
	for (it = vect.begin(); it != vect.end(); ++it) {
		// insert all the words and their gestures
		InsertWordInJson((*it), &outputFile);

		// print the comma only if it is not the last object 
		if (i == vectSize - 1) {
			outputFile << "\t]" << endl;
		}
		else {
			outputFile << "\t]," << endl;
		}
		i++;
	}

	outputFile << "}" << endl;
	outputFile.close();
}