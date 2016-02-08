#include "Dictionary.h"
#include "Node.h"
#include <string>

using std::string;

class Dictionary {
public :
	void insert(string word, long symbol);

	bool remove(long symbol);

	bool isPresent(long symbol);

	//Avance si c'est possible le currentNode sur le nouveau symbole et return "". Sinon remet le currentNode à la racine et retourne la valeur du currentNode juste avant.
	string read(long symbol);

	//Retourne le mot correspondant au symbole du noeud courant
	string getValueCurrentNode();

	Dictionary();

	~Dictionary();
private :
	Node * racine;
	Node * currentNode;
};

Dictionary::Dictionary()
{
	racine = new Node("", 0);
	currentNode = racine;
}

void Dictionary::insert(string word, long symbol) {
	if (!currentNode->isPresent(symbol)) currentNode->addNode(word,symbol);
}

bool Dictionary::remove(long symbol) {
	return currentNode->removeNode(symbol);
}

bool Dictionary::isPresent(long symbol) {
	return currentNode->isPresent(symbol);
}

Dictionary::~Dictionary()
{
}
