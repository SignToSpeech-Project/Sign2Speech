#include "Dictionary.h"
#include "Node.h"
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::pair;

class Dictionary {
public :
	Node* insert(string word, long symbol);

	void insertList(vector<pair<string,long>> v);

	bool remove(long symbol);

	bool isPresent(long symbol);

	string read(long symbol);

	string getWordCurrentNode();

	Dictionary();

	~Dictionary();

private :
	Node * racine;
	Node * currentNode;
};

Dictionary::Dictionary()
{
	racine = new Node("", 0);
	currentNode = racine;
}

//Return the word of the currentNode
string Dictionary::getWordCurrentNode() {
	return currentNode->getWord();
}

//Insert in the dictionary the symbol linked to a word as a children of the currentNode. You don't need to check if it's already in the children of the currentNode
Node* Dictionary::insert(string word, long symbol) {
	return currentNode->addNode(word,symbol);
}

//Creation from the racine of the tree of the list of symbol/pair you sent
void Dictionary::insertList(vector<pair<string, long>> v) {

	Node* currentNodeCreation = racine;
	for (vector<pair<string, long>>::iterator it = v.begin(); it != v.end(); ++it)
	{
		currentNodeCreation = insert((*it).first, (*it).second);
	}
}

//Remove if it is possible the children of the currentNode with the symbol symbol. Return true if the remove was successful, otherwhise false
bool Dictionary::remove(long symbol) {
	return currentNode->removeNode(symbol);
}

//Return true if the children of the currentNode with the symbol symbol exist. Otherwhise, false
bool Dictionary::isPresent(long symbol) {
	return currentNode->isPresent(symbol);
}

//Update currentNode if it is posssible and return "". Otherwhise, reset currentNode (currentNode = racine) and return the previous currentNode word value
string Dictionary::read(long symbol) {
	Node* n = currentNode->getChildren(symbol);
	if (n != NULL) {
		currentNode = n;
		return "";
	}
	else {
		string word = currentNode->getWord();
		currentNode = racine;
		return word;
	}
}

Dictionary::~Dictionary()
{
}
