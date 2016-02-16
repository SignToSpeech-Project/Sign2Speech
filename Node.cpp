#include "Node.h"

//Key : symbol. Value : word

string Node::getWord() { return word; }
long Node::getSymbol() { return symbol; }

//Add to the Node childrens list the Node n
//If there is already a symbol s in the childre list, it just update his linked word
Node* Node::addNode(string w, long s) {
	for (vector<Node *>::iterator it = childrens.begin(); it != childrens.end(); ++it)
	{
		if (((*it)->symbol == s)) {
			(*it)->word = w;
			return (*it);
		}
	}
	Node * n = new Node(w, s);
	childrens.push_back(n);
	return n;

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

//Return the children of this node with the symbol s. Otherwise return NULL
Node* Node::getChildren(long s) {
	for (vector<Node *>::iterator it = childrens.begin(); it != childrens.end(); ++it)
	{
		if (((*it)->symbol == s)) {
			return (*it);
		}
	}
	return NULL;
}

vector <Node*> Node::getChildrens() {
	return childrens;
}

bool Node::hasChildrens() {
	return childrens.size() != 0;
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
