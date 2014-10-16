#include "glpk.h"
#include "element.h"
#include "set.h"
#include "set-cover.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstring>

using std::vector;

int main(int argc, char* argv[]) {
	SetCover* problem = SetCover::problemSetup("set-cover-input");
	vector<double> result;
	if (!strcmp(argv[1], "LP_rounding"))	
		result = problem->approx_LP_rounding();
	else if (!strcmp(argv[1], "dual_rounding"))
		result = problem->approx_dual_rounding();
	else if (!strcmp(argv[1], "primal_dual"))
		result = problem->approx_primal_dual();
	else if (!strcmp(argv[1], "greedy"))
		result = problem->approx_greedy();
	else if (!strcmp(argv[1], "exact"))
		result = problem->exact_MIP();
	else // default
		result = problem->approx_LP_rounding();
	std::cout << "Total weight: " << result[6] << '\n';
	std::cout << "Set chosen: ";
	for (int i = 0; i < 6; i++){
		std::cout << (int)result[i] << " ";
	}
	std::cout << "\n";

	std::cin.get();
	return 0;
}

