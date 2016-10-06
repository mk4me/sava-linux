#pragma once

#ifndef Hungarian_h__
#define Hungarian_h__

////////////////////////////////////////////////////////////////////////////////////////////////
// HUNGARIAN ALGORITHM
////////////////////////////////////////////////////////////////////////////////////////////////

#define HUNGARIAN_NOT_ASSIGNED 0
#define HUNGARIAN_ASSIGNED 1

#define HUNGARIAN_MODE_MINIMIZE_COST   0
#define HUNGARIAN_MODE_MAXIMIZE_UTIL 1

typedef struct {
    int num_rows;
    int num_cols;
    float** cost;
    int** assignment;
} hungarian_problem_t;

/** This method initialize the hungarian_problem structure and init
 *  the  cost matrices (missing lines or columns are filled with 0).
 *  It returns the size of the quadratic(!) assignment matrix. **/
int hungarian_init( hungarian_problem_t* p,
                    float** cost_matrix,
                    int rows,
                    int cols,
                    int mode );

/** Free the memory allocated by init. **/
void hungarian_free( hungarian_problem_t* p );

/** This method computes the optimal assignment. **/
void hungarian_solve( hungarian_problem_t* p );

/** Print the computed optimal assignment. **/
void hungarian_print_assignment( hungarian_problem_t* p );

/** Print the cost matrix. **/
void hungarian_print_costmatrix( hungarian_problem_t* p );

/** Print cost matrix and assignment matrix. **/
void hungarian_print_status( hungarian_problem_t* p );


#endif // Hungarian_h__
