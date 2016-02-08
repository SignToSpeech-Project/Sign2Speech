#include "Node.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

//Key : symbol. Value : word
class Node {

private :
	vector <Node*> childrens;
	long symbol;
	string word;

public : 
	void addNode(string w, long s);

	bool removeNode(long s);

	bool isPresent(long s);

	Node(string w, long s);
	~Node();
};

//Add to the Node childrens list the Node n
//Do not check if the Node n is already present in the list.
void Node::addNode(string w, long s) {
	Node * n = new Node(w, s);
	childrens.push_back(n);

}

//Remove the first children with the symbol s from Node childrens list. Return true if the remove was sucessfull, otherwise false
bool Node::removeNode(long s) { //True if removed, else false
	for (vector<Node *>::iterator it = childrens.begin(); it != childrens.end(); ++it) 
	{
		if (((*it)->symbol == s)) {
			childrens.erase(it);
			return true;
		}
	}
	return false;
}

//Return true if symbol s is present at least one time in the Node childrens list. Else return false
bool Node::isPresent(long s) {
	for (vector<Node *>::iterator it = childrens.begin(); it != childrens.end(); ++it)
	{
		if (((*it)->symbol == s)) {
			return true;
		}
	}
	return false;
}

//Constructor of Node. w is the word linked to the symbol s
Node::Node(string w, long s)
{
	word = w;
	symbol = s;
}


Node::~Node()
{
}
