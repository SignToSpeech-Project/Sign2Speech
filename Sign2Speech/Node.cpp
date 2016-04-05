#include "stdafx.h"

//Key : symbol - Value : word

string Node::getWord() { return word; }
long Node::getSymbol() { return symbol; }

//Add to the Node children list the Node n
//If there is already a symbol s in the children list, it just update his linked word
Node* Node::addNode(string w, long s) {
	for (vector<Node *>::iterator it = children.begin(); it != children.end(); ++it)
	{
		if (((*it)->symbol == s)) {
			if( w != "" ) (*it)->word = w;
			return (*it);
		}
	}
	Node * n = new Node(w, s);
	children.push_back(n);
	return n;

}

//Remove the first child with the symbol s from Node children list. Return true if the remove was sucessfull, otherwise false
bool Node::removeNode(long s) { //True if removed, else false
	for (vector<Node *>::iterator it = children.begin(); it != children.end(); ++it)
	{
		if (((*it)->symbol == s)) {
			children.erase(it);
			return true;
		}
	}
	return false;
}

//Return true if symbol s is present at least one time in the Node children list. Else return false
bool Node::isPresent(long s) {
	for (vector<Node *>::iterator it = children.begin(); it != children.end(); ++it)
	{
		if (((*it)->symbol == s)) {
			return true;
		}
	}
	return false;
}

//Return the amount of difference of the fingers between 2 symbols following the function : 10 * eg + 3 * em + es + nbdif;
int Node::scoreGeasture(long s1, long s2) {

	int nbdif = 0; //Nb errors
	int eg = 0; //nb of major errors
	int em = 0; //nb of medium errors
	int es = 0; //nb of simple errors
	int fs1; //Finger x of s1
	int fs2; //finger x of s2
	int diffFingers;

	//Fingers difference
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
	
	int trajectorie1 = (s1 >> 11) & 0b11111111; //Trajectorie of s1
	int trajectorie2 = (s2 >> 11) & 0b11111111; //Trajectorie of s2

	//Checking if we have a static geasture
	if ( (trajectorie1 == 0) || (trajectorie2 == 0) ) {
		if (trajectorie1 != trajectorie2) {
			eg++;
			nbdif++;
		}
	}

	//Else, checking trajectories differences
	else {

		int t11 = (s1 >> 11) & 0b1; //Horizontal right bit
		int t21 = (s2 >> 11) & 0b1; //Horizontal right bit
		int t12 = (s1 >> 12) & 0b1; //Horizontal left bit
		int t22 = (s2 >> 12) & 0b1; //Horizontal left bit
		int t13 = (s1 >> 13) & 0b1; //Vertical right bit
		int t23 = (s2 >> 13) & 0b1; //Vertical right bit
		int t14 = (s1 >> 14) & 0b1; //Vertical left bit
		int t24 = (s2 >> 14) & 0b1; //Vertical left bit
		int t17 = (s1 >> 17) & 0b1; //Full ellipse bit
		int t27 = (s2 >> 17) & 0b1; //Full ellipse bit
		int t18 = (s1 >> 18) & 0b1; //Partial ellipse bit
		int t28 = (s2 >> 18) & 0b1; //Partial ellipse bit


		if ((t11 == 1 && t22 == 1) || (t12 == 1 && t21 == 1)) {
			em++;
			nbdif++;
		}

		if ((t13 == 1 && t24 == 1) || (t14 == 1 && t23 == 1)) {
			em++;
			nbdif++;
		}

		if ((((t11 == 1) || (t12 == 1) || (t13 == 1) || (t14 == 1)) && ((t27 == 1) || (t28 == 1)))
			|| (((t21 == 1) || (t22 == 1) || (t23 == 1) || (t24 == 1)) && ((t17 == 1) || (t18 == 1)))) {
			eg++;
			nbdif++;
		}

		if ((t11 == 1 && t23 == 1) || (t13 == 1 && t21 == 1)) {
			es++;
			nbdif++;
		}

		if ((t11 == 1 && t24 == 1) || (t14 == 1 && t21 == 1)) {
			es++;
			nbdif++;
		}

		if ((t12 == 1 && t23 == 1) || (t13 == 1 && t22 == 1)) {
			es++;
			nbdif++;
		}

		if ((t12 == 1 && t24 == 1) || (t14 == 1 && t22 == 1)) {
			es++;
			nbdif++;
		}

		if ((t17 == 1 && t28 == 1) || (t18 == 1 && t27 == 1)) {
			em++;
			nbdif++;
		}
	}
	

	return 10 * eg + 3 * em + es + nbdif;
}

//Return the child of this node with the symbol s. Otherwise return NULL
Node* Node::getChild(long s) {
	int score = 1000;
	int tempScore = 1000;
	Node* tempNode = NULL;
	for (vector<Node *>::iterator it = children.begin(); it != children.end(); ++it)
	{
		if (((*it)->symbol == s)) { //Optimisation because we already found the exact symbol
			return (*it);
		}
		tempScore = 
			scoreGeasture(s, (*it)->symbol);
		if (tempScore < score) {
			score = tempScore;
			tempNode = (*it);
		}
	}
	if (score < SEUIL) 	return tempNode;
	
	else return NULL;
}

//Retun all children of the current Node
vector <Node*> Node::getChildren() {
	return children;
}

//Return true if the current nod has at least one child. Otherwise, return false
bool Node::hasChild() {
	return children.size() != 0;
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
