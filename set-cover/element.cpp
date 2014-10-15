#include "element.h"
#include <vector>

using std::vector;

Element::Element() : id(0), count(0){}
Element::Element(int i) : id(i), count(0) {}

// Generate elements with id 1,2,...,n
vector<Element*>* Element::generateElements(int n){
	if (n <= 0) return nullptr;
	vector<Element*>* eptr = new vector<Element*>();
	for (int i = 0; i < n; i++){
		eptr->push_back(new Element(i + 1));
	}
	return eptr;
}