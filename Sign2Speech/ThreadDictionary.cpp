#include "ThreadDictionary.h"
#include "ThreadHandTools.h"

ThreadDictionary::ThreadDictionary(mutex *mBR, mutex *mBW, mutex *mSS, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, bool *sS) : ThreadApp(mBR, mBW, mSS, pg, bR, bW, sS) {
}

void ThreadDictionary::run() {
	//Initialisation Dico-------------------------------------------------------------------------
	Dictionary d;

	Parser p("output.json");

	// vector of vector used to store all the pairs
	vector< vector< pair<string, long> > > res;

	// parse the json file
	Debugger::debug("Parsing Json file...\n");
	res = p.ReadJsonFile();
	Debugger::debug("Parsing > OK");

	// insert all the vectors of pairs in the dictionary
	for (vector<vector<pair<string, long>>>::iterator it = res.begin(); it != res.end(); ++it) {
		d.insertList((*it));
	}

	time_t start = time(0);

	while (*program_on) {



		if (bufferWrite->size() != 0) { //Update Dico
			mBufferW->lock();
			vector<vector<pair<string, long>>>::iterator it = bufferWrite->begin(); 
			d.insertList((*it));
			bufferWrite->erase(it);
			mBufferW->unlock();
			
		}


		double seconds_since_start = difftime(time(0), start);
		if (seconds_since_start > 2) { //TIMEOUT
			string currentSymbol = d.refreshDictionary();
			//Debugger::debug("Refresh dictionary");
			if ((currentSymbol != "") && (currentSymbol != "0x1 : racine")) {
				Debugger::info("Sending: " + currentSymbol);
				ThreadHandTools::webSock->send("{\"content\":\""+ currentSymbol +"\"}");
				mSymbolSent->lock();
				*symbolSent = true;
				mSymbolSent->unlock();
				if (ThreadHandTools::webSock->getReadyState() != WebSocket::CLOSED) {
					ThreadHandTools::webSock->poll();
					ThreadHandTools::webSock->dispatch(ThreadHandTools::handle_message);
				}
			}
			start = time(0);
		}


		if (bufferRead->size() != 0) { //Get CurrentSymbol
			mBufferR->lock();
			vector<long>::iterator it = bufferRead->begin();
			string currentSymbol = d.read(*it);
			std:stringstream out;
			out << "Reading : " << (*it) << " Signification : " << currentSymbol << endl;
			Debugger::debug(out.str());
			if ((currentSymbol.find("0x0 : Not final word") == std::string::npos) && (currentSymbol != "0x1 : racine") && (currentSymbol != "")) {
				Debugger::info("Sending: " + currentSymbol);
				ThreadHandTools::webSock->send("{\"content\":\"" + currentSymbol + "\"}");
				mSymbolSent->lock();
				*symbolSent = true;
				mSymbolSent->unlock();
				if (ThreadHandTools::webSock->getReadyState() != WebSocket::CLOSED) {
					ThreadHandTools::webSock->poll();
					ThreadHandTools::webSock->dispatch(ThreadHandTools::handle_message);
				}
			}
			bufferRead->erase(it);
			start = time(0);
			mBufferR->unlock();
		}



	}


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