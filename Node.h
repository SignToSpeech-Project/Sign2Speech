#pragma once
#include <vector>
#include <string>

using std::vector;
using std::string;

//Key : symbol. Value : word
class Node
{


private:

	vector <Node*> childrens;
	long symbol;
	string word;

public:

	long getSymbol();
	string getWord();

	//Add to the Node childrens list the Node n
	//If there is already a symbol s in the childre list, it just update his linked word
	Node* addNode(string w, long s);

	//Remove the first children with symbol s from Node  childrens list. Return true if the remove was sucessfull, otherwise false
	bool removeNode(long s);

	//Return true if the symbol s is present at least one time in the Node childrens list. Else return false
	bool isPresent(long s);

	//Return the children of this node with the symbol s. Otherwise return NULL
	Node* getChildren(long s);

	//Constructor of Node. w is the word linked to the symbol s
	Node(string w, long s);
	~Node();
};

