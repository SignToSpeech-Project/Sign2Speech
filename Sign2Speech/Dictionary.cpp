#include "Node.h"
#include "stdafx.h"

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

//Update currentNode if it is posssible and return "0x0 : Not final word". Otherwhise, reset currentNode (currentNode = racine) and return the previous currentNode word value
string Dictionary::read(long symbol) {
	Node* n = currentNode->getChildren(symbol);
	if (n != NULL) {
		currentNode = n;
		return "0x0 : Not final word";
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

//Return the vector of vector of pairs of the current Dictionnary
vector<vector<pair<string, long>>> Dictionary::createVectorDictionary() {
	
	vector<vector<pair<string, long>>> v;

	vector<pair<string, long>> v2;
	vector<Node *>::iterator it;
	vector<Node *> vtemp = racine->getChildrens();
	for ( it = vtemp.begin(); it != vtemp.end(); ++it) {
		vectorDictionary(&v, v2, (*it)); 
	}
	return v;
}

//Private function used for createVectorDictionary(). Used for recursivity
void Dictionary::vectorDictionary(vector<vector<pair<string, long>>>* v, vector<pair<string, long>> v2, Node* n) {
	if (!(n->hasChildrens())) {
		//ajouter à v2 les info du n actuel et les push back dans v
		pair<string, long> p(n->getWord(), n->getSymbol());
		v2.push_back(p);
		v->push_back(v2);
	}
	else {
		pair<string, long> p(n->getWord(), n->getSymbol());
		v2.push_back(p);
		vector<Node *> vtemp = n->getChildrens();
		for (vector<Node *>::iterator it = vtemp.begin(); it != vtemp.end(); ++it)
		{
			//ajouter les infos de n au vecteur v2 et appeller vectorDictionnary sur le fils de n
			vectorDictionary(v, v2, (*it));
		}
	}
}

Dictionary::~Dictionary()
{
}
