Approximation Algorithms
========================
The project currently contains various approximation algorithms for the weighted set cover problem. 

Set Cover
---------
The algorithms implemented for the weighted set cover are:
- LP-rounding (f-approximation)
- Dual-rounding (f-approximation)
- Primal-dual (f-approximation)
- Greedy (H(n)-approximation)
- Exact (using mixed-integer programming)

f: Maximum number of times an element appears in any subset  
H(n): nth harmonic number = 1 + 1/2 + ... + 1/n

Input
-----
Input file is "set-cover-input" with the following format:

```
10 
6
1 5 7 8 9 10
1 5 7 2
1 3 6 10
1 5 6 7 9 10
1 1 6 4 8 10
1 2 4 1 3 9
```
First line: Number of ground elements  (10 in the example)  
Second line: Number of sets (6 in the example)  
Third line onwards: Weight (1) + IDs of elements in set (5 7 8 9 10)

The ground element ID is numbered as 1,2,...,10 in the example.


Dependencies
-----------
The program uses the linear programming and mixed-integer programming solver from the [GNU Linear Programming Kit] \(GLPK\).

Testing is done using [Catch].

GLPK is needed for the program to execute correctly while both GLPK and Catch are needed for the test file.

Installation
-----------
```
git clone https://github.com/cllim/approximation-algorithms/
cd approximation-algorithms/set-cover
make
set-cover "option"
```
Replace "option" with "LP-rounding", "dual_rounding", "primal_dual", "greedy", or "exact" to invoke the corresponding algorithm (without the inverted commas). Defaults to the exact MIP solver if nothing is specified.

To run the test file instead, do
```
make test
test
```

License
-------
MIT

References
----------
Williamson, David P., and David B. Shmoys. *The design of approximation algorithms*. Cambridge University Press, 2011.

Vazirani, Vijay V. *Approximation algorithms*. Springer, 2001.

Note: Free electronic copy of these books could be found online

[GNU Linear Programming Kit]:https://www.gnu.org/software/glpk/ 
[Catch]:https://github.com/philsquared/Catch
