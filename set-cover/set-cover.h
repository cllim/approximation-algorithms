#ifndef SC_SET_COVER_H_
#define SC_SET_COVER_H_

#include "element.h"
#include "set.h"
#include <vector>
#include <string>

class SetCover{
public:
	Set* groundset;
	std::vector<Set*> subset;
	int f; // The maximum number of times an element is contained in any subset
	int n; // The number of elements in the groundset
	int covered; // The number of covered elements
	/* n and covered are used for O(1) determination of whether the set is covered */

	SetCover(Set* g, std::vector<Set*> s);		// Constructor

	static SetCover* problemSetup(std::string filename);  // Setup the problem by reading from the input file
	
	bool isCovered();   // Check if all elements are covered
	bool isFeasible();	// A set cover exists
	void reset();		// Reset to the state where no subset is chosen
	void setf();		// Determine and set f
	std::vector<double> approx_LP_rounding();	// Find set cover via LP rounding (f-approximation algorithm)	
	std::vector<double> approx_dual_rounding(); // Find set cover via dual rounding (f-approximation algorithm)	
	std::vector<double> exact_MIP();			// Find optimal set cover via mixed integer programming
	std::vector<double> approx_greedy();		// Find set cover via greedy algorithm (H(n)-approximation algorithm where H(n) is the nth harmonic number)
	std::vector<double> approx_primal_dual();	// Find set cover via primal-dual algorithm (f-approximation algorithm)
};

#endif /* SC_SET_COVER_H_*/