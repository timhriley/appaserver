/* permutation.h */
/* ------------- */
/* ==================================================== */
/* Function header for PERMUTATION ADT			*/
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */


#ifndef PERMUTATION_H
#define PERMUTATION_H

typedef struct
{
	int N;
	unsigned int n_factoral;
	int level;
	int *value_array;
	int *permutation_array;
} PERMUTATION;

PERMUTATION *permutation_new(		int N );
void permutation_visit(			int *permutation_array,
					int *value_array,
					int *level,
					int N,
					int k );
void permutation_set_permutation_array(	int *permutation_array,
					int *value_array,
					int N );
unsigned int permutation_get_factoral(	int N );
void permutation_output_permutation_array(
					int *permutation_array,
					unsigned int n_factoral,
					int N );
int *permutation_allocate_permutation_array(
					int N, unsigned int n_factoral );
int permutation_get_single_offset(
					int x,
					int y,
					int y_width );
#endif

