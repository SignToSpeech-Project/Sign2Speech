#pragma once
#include "Node.h"
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::pair;

class Dictionary
{

private:

	Node * racine;
	Node * currentNode;
	Node* currentNodeCreation;

public:

	//Insert in the dictionary the symbol linked to a word as a children of the currentNode. You don't need to check if it's already in the children of the currentNode
	Node* insert(string word, long symbol);

	//Creation from the racine of the tree of the list of symbol/pair you sent
	void insertList(vector<pair<string, long>> v);

	//Remove if it is possible the children of the currentNode with the symbol symbol. Return true if the remove was successful, otherwhise false
	bool remove(long symbol);

	//Return true if the children of the currentNode with the symbol symbol exist. Otherwhise, false
	bool isPresent(long symbol);

	//Update currentNode if it is posssible and return "". Otherwhise, reset currentNode (currentNode = racine) and return the previous currentNode word value
	string read(long symbol);

	//Return the word of the currentNode
	string getWordCurrentNode();

	//Put the currentNode on the racine and return previous currentNode word value.
	string refreshDictionary();

	Dictionary();
	~Dictionary();
};

