#pragma once
#include <vector>
#include <string>

using std::vector;
using std::string;

#define SEUIL 8

//Key : symbol. Value : word
class Node
{


private:

	vector <Node*> children;
	long symbol;
	string word;

	//Return the amount of difference of the fingers between 2 symbols following the function : 10 * eg + 3 * em + es + nbdif;
	int scoreGeasture(long s1, long s2); 

public:

	long getSymbol();
	string getWord();

	//Add to the Node children list the Node n
	//If there is already a symbol s in the children list, it just update his linked word
	Node* addNode(string w, long s);

	//Remove the first child with symbol s from Node children list. Return true if the remove was sucessfull, otherwise false
	bool removeNode(long s);

	//Return true if the symbol s is present at least one time in the Node children list. Else return false
	bool isPresent(long s);

	//Return the child of this node with the symbol s. Otherwise return NULL
	Node* getChild(long s);

	//Retun all children of the current Node
	vector <Node*> getChildren();

	//Return true if the current nod has at least one child. Otherwise, return false
	bool hasChild();
	
	//Constructor of Node. w is the word linked to the symbol s
	Node(string w, long s);
	~Node();
};

