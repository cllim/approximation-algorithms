//#define CATCH_CONFIG_MAIN  // Use main() provided by Catch 
#define CATCH_CONFIG_RUNNER  // Define our own main
#include <iostream>
#include "catch.hpp"
#include <vector>
#include <cstdlib>
#include "../Set-Cover/element.h"
#include "../Set-Cover/set.h"
#include "../Set-Cover/set-cover.h"

using std::vector;
using std::string;
/* Test of class Element */
/**
* Test of generateElements function, of class Element.
*/
TEST_CASE("Generate n elements", "[Element]") {
	int n = 0;
	vector<Element*>* eptr = Element::generateElements(n);
	REQUIRE(eptr == nullptr);
	delete eptr;

	n = -45;
	eptr = Element::generateElements(n);
	REQUIRE(eptr == nullptr);
	delete eptr;

	n = 5;
	eptr = Element::generateElements(n);
	vector<Element*> e = *eptr;
	REQUIRE(e.size() == n);
	for (size_t i = 0; i < e.size(); i++){
		REQUIRE(e[i]->id == i + 1);
	}
	delete eptr;
}

/* End of test of class Element*/

/* Test of class Set */
/**
* Test of hasElement function, of class Set.
*/
TEST_CASE("Check if a set contains a certain element", "[Set]") {
	int n = 4;
	vector<Element*> e = *Element::generateElements(n);

	vector<Element*> elementsofs1 = { e[0], e[2] };
	Set s1(elementsofs1); //s1 has elements 1 and 3
	REQUIRE(s1.hasElement(-5) == false);
	REQUIRE(s1.hasElement(0) == false);
	REQUIRE(s1.hasElement(1) == true);
	REQUIRE(s1.hasElement(2) == false);
	REQUIRE(s1.hasElement(3) == true);
	REQUIRE(s1.hasElement(4) == false);
	REQUIRE(s1.hasElement(8) == false);
}

/**
* Test of createSet static function, of class Set.
*/
TEST_CASE("Create Set from input string", "[Set]") {
	unsigned int n = 5;
	int weight = 1;

	// Create set with all elements
	vector<Element*> e = *Element::generateElements(n);
	Set* ground = Set::createSet("1 2 3 4 5", weight, e);
	REQUIRE(ground->e.size() == n);
	REQUIRE(ground->weight == weight);
	for (unsigned int i = 0; i < n; i++){
		REQUIRE(ground->hasElement(i + 1));
	}

	// Input empty string
	Set* emptyset = Set::createSet("", weight, e);
	REQUIRE(emptyset == nullptr);

	// Input no element
	vector<Element*> f;
	Set* s2 = Set::createSet("1 3 osjd 9 5", weight, f);
	REQUIRE(emptyset == nullptr);

	// String with invalid characters and invalid weight
	weight = -5;
	Set* s1 = Set::createSet("1 3 9 5", weight, e);
	REQUIRE(s1->weight == 1); // defaults to 1
	REQUIRE(s1->e.size() == 3);
	REQUIRE(s1->hasElement(1));
	REQUIRE(s1->hasElement(3));
	REQUIRE(s1->hasElement(5));
	REQUIRE(!s1->hasElement(2));
	REQUIRE(!s1->hasElement(4));
}

/**
* Test of setToChosen function, of class Set.
*/
TEST_CASE("Change a Set to chosen", "[Set]") {
	int n = 4;
	vector<Element*> e = *Element::generateElements(n);

	vector<Element*> elementsofs1 = { e[0], e[2] };
	Set s1(elementsofs1); //s1 has elements 1 and 3
	vector<Element*> elementsofs2 = { e[1], e[2] };
	Set s2(elementsofs2); //s1 has elements 2 and 3
	vector<Element*> elementsofs3 = { e[3], e[2] };
	Set s3(elementsofs3); //s1 has elements 4 and 3

	REQUIRE(!s1.chosen);
	s1.setToChosen();
	REQUIRE(s1.chosen);
	REQUIRE(e[0]->count == 1);
	REQUIRE(e[1]->count == 0);
	REQUIRE(e[2]->count == 1);
	REQUIRE(e[3]->count == 0);

	// check for repeat
	s1.setToChosen();
	REQUIRE(s1.chosen);
	REQUIRE(e[0]->count == 1);
	REQUIRE(e[1]->count == 0);
	REQUIRE(e[2]->count == 1);
	REQUIRE(e[3]->count == 0);

	// Two sets
	s2.setToChosen();
	REQUIRE(e[0]->count == 1);
	REQUIRE(e[1]->count == 1);
	REQUIRE(e[2]->count == 2);
	REQUIRE(e[3]->count == 0);

	// All three sets
	s3.setToChosen();
	REQUIRE(e[0]->count == 1);
	REQUIRE(e[1]->count == 1);
	REQUIRE(e[2]->count == 3);
	REQUIRE(e[3]->count == 1);
}

/**
* Test of countNotCovered function, of class Set.
*/
TEST_CASE("Count number of elements not yet covered", "[Set]") {
	int n = 4;
	vector<Element*> e = *Element::generateElements(n);
	Set* ground = Set::createSet("1 2 3 4", 1, e);
	Set* s1 = Set::createSet("1 2", 1, e);
	Set* s2 = Set::createSet("2 3", 1, e);

	REQUIRE(ground->countNotCovered() == 4);
	REQUIRE(s1->countNotCovered() == 2);
	REQUIRE(s2->countNotCovered() == 2);

	s1->setToChosen();
	REQUIRE(ground->countNotCovered() == 2);
	REQUIRE(s1->countNotCovered() == 0);
	REQUIRE(s2->countNotCovered() == 1);
}

/* End of test of class Set */

/* Test of class SetCover */
/**
* Test of setf function, of class SetCover.
*/
TEST_CASE("Determine and set f: The maximum number of times an element is contained in any subset", "[SetCover]") {
	int n = 5;
	vector<Element*> e = *Element::generateElements(n);

	Set* ground = Set::createSet("1 2 3 4 5", 1, e);

	Set* s1 = Set::createSet("1 2 3", 1, e);
	Set* s2 = Set::createSet("2 4", 1, e);
	Set* s3 = Set::createSet("3 4", 1, e);
	Set* s4 = Set::createSet("4 5", 1, e);
	vector<Set*> collection;
	SetCover problem(ground, collection);
	REQUIRE(problem.f == 0);

	collection = { s1, s2, s3, s4 };
	SetCover problem2(ground, collection);
	REQUIRE(problem2.f == 3); // Element with id 4 appears 3 times
}

/**
* Test of isFeasible function, of class SetCover.
*/
TEST_CASE("Check if problem has a feasible solution", "[SetCover]") {
	int n = 5;
	vector<Element*> e = *Element::generateElements(n);

	Set* ground = Set::createSet("1 2 3 4 5", 1, e);

	Set* s1 = Set::createSet("1 2 3", 1, e);
	Set* s2 = Set::createSet("2 4", 1, e);
	Set* s3 = Set::createSet("3 4", 1, e);
	Set* s4 = Set::createSet("4 5", 1, e);
	vector<Set*> collection = { s1, s2, s3 };
	SetCover problem(ground, collection);
	REQUIRE(!problem.isFeasible());

	vector<Set*> collection2 = { s1, s2, s3, s4 };
	SetCover problem2(ground, collection2);
	REQUIRE(problem2.isFeasible());
}

/**
* Test of approx_LP_rounding function, of class SetCover.
*/
TEST_CASE("Find set cover via LP rounding", "[SetCover]") {
	int n = 5;
	vector<Element*> e = *Element::generateElements(n);

	Set* ground = Set::createSet("1 2 3 4 5", 1, e);

	Set* s1 = Set::createSet("1 2 3", 1, e);
	Set* s2 = Set::createSet("1 2", 1, e);
	Set* s3 = Set::createSet("3 4", 1, e);
	Set* s4 = Set::createSet("4 5", 1, e);
	vector<Set*> collection = { s1, s2, s3, s4 };
	SetCover problem(ground, collection);
	vector<double> result = problem.approx_LP_rounding();
	/* The LP solution is the optimal solution */
	REQUIRE(result[0] == 1);
	REQUIRE(result[1] == 0);
	REQUIRE(result[2] == 0);
	REQUIRE(result[3] == 1);
	REQUIRE(result[4] == 2); // total weight

	problem.reset();
	s1 = Set::createSet("1 2 3", 5, e);
	s2 = Set::createSet("1 2", 2, e);
	s3 = Set::createSet("3 4", 2, e);
	s4 = Set::createSet("4 5", 1, e);
	collection = { s1, s2, s3, s4 };
	SetCover problem2(ground, collection);
	result = problem2.approx_LP_rounding();
	/* In the optimal solution, the variable associated with s1 is set to zero
	 * because one could always choose s2 and s3 instead to achieve a lower total weight
	 * By elimination of s1, we again find the LP solution to be the optimal solution
	 */
	REQUIRE(result[0] == 0);
	REQUIRE(result[1] == 1);
	REQUIRE(result[2] == 1);
	REQUIRE(result[3] == 1);
	REQUIRE(result[4] == 5); // total weight
}

/**
* Test of approx_dual_rounding function, of class SetCover.
*/
TEST_CASE("Find set cover via dual rounding", "[SetCover]") {
	int n = 5;
	vector<Element*> e = *Element::generateElements(n);

	Set* ground = Set::createSet("1 2 3 4 5", 1, e);

	Set* s1 = Set::createSet("1 2 3", 1, e);
	Set* s2 = Set::createSet("1 2", 1, e);
	Set* s3 = Set::createSet("3 4", 1, e);
	Set* s4 = Set::createSet("4 5", 1, e);
	vector<Set*> collection = { s1, s2, s3, s4 };
	SetCover problem(ground, collection);
	vector<double> result = problem.approx_dual_rounding();
	/* The dual solution satisfies all its constraints with equality, and thus all sets are chosen  */
	REQUIRE(result[0] == 1);
	REQUIRE(result[1] == 1);
	REQUIRE(result[2] == 1);
	REQUIRE(result[3] == 1);
	REQUIRE(result[4] == 4); // total weight

	problem.reset();
	s1 = Set::createSet("1 2 3", 5, e);
	s2 = Set::createSet("1 2", 2, e);
	s3 = Set::createSet("3 4", 2, e);
	s4 = Set::createSet("4 5", 1, e);
	collection = { s1, s2, s3, s4 };
	SetCover problem2(ground, collection);
	result = problem2.approx_dual_rounding();
	/* The dual solution satisfies all but the constraints associated with s1 with equality
	 * The constraint associated with s1 would end up as 3 <= 5 (not satisfied with equality)
	 */
	REQUIRE(result[0] == 0);
	REQUIRE(result[1] == 1);
	REQUIRE(result[2] == 1);
	REQUIRE(result[3] == 1);
	REQUIRE(result[4] == 5); // total weight
}

/**
* Test of exact_MIP function, of class SetCover.
*/
TEST_CASE("Find optimal set cover via mixed integer programming", "[SetCover]") {
	int n = 5;
	vector<Element*> e = *Element::generateElements(n);

	Set* ground = Set::createSet("1 2 3 4 5", 1, e);

	Set* s1 = Set::createSet("1 2 3", 1, e);
	Set* s2 = Set::createSet("1 2", 1, e);
	Set* s3 = Set::createSet("3 4", 1, e);
	Set* s4 = Set::createSet("4 5", 1, e);
	vector<Set*> collection = { s1, s2, s3, s4 };
	SetCover problem(ground, collection);
	vector<double> result = problem.exact_MIP();
	/* Solution is obvious by inspection */
	REQUIRE(result[0] == 1);
	REQUIRE(result[1] == 0);
	REQUIRE(result[2] == 0);
	REQUIRE(result[3] == 1);
	REQUIRE(result[4] == 2); // total weight

	problem.reset();
	s1 = Set::createSet("1 2 3", 5, e);
	s2 = Set::createSet("1 2", 2, e);
	s3 = Set::createSet("3 4", 2, e);
	s4 = Set::createSet("4 5", 1, e);
	collection = { s1, s2, s3, s4 };
	SetCover problem2(ground, collection);
	result = problem2.exact_MIP();
	/* Solution is obvious by inspection */
	REQUIRE(result[0] == 0);
	REQUIRE(result[1] == 1);
	REQUIRE(result[2] == 1);
	REQUIRE(result[3] == 1);
	REQUIRE(result[4] == 5); // total weight
}

/**
* Test of approx_greedy function, of class SetCover.
*/
TEST_CASE("Find set cover via greedy algorithm", "[SetCover]") {
	int n = 5;
	vector<Element*> e = *Element::generateElements(n);

	Set* ground = Set::createSet("1 2 3 4 5", 1, e);

	Set* s1 = Set::createSet("1 2 3", 1, e);
	Set* s2 = Set::createSet("1 2", 1, e);
	Set* s3 = Set::createSet("3 4", 1, e);
	Set* s4 = Set::createSet("4 5", 1, e);
	vector<Set*> collection = { s1, s2, s3, s4 };
	SetCover problem(ground, collection);
	vector<double> result = problem.approx_greedy();
	/* Set s1 gives the best bang for the metric (3 vs 2 for all others) at the start 
	 * After s1 is chosen, s4 gives the best bang for the metric (2 vs 1 for all others) 
	 * Set is covered after s4 is chosen and terminates
	 */
	REQUIRE(result[0] == 1);
	REQUIRE(result[1] == 0);
	REQUIRE(result[2] == 0);
	REQUIRE(result[3] == 1);
	REQUIRE(result[4] == 2); // total weight

	problem.reset();
	s1 = Set::createSet("1 2 3", 5, e);
	s2 = Set::createSet("1 2", 2, e);
	s3 = Set::createSet("3 4", 2, e);
	s4 = Set::createSet("4 5", 1, e);
	collection = { s1, s2, s3, s4 };
	SetCover problem2(ground, collection);	
	result = problem2.approx_greedy();
	/* Set s4 gives the best bang for the metric (2 vs 3/5 or 1 for all others) at the start
	 * After s4 is chosen, s2 gives the best bang for the metric (1 vs 3/5 or 1/2 for all others)
	 * After s4 and s2 are chosen, s3 gives the best bang for the metric (1/2 vs 1/5 for s1)
	 * Set is covered after s3 is chosen and terminates
	 */
	REQUIRE(result[0] == 0);
	REQUIRE(result[1] == 1);
	REQUIRE(result[2] == 1);
	REQUIRE(result[3] == 1);
	REQUIRE(result[4] == 5); // total weight
}

/**
* Test of approx_primal_dual function, of class SetCover.
*/
TEST_CASE("Find set cover via primal-dual algorithm", "[SetCover]") {
	int n = 5;
	vector<Element*> e = *Element::generateElements(n);

	Set* ground = Set::createSet("1 2 3 4 5", 1, e);

	Set* s1 = Set::createSet("1 2 3", 1, e);
	Set* s2 = Set::createSet("1 2", 1, e);
	Set* s3 = Set::createSet("3 4", 1, e);
	Set* s4 = Set::createSet("4 5", 1, e);
	vector<Set*> collection = { s1, s2, s3, s4 };
	SetCover problem(ground, collection);
	vector<double> result = problem.approx_primal_dual();
	/* Algorithm first increments the dual variable associated with element 1 to 1 and set s1 is chosen
	 * The dual variable associated with element 4 is increased to 1 and set s3 is chosen
	 * The dual variable associated with element 5 is increased by 0 and set s4 is chosen and program terminates
	 */
	REQUIRE(result[0] == 1);
	REQUIRE(result[1] == 0);
	REQUIRE(result[2] == 1);
	REQUIRE(result[3] == 1);
	REQUIRE(result[4] == 3); // total weight

	problem.reset();
	s1 = Set::createSet("1 2 3", 5, e);
	s2 = Set::createSet("1 2", 2, e);
	s3 = Set::createSet("3 4", 2, e);
	s4 = Set::createSet("4 5", 1, e);
	collection = { s1, s2, s3, s4 };
	SetCover problem2(ground, collection);
	result = problem2.approx_primal_dual();
	/* Algorithm first increments the dual variable associated with element 1 to 2 and set s2 is chosen
	* The dual variable associated with element 3 is increased to 2 and set s3 is chosen
	* The dual variable associated with element 5 is increased by 0 and set s4 is chosen and program terminates
	*/
	REQUIRE(result[0] == 0);
	REQUIRE(result[1] == 1);
	REQUIRE(result[2] == 1);
	REQUIRE(result[3] == 1);
	REQUIRE(result[4] == 5); // total weight
}

/* End of test of class SetCover */

TEST_CASE("Test of problem creation", "[Problem]"){
	// Using the given set-cover-input file
	SetCover* problem = SetCover::problemSetup("set-cover-input");
	vector<Set*> subset = problem->subset;
	REQUIRE(problem->n == 10);
	REQUIRE(problem->f == 4); // 10 appears in 4 subsets
	REQUIRE(problem->covered == 0);
	REQUIRE(problem->isFeasible());
	REQUIRE(subset[0]->weight == 1);
	REQUIRE(subset[0]->e.size() == 5);
	REQUIRE(subset[1]->weight == 1);
	REQUIRE(subset[1]->e.size() == 3);
	REQUIRE(subset[5]->weight == 1);
	REQUIRE(subset[5]->e.size() == 5);
}

int main(int argc, char * const argv[]) {
	int y = Catch::Session().run(argc, argv);
	std::cin.get();

	return y;
}