#include "ThreadDictionary.h"

ThreadDictionary::ThreadDictionary(mutex* mP, mutex *mBR, mutex *mBW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW) : ThreadApp(mP,mBR,mBW,pg,bR,bW) {}

void ThreadDictionary::run() {
	//Initialisation Dico-------------------------------------------------------------------------
	Dictionary d;

	Parser p("examples/example.json");

	// vector of vector used to store all the pairs
	vector< vector< pair<string, long> > > res;

	// parse the json file
	cout << "Parsing Json file..." << endl;
	res = p.ReadJsonFile();
	cout << "Parsing > OK" << endl;
	cout << "" << endl;
	//
	// insert all the vectors of pairs in the dictionary
	for (vector<vector<pair<string, long>>>::iterator it = res.begin(); it != res.end(); ++it) {
		d.insertList((*it));
	}

	mProgram_on->lock();
	while (*program_on) {
		mProgram_on->unlock();


		mBufferW->lock();
		if (bufferWrite->size() != 0) { //Update Dico
			for (vector<vector<pair<string, long>>>::iterator it = bufferWrite->begin(); it != bufferWrite->end(); ++it) {
				d.insertList((*it));
				bufferWrite->erase(it);
			}
		}
		mBufferW->unlock();

		mBufferR->lock();
		//TODO : Reset dico au bout d'un timeOut
		if (bufferRead->size() != 0) { //Get CurrentSymbol
			for (vector<long>::iterator it = bufferRead->begin(); it != bufferRead->end(); ++it) {
				string currentSymbol = d.read(*it);
				if (currentSymbol != "0x0 : Not final word") {
					//TODO : Affichage à l'écran du mot correspondant
				}
				bufferRead->erase(it);
			}
		}
		mBufferR->unlock();

		mProgram_on->lock();
	}
	mProgram_on->unlock();

	//Sauvegarde Dico-------------------------------------------------------------------------

	//cout << "Vector Dictionary creation..." << endl;
	vector<vector<pair<string, long>>> v = d.createVectorDictionary(); // Create vector of vectors of pairs from the dict
	//cout << "Vector Dictionary > OK" << endl;
	//cout << "" << endl;
	//cout << "Writting Json file from vect..." << endl;
	p.WriteJsonFile(v);
	//cout << "Json file > OK" << endl;
	//cout << "" << endl;

}