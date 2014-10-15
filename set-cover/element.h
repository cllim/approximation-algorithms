#ifndef SC_ELEMENT_H_
#define SC_ELEMENT_H_

#include <vector>

class Element{
public:
	int id;
	int count;

	Element();
	Element(int i);

	static std::vector<Element*>* generateElements(int n); // generate elements with id 1,2,...,n
};

#endif /* AA_SC_ELEMENT_H_ */