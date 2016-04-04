#include "stdafx.h"

//Key : symbol. Value : word

string Node::getWord() { return word; }
long Node::getSymbol() { return symbol; }

//Add to the Node childrens list the Node n
//If there is already a symbol s in the childre list, it just update his linked word
Node* Node::addNode(string w, long s) {
	for (vector<Node *>::iterator it = childrens.begin(); it != childrens.end(); ++it)
	{
		if (((*it)->symbol == s)) {
			if( w != "" ) (*it)->word = w;
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

//Return the amount of difference of the fingers between 2 symbols

int Node::scoreFingers(long s1, long s2) {
	int nbdif = 0;
	int eg = 0;
	int em = 0;
	int es = 0;
	int fs1;
	int fs2;
	int diffFingers;

	for (int f = 0; f <= 4; f++) {
		fs1 = (s1 >> (2 * f) ) & 0b11;
		fs2 = (s2 >> (2 * f)) & 0b11;
		diffFingers = abs(fs1 - fs2);
		if (diffFingers == 1) {
			es++;
			nbdif++;
		}
		else if (diffFingers == 2) {
			em++;
			nbdif++;
		}

		else if (diffFingers == 3) {
			eg++;
			nbdif++;
		}

	}
	return 10 * eg + 3 * em + es + nbdif;
}

//Return the children of this node with the symbol s. Otherwise return NULL
Node* Node::getChildren(long s) {
	int score = 1000;
	int tempScore = 1000;
	Node* tempNode = NULL;
	int seuil = 5; 
	for (vector<Node *>::iterator it = childrens.begin(); it != childrens.end(); ++it)
	{
		if (((*it)->symbol == s)) { //Optimisation because we already found the exact symbol
			return (*it);
		}
		tempScore = 
			scoreFingers(s, (*it)->symbol);
		if (tempScore < score) {
			score = tempScore;
			tempNode = (*it);
		}
	}
	if (score < SEUIL) 	return tempNode;
	
	else return NULL;
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
