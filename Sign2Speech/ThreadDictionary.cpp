#include "ThreadDictionary.h"
#include "ThreadHandTools.h"

ThreadDictionary::ThreadDictionary(mutex* mP, mutex *mBR, mutex *mBW, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, Sign2Speech *w) : ThreadApp(mP, mBR, mBW, pg, bR, bW) {
	win = w;
}

void ThreadDictionary::run() {
	//Initialisation Dico-------------------------------------------------------------------------
	Dictionary d;

	Parser p("examples/example.json");

	// vector of vector used to store all the pairs
	vector< vector< pair<string, long> > > res;

	// parse the json file
	win->appendText("Parsing Json file...");
	res = p.ReadJsonFile();
	win->appendText("Parsing > OK");
	//
	// insert all the vectors of pairs in the dictionary
	for (vector<vector<pair<string, long>>>::iterator it = res.begin(); it != res.end(); ++it) {
		d.insertList((*it));
	}

	time_t start = time(0);

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

		double seconds_since_start = difftime(time(0), start);
		if (seconds_since_start > 5) { //TIMEOUT
			string currentSymbol = d.refreshDictionary();
			cout << "refresh dico" << endl;
			if ((currentSymbol != "") && (currentSymbol != "0x1 : racine")) {
				ThreadHandTools::webSock->send("{\"content\":\""+ currentSymbol +"\"}");
				if (ThreadHandTools::webSock->getReadyState() != WebSocket::CLOSED) {
					ThreadHandTools::webSock->poll();
					ThreadHandTools::webSock->dispatch(ThreadHandTools::handle_message);
				}
			}
			start = time(0);
		}

		mBufferR->lock();
		if (bufferRead->size() != 0) { //Get CurrentSymbol
			vector<long>::iterator it = bufferRead->begin();
			string currentSymbol = d.read(*it);
			//cout << "Reading : " << (*it) << " Signification : " << currentSymbol << endl;
			string out = "Reading : " + (*it);
			out += " Signification : " + currentSymbol;
			win->appendText((char*)out.c_str());
			if ((currentSymbol != "0x0 : Not final word") && (currentSymbol != "0x1 : racine")) {
				ThreadHandTools::webSock->send("{\"content\":\"" + currentSymbol + "\"}");
				if (ThreadHandTools::webSock->getReadyState() != WebSocket::CLOSED) {
					ThreadHandTools::webSock->poll();
					ThreadHandTools::webSock->dispatch(ThreadHandTools::handle_message);
				}
			}
			bufferRead->erase(it);
			start = time(0);
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