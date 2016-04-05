#include "ThreadDictionary.h"
#include "ThreadHandTools.h"

ThreadDictionary::ThreadDictionary(mutex *mBR, mutex *mBW, mutex *mSS, bool* pg, vector<long>* bR, vector<vector<pair<string, long>>>* bW, bool *sS) : ThreadApp(mBR, mBW, mSS, pg, bR, bW, sS) {
}

//Start the thread
void ThreadDictionary::run() {

	//Dictionary Initialisation-------------------------------------------------------------------------
	Dictionary d;

	Parser p("output.json");

	//Vector of vector used to store all the pairs
	vector< vector< pair<string, long> > > res;

	//parse the JSON file
	Debugger::debug("Parsing Json file...\n");
	res = p.ReadJsonFile();
	Debugger::debug("Parsing > OK");

	//Insert all vectors of pairs in the dictionary
	for (vector<vector<pair<string, long>>>::iterator it = res.begin(); it != res.end(); ++it) {
		d.insertList((*it));
	}

	time_t start = time(0);

	while (*program_on) {

		if (bufferWrite->size() != 0) { //If there is a word to add to the Dictionnary
			mBufferW->lock();
			vector<vector<pair<string, long>>>::iterator it = bufferWrite->begin(); 
			d.insertList((*it));
			bufferWrite->erase(it);
			mBufferW->unlock();
			
		}


		double seconds_since_start = difftime(time(0), start);

		if (seconds_since_start > 2) { //TIMEOUT to refresh Dictionary pointer node
			string currentSymbol = d.refreshDictionary();
			if ((currentSymbol != "") && (currentSymbol != "0x1 : root")) {
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


		if (bufferRead->size() != 0) { //If there is a word to send on WebRTC
			mBufferR->lock();
			vector<long>::iterator it = bufferRead->begin();
			string currentSymbol = d.read(*it);
			std:stringstream out;
			out << "Reading : " << (*it) << " Signification : " << currentSymbol << endl;
			Debugger::debug(out.str());
			if (!(currentSymbol.find("0x0 : Not final word") == std::string::npos) && (currentSymbol != "0x1 : root") && (currentSymbol != "")) {
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


	//Creating JSON File-------------------------------------------------------------------------

	vector<vector<pair<string, long>>> v = d.createVectorDictionary(); // Create vector of vectors of pairs from the dict

	p.WriteJsonFile(v);

}