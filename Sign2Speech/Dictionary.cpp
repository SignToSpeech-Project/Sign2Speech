#include "Node.h"
#include "stdafx.h"

Dictionary::Dictionary()
{
	root = new Node("0x1 : root", 0);
	currentNode = root;
	currentNodeCreation = root;
}

//Return the word of the currentNode
string Dictionary::getWordCurrentNode() {
	return currentNode->getWord();
}

//Insert in the dictionary the symbol linked to a word as a children of the currentNode. You don't need to check if it's already in the children of the currentNode
Node* Dictionary::insert(string word, long symbol) {
	return currentNodeCreation->addNode(word, symbol);
}

//Creation from the racine of the tree of the list of symbol/pair you sent
void Dictionary::insertList(vector<pair<string, long>> v) {
	for (vector<pair<string, long>>::iterator it = v.begin(); it != v.end(); ++it)
	{
		currentNodeCreation = insert((*it).first, (*it).second);
	}
	currentNodeCreation = root;
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
	Node* n = currentNode->getChild(symbol);
	if (n != NULL) {
		if (n->hasChild()) {
			currentNode = n;
			return "0x0 : Not final word, signification temp : "+ n->getWord();
		}
		else { //If the current word is a final word
			string word = n->getWord();
			currentNode =root;
			return word;
		}
	}
	else {
		string word = currentNode->getWord();
		currentNode = root;
		n = currentNode->getChild(symbol); //To not lost the current symbol
		if (n != NULL) {
			if(n->hasChild()) currentNode = n;
			else { //If the next symbol has a meaning and is a final word, instead of waiting next call to return the final word, it return "old word + new word"
				string temp = n->getWord();
				if (temp != "") word += " " + temp;
				currentNode = root;
			}
		}
		return word;
	}
}


//Put the currentNode on the racine and return previous currentNode word value.
string Dictionary::refreshDictionary() {
	string word = currentNode->getWord();
	currentNode = root;
	return word;
}

//Return the vector of vector of pairs of the current Dictionnary
vector<vector<pair<string, long>>> Dictionary::createVectorDictionary() {

	vector<vector<pair<string, long>>> v;

	vector<pair<string, long>> v2;
	vector<Node *>::iterator it;
	vector<Node *> vtemp = root->getChildren();
	for (it = vtemp.begin(); it != vtemp.end(); ++it) {
		vectorDictionary(&v, v2, (*it));
	}
	return v;
}

//Private function used for createVectorDictionary(). Used for recursivity
void Dictionary::vectorDictionary(vector<vector<pair<string, long>>>* v, vector<pair<string, long>> v2, Node* n) {
	if (!(n->hasChild())) {
		//ajouter à v2 les info du n actuel et les push back dans v
		pair<string, long> p(n->getWord(), n->getSymbol());
		v2.push_back(p);
		v->push_back(v2);
	}
	else {
		pair<string, long> p(n->getWord(), n->getSymbol());
		v2.push_back(p);
		vector<Node *> vtemp = n->getChildren();
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
