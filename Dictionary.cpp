#include "Dictionary.h"
#include "Node.h"


Dictionary::Dictionary()
{
	racine = new Node("racine", 0);
	currentNode = racine;
	currentNodeCreation = racine;
}

//Return the word of the currentNode
string Dictionary::getWordCurrentNode() {
	return currentNode->getWord();
}

//Insert in the dictionary the symbol linked to a word as a children of the currentNode. You don't need to check if it's already in the children of the currentNode
Node* Dictionary::insert(string word, long symbol) {
	return currentNodeCreation->addNode(word,symbol);
}

//Creation from the racine of the tree of the list of symbol/pair you sent
void Dictionary::insertList(vector<pair<string, long>> v) {
	for (vector<pair<string, long>>::iterator it = v.begin(); it != v.end(); ++it)
	{
		currentNodeCreation = insert((*it).first, (*it).second);
	}
	currentNodeCreation = racine;
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
		return "retourne vide";
	}
	else {
		string word = currentNode->getWord();
		currentNode = racine;
		n = currentNode->getChildren(symbol); //To not lost the current symbol
		if (n != NULL) currentNode = n;
		return word;
	}
}


//Put the currentNode on the racine and return previous currentNode word value.
string Dictionary::refreshDictionary() {
	string word = currentNode->getWord();
	currentNode = racine;
	return word;
}

Dictionary::~Dictionary()
{
}
