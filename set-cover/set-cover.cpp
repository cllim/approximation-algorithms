#include "set-cover.h"
#include "element.h"
#include "set.h"
#include "glpk.h"
#include <assert.h>
#include <vector>
#include <limits>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>

using std::vector;
using std::string;

/* helper function */
bool isEqual(double a, double b){
	double x = a > b ? a - b : b - a;
	return x < 1e-8;
}

/* Alternative for when to_string() isn't supported by compiler (g++) */
string numberToString(int x)
{
	std::ostringstream ss;
	ss << x;
	return ss.str();
}

/* Constructor */
SetCover::SetCover(Set* g, vector<Set*> s) : groundset(g), subset(s), covered(0) {
	setf();
	n = g->e.size();
}

/* Setup the problem by reading from the input file */
SetCover* SetCover::problemSetup(string filename){
	std::ifstream input(filename);
	string line;
	if (!input.is_open()){
		std::cout << "Unable to open file.";
		std::cin.get();
		return nullptr;
	}

	// First line specifies the number of elements
	std::getline(input, line);
	line.erase(line.find_last_not_of(" \n\r\t") + 1); //right trim spaces
	if (!strIsNumber(line)){
		std::cout << "Invalid number of elements";
		std::cin.get();
		return nullptr;
	}
	int n = strtol(line.c_str(), 0, 10);
	if (n <= 0){
		std::cout << "Invalid number of elements";
		std::cin.get();
		return nullptr;
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
			weight = 1; // defaults to 1 for invalid input
		}
		weight = weight <= 0 ? 1 : weight; // defaults to 1 for non-positive weight

		// Next parse the elements
		elements = line.substr(spaceafterweight + 1, line.length());
		elements.erase(elements.find_last_not_of(" \n\r\t") + 1); //right trim spaces
		subset.push_back(Set::createSet(elements, weight, e));
	}

	return new SetCover(ground, subset);
}

/* Check if all elements are covered */
bool SetCover::isCovered(){
	return n == covered;
}

/* Check if set cover exists by choosing all subset */
bool SetCover::isFeasible(){
	for (Set* s : subset){
		s->setToChosen();
	}
	for (Element* e : groundset->e){
		if (e->count == 0)
			return false;
	}
	reset();
	return true;
}

/* Reset to the state where no subset is chosen */
void SetCover::reset(){
	for (Set* s : subset){
		s->chosen = false;
	}
	for (Element* e : groundset->e){
		e->count = 0;
	}
	covered = 0;
}

/* Determine and set f */
void SetCover::setf(){
	int f = 0;
	for (Set* s : subset){
		s->setToChosen();
	}
	for (Element* e : groundset->e){
		if (e->count > f)
			f = e->count;
	}
	reset();
	this->f = f;
}

/* Find set cover via LP rounding (f-approximation algorithm) */
vector<double> SetCover::approx_LP_rounding(){
	int numrow = n;
	int numcol = subset.size();
	int k = 1;
	int contains = 0;

	/* Note: GLPK uses 1-based numbering*/
	glp_prob *lp;
	int* ia = new int[1 + numrow*numcol];
	int* ja = new int[1 + numrow*numcol];
	double* ar = new double[1 + numrow*numcol];
	vector<double> x(1 + numcol);

	/* create problem */
	lp = glp_create_prob();
	glp_set_prob_name(lp, "LP Rounding of Set Cover");
	glp_set_obj_dir(lp, GLP_MIN);

	/* construct matrix for the problem problem */
	glp_add_rows(lp, numrow);
	for (int r = 1; r <= numrow; r++){
		glp_set_row_bnds(lp, r, GLP_LO, 1.0, 0);
	}
	glp_add_cols(lp, numcol);
	for (int c = 1; c <= numcol; c++){
		glp_set_col_bnds(lp, c, GLP_LO, 0.0, 0);
		glp_set_obj_coef(lp, c, subset[c - 1]->weight);
	}
	for (int r = 1; r <= numrow; r++){
		for (int c = 1; c <= numcol; c++){
			k = c + numcol*(r - 1);
			/* Coefficient of row r, column c
			 * 1: if subset c-1 contains element r
			 * 0: otherwise
			 */
			contains = subset[c - 1]->hasElement(r) ? 1 : 0;
			ia[k] = r, ja[k] = c, ar[k] = contains;
		}
	}
	glp_load_matrix(lp, numrow*numcol, ia, ja, ar);
	/* solve problem */
	glp_simplex(lp, NULL);
	/* recover results */
	// double val = glp_get_obj_val(lp);
	for (int c = 1; c <= numcol; c++){
		x[c - 1] = glp_get_col_prim(lp, c);
	}
	/* housekeeping */
	glp_delete_prob(lp);
	glp_free_env();
	delete[] ia;
	delete[] ja;
	delete[] ar;

	/* Rounding: round each element of x to 1 if it exceeds 1/f, round to 0 otherwise */
	double totalweight = 0;
	for (int c = 1; c <= numcol; c++){
		if (x[c - 1] > 1 / f){
			x[c - 1] = 1;
			totalweight += subset[c - 1]->weight;
		}
		else {
			x[c - 1] = 0;
		}
	}
	x[x.size() - 1] = totalweight;
	return x;
}

/* Find set cover via dual rounding (f-approximation algorithm)	*/
vector<double> SetCover::approx_dual_rounding(){
	int numcol = groundset->e.size();
	int numrow = subset.size();
	int k = 1;
	int contains = 0;

	/* Note: GLPK uses 1-based numbering*/
	glp_prob *lp;
	int* ia = new int[1 + numrow*numcol];
	int* ja = new int[1 + numrow*numcol];
	double* ar = new double[1 + numrow*numcol];
	vector<double> x(1 + numrow, 0);

	/* create problem */
	lp = glp_create_prob();
	glp_set_prob_name(lp, "Dual Rounding of Set Cover");
	glp_set_obj_dir(lp, GLP_MAX);

	/* construct matrix for the problem problem */
	glp_add_rows(lp, numrow);
	for (int r = 1; r <= numrow; r++){
		glp_set_row_bnds(lp, r, GLP_UP, 0, subset[r - 1]->weight);
	}
	glp_add_cols(lp, numcol);
	for (int c = 1; c <= numcol; c++){
		glp_set_col_bnds(lp, c, GLP_LO, 0.0, 0);
		glp_set_obj_coef(lp, c, 1);
	}
	for (int r = 1; r <= numrow; r++){
		for (int c = 1; c <= numcol; c++){
			k = c + numcol*(r - 1);
			/* Coefficient of row r, column c
			* 1: if subset r-1 contains element c
			* 0: otherwise
			*/
			contains = subset[r - 1]->hasElement(c) ? 1 : 0;
			ia[k] = r, ja[k] = c, ar[k] = contains;
		}
	}
	glp_load_matrix(lp, numrow*numcol, ia, ja, ar);
	/* solve problem */
	glp_simplex(lp, NULL);
	/* recover results */
	// double val = glp_get_obj_val(lp);
	vector<double> y(numcol);
	for (int c = 1; c <= numcol; c++){
		y[c - 1] = glp_get_col_prim(lp, c);
	}
	/* housekeeping */
	glp_delete_prob(lp);
	glp_free_env();
	delete[] ia;
	delete[] ja;
	delete[] ar;

	/* Rounding: choose a subset if its corresponding constraint is satisfied with equality */
	double totalweight = 0;
	double dualsum = 0;
	Set* s;
	for (int r = 1; r <= numrow; r++){
		s = subset[r - 1];
		dualsum = 0;
		for (Element* e : s->e){
			dualsum += y[(e->id) - 1];
		}
		if (isEqual(s->weight, dualsum)){
			x[r - 1] = 1;
			totalweight += s->weight;
		}
	}
	x[x.size() - 1] = totalweight;
	return x;
}

/* Find optimal set cover via mixed integer programming	*/
vector<double> SetCover::exact_MIP(){
	int numrow = groundset->e.size();
	int numcol = subset.size();
	int k = 1;
	int contains = 0;

	/* Note: GLPK uses 1-based numbering*/
	glp_prob *mip;
	int* ia = new int[1 + numrow*numcol];
	int* ja = new int[1 + numrow*numcol];
	double* ar = new double[1 + numrow*numcol];
	vector<double> x(1 + numcol);

	/* create problem */
	mip = glp_create_prob();
	glp_set_prob_name(mip, "Mixed-Integer Programming solution to Set Cover");
	glp_set_obj_dir(mip, GLP_MIN);

	/* construct matrix for the problem problem */
	glp_add_rows(mip, numrow);
	for (int r = 1; r <= numrow; r++){
		glp_set_row_bnds(mip, r, GLP_LO, 1.0, 0);
	}
	glp_add_cols(mip, numcol);
	for (int c = 1; c <= numcol; c++){
		glp_set_col_kind(mip, c, GLP_BV);
		//glp_set_col_bnds(mip, c, GLP_LO, 0.0, 0);
		glp_set_obj_coef(mip, c, subset[c - 1]->weight);
	}
	for (int r = 1; r <= numrow; r++){
		for (int c = 1; c <= numcol; c++){
			k = c + numcol*(r - 1);
			/* Coefficient of row r, column c
			* 1: if subset c-1 contains element r
			* 0: otherwise
			*/
			contains = subset[c - 1]->hasElement(r) ? 1 : 0;
			ia[k] = r, ja[k] = c, ar[k] = contains;
		}
	}
	glp_load_matrix(mip, numrow*numcol, ia, ja, ar);
	/* solve problem */
	glp_iocp parm;
	glp_init_iocp(&parm);
	parm.presolve = GLP_ON;
	int err = glp_intopt(mip, &parm);

	/* recover results */
	for (int c = 1; c <= numcol; c++){
		x[c - 1] = glp_mip_col_val(mip, c);
	}
	x[x.size() - 1] = glp_mip_obj_val(mip);
	/* housekeeping */
	glp_delete_prob(mip);
	glp_free_env();
	delete[] ia;
	delete[] ja;
	delete[] ar;
	
	return x;
}


/* Find set cover via greedy algorithm
 * H(n)-approximation algorithm
 * H(n) = 1 + 1/2 + ... + 1/n is the nth harmonic number
 */
vector<double> SetCover::approx_greedy(){
	double totalweight = 0;
	double price;
	int notcovered = 0, covered = 0;
	double metric = 0;
	Set* best = nullptr;
	while (!isCovered()){
		price = std::numeric_limits<double>::max();
		best = nullptr;
		for (Set* s : subset){
			notcovered = s->countNotCovered();
			if (notcovered > 0) {
				metric = (s->weight) / notcovered; // metric to decide which set gives the best bang for the buck
				if (price > metric){
					price = metric;
					best = s;
					covered = notcovered;
				}
			}
		}
		best->setToChosen();
		this->covered += covered;
		totalweight += best->weight;
	}

	vector<double> x(1 + subset.size());
	for (size_t i = 0; i < subset.size(); i++){
		x[i] = subset[i]->chosen;
	}
	x[x.size() - 1] = totalweight;
	return x;
}

/* Find set cover via primal-dual algorithm (f-approximation algorithm) */
vector<double> SetCover::approx_primal_dual(){
	double totalweight = 0;
	vector<double> y(n, 0);  // dual variable

	int id = 1;
	double increment = 0;
	double diff = 0;

	Set* best = nullptr;
	while (!isCovered()){
		// Find element that is not covered
		for (Element* e : groundset->e){
			if (e->count == 0){
				id = e->id;
				break;
			}
		}
		// Find the unchosen subset with the least increase in dual variable till constraint is satisfied with equality
		increment = std::numeric_limits<double>::max();
		best = nullptr;
		for (Set* s : subset){
			if ((!(s->chosen)) && s->hasElement(id)){
				diff = s->weight;

				for (int i = 1; i <= n; i++){
					if (s->hasElement(i)){
						diff -= y[i - 1];
					}
				}
				if (diff < increment){
					increment = diff;
					best = s;
				}
			}
		}
		y[id - 1] += increment;
		this->covered += best->countNotCovered();
		best->setToChosen();
		totalweight += best->weight;
	}
	vector<double> x(1 + subset.size());
	for (size_t i = 0; i < subset.size(); i++){
		x[i] = subset[i]->chosen ? 1 : 0;
	}
	x[x.size() - 1] = totalweight;
	return x;
}