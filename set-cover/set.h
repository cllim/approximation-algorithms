#ifndef SC_SET_H_
#define SC_SET_H_

#include "element.h"
#include <vector>
#include <string>

class Set{
public:
	std::vector<Element*> e;
	double weight;
	bool chosen;

	// Constructors
	Set(std::vector<Element*> elements); 
	Set(std::vector<Element*> elements, double w); 
	Set(std::vector<Element*> elements, double w, bool b); 

	// Factory method to create new instance from input string
	static Set* createSet(std::string s, double w, std::vector<Element*> e);

	void setToChosen();			// Change a set to chosen
	bool hasElement(int id);	// Check if set contains element with given id
	int countNotCovered();		// Count number of elements not yet covered in the set
};

// Helper
bool strIsNumber(std::string s);

#endif  /* SC_SET_H_ */