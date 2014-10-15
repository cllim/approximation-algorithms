#include "glpk.h"
#include "element.h"
#include "set.h"
#include "set-cover.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>

using std::string;
using std::vector;

/* Alternative for when to_string() isn't supported by compiler (g++) */
string numberToString(int x)
{
	std::ostringstream ss;
	ss << x;
	return ss.str();
}


int main() {
	std::ifstream input("set-cover-input");
	string line;
	if (!input.is_open()){
		std::cout << "Unable to open file.";
		std::cin.get();
		return 0;
	}
	// First line specifies the number of elements
	std::getline(input, line);
	line.erase(line.find_last_not_of(" \n\r\t") + 1); //right trim spaces
	if (!strIsNumber(line)){
		std::cout << "Invalid number of elements";
		std::cin.get();
		return 0;
	}
	int n = strtol(line.c_str(), 0, 10);
	if (n <= 0){
		std::cout << "Invalid number of elements";
		std::cin.get();
		return 0;
	}
	vector<Element*> e = *Element::generateElements(n);
	string all = "";
	for (int i = 1; i <= n; i++){
		all = all + numberToString(i);
		if (i != n)
			all += " ";
	}
	Set* ground = Set::createSet(all, 1, e);

	// Second line specifies the number of sets (optional)
	getline(input, line);
	line.erase(line.find_last_not_of(" \n\r\t") + 1); //right trim spaces
	int numset = 0;
	if (strIsNumber(line)){
		numset = strtol(line.c_str(), 0, 10);
	}

	vector<Set*> subset;
	subset.reserve(numset);
	// Third to last line create the subsets
	size_t spaceafterweight;
	string sweight;
	int weight;
	string elements;
	int countset = 0;
	while (std::getline(input, line)){
		countset++;
		// First parse the weight
		spaceafterweight = line.find_first_of(" ");
		sweight = line.substr(0, spaceafterweight);
		if (strIsNumber(sweight)){
			weight = strtol(sweight.c_str(), 0, 10);
		}
		else{
			weight = 1; // defaults to 1
		}
		// Next parse the elements
		elements = line.substr(spaceafterweight + 1, line.length());
		elements.erase(elements.find_last_not_of(" \n\r\t") + 1); //right trim spaces
		subset.push_back(Set::createSet(elements, weight, e));
	}

	SetCover problem(ground, subset);
	vector<double> result(countset + 1);


	result = problem.approx_LP_rounding();
	std::cout << "Total weight: " << result[countset] << '\n';
	std::cout << "Set chosen: ";
	for (int i = 0; i < countset; i++){
		std::cout << (int)result[i] << " ";
	}
	std::cout << "\n";

	std::cin.get();
	return 0;
}

