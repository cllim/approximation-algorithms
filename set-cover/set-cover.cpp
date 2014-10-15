#include "set-cover.h"
#include "element.h"
#include "set.h"
#include "glpk.h"
#include <assert.h>
#include <vector>
#include <limits>

using std::vector;

// helper function
bool isEqual(double a, double b){	
	double x = a > b ? a - b : b - a;
	return x < 1e-8;
}

SetCover::SetCover(Set* g, vector<Set*> s) : groundset(g), subset(s), covered(0) {
	setf();
	n = g->e.size();
}

bool SetCover::isCovered(){
	for (Element* e : groundset->e){
		if (e->count == 0)
			return false;
	}
	return true; // n == covered;
}

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

void SetCover::reset(){
	for (Set* s : subset){
		s->chosen = false;
	}
	for (Element* e : groundset->e){
		e->count = 0;
	}
	covered = 0;
}

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

vector<double> SetCover::approx_LP_rounding(){
	int numrow = n;
	int numcol = subset.size();
	int k = 1;
	int contains = 0;

	/* Note: GLPK uses 1-based numbering*/
	glp_prob *lp;
	int ia[1 + 1000], ja[1 + 1000];
	double ar[1 + 1000], val;
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
	val = glp_get_obj_val(lp);
	for (int c = 1; c <= numcol; c++){
		x[c - 1] = glp_get_col_prim(lp, c);
	}
	/* housekeeping */
	glp_delete_prob(lp);
	glp_free_env();

	/* Rounding */
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


vector<double> SetCover::approx_dual_rounding(){
	int numcol = groundset->e.size();
	int numrow = subset.size();
	int k = 1;
	int contains = 0;

	/* Note: GLPK uses 1-based numbering*/
	glp_prob *lp;
	int ia[1 + 1000], ja[1 + 1000];
	double ar[1 + 1000], val;
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
	val = glp_get_obj_val(lp);
	vector<double> y(numcol);
	for (int c = 1; c <= numcol; c++){
		y[c - 1] = glp_get_col_prim(lp, c);
	}
	/* housekeeping */
	glp_delete_prob(lp);
	glp_free_env();

	/* Rounding */
	double totalweight = 0;
	double dualsum = 0;
	Set* s;
	for (int r = 1; r <= numrow; r++){
		s = subset[r - 1];
		dualsum = 0;
		for (Element* e : s->e){
			dualsum += y[(e->id)-1];
		}
		if (isEqual(s->weight, dualsum)){
			x[r - 1] = 1;
			totalweight += s->weight;
		}
	}
	x[x.size() - 1] = totalweight;
	return x;
}

vector<double> SetCover::exact_MIP(){
	int numrow = groundset->e.size();
	int numcol = subset.size();
	int k = 1;
	int contains = 0;

	/* Note: GLPK uses 1-based numbering*/
	glp_prob *mip;
	int ia[1 + 1000], ja[1 + 1000];
	double ar[1 + 1000], val;
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
	val = glp_mip_obj_val(mip);
	for (int c = 1; c <= numcol; c++){
		x[c - 1] = glp_mip_col_val(mip, c);
	}
	/* housekeeping */
	glp_delete_prob(mip);
	glp_free_env();

	double totalweight = 0;
	for (int c = 1; c <= numcol; c++){
		if (x[c - 1])
			totalweight += subset[c - 1]->weight;
	}
	x[x.size() - 1] = totalweight;
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