#include "element.h"
#include "set.h"
#include <assert.h>
#include <vector>
#include <string>
#include <cstdlib>

using std::string;
using std::vector;

// Helper
bool strIsNumber(string s){
	int num = 0;
	for (char c : s){
		num = c - '0';
		if (!(0 <= num && num <= 9)){
			return false;
		}
	}
	return true;
}

// Constructors
Set::Set(vector<Element*> elements) : e(elements), weight(1), chosen(false) {}
Set::Set(vector<Element*> elements, double w) : e(elements), weight(w), chosen(false) {}
Set::Set(vector<Element*> elements, double w, bool b) : e(elements), weight(w), chosen(b) {}

// Factory method to create Set from input string
Set* Set::createSet(string str, double w, vector<Element*> e){
	if (str == "" || e.size() == 0) return nullptr;
	w = w > 0 ? w : 1; // defaults to 1 if weight is non-positive

	size_t pos = 0;
	size_t currspace = 0;
	size_t prevspace = 0;
	size_t index = 0;
	string input = "";
	int num;
	bool validinput = false;
	vector<Element*> elementinset;
	do{
		validinput = true;
		currspace = str.find_first_of(" ", prevspace);
		if (currspace == string::npos)
			currspace = str.length();
		input = str.substr(prevspace, currspace - prevspace);
		validinput = strIsNumber(input);
		if (validinput){
			index = strtol(str.substr(prevspace, currspace - prevspace).c_str(), 0, 10) - 1;
			prevspace = currspace + 1;
			if (0 <= index && index < e.size()){
				elementinset.push_back(e[index]);
			}
		}
	} while (currspace != str.length());
	return new Set(elementinset, w);
}

// Change a set to chosen
void Set::setToChosen(){
	if (chosen) return;
	else{
		chosen = true;
		for (Element* e : this->e){
			e->count++;
		}
	}
}

// Check if set contains element with given id
bool Set::hasElement(int id){
	for (Element* e : this->e){
		if (e->id == id)
			return true;
	}
	return false;
}

// Count number of elements not yet covered in the set
int Set::countNotCovered(){
	int notcovered = 0;
	for (Element* e : this->e){
		if (e->count == 0){
			notcovered++;
		}
	}
	return notcovered;
}
