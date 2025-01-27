/* library/covariance.h */
/* -------------------- */
/* Tim Riley    	*/
/* -------------------- */

#ifndef COVARIANCE_H
#define COVARIANCE_H
#include "list.h"
#include "hash_table.h"

typedef struct
{
	char *date_time_key;
	double value;
} COVARIANCE_RECORD;

typedef struct
{
	char *datatype_name;
	HASH_TABLE *covariance_record_hash_table;
} COVARIANCE_DATATYPE;

typedef struct
{
	COVARIANCE_DATATYPE *anchor_datatype_group;
	LIST *compare_covariance_datatype_list;
} COVARIANCE;

COVARIANCE *covariance_new_covariance(	char *anchor_datatype_group_name );
COVARIANCE_DATATYPE *covariance_new_covariance_datatype(
					char *datatype_name );
COVARIANCE_DATATYPE *covariance_set_covariance_datatype(
					LIST *compare_covariance_datatype_list,
					char *datatype_name );
int covariance_datatype_compare(	COVARIANCE_DATATYPE *c1,
					char *compare_datatype_name );
void covariance_set_double(		COVARIANCE_DATATYPE *
						anchor_datatype_group,
					LIST *compare_covariance_datatype_list,
					char *datatype_name,
					char *date_time_key,
					double value );
int covariance_get_results(		double *covariance,
					int *count,
					double *average,
					double *standard_deviation,
					double *correlation,
					COVARIANCE_DATATYPE *
						anchor_datatype_group,
					LIST *compare_covariance_datatype_list,
					char *datatype_name );
int covariance_get_average(	double *average,
				int *count,
				HASH_TABLE *covariance_record_hash_table,
				LIST *date_time_key_list );
COVARIANCE_RECORD *covariance_new_covariance_record(
					char *date_time_key,
					double value );
double covariance_get_sum_of_products(
		double anchor_average,
		double compare_average,
		HASH_TABLE *anchor_datatype_group_covariance_record_hash_table,
		HASH_TABLE *compare_datatype_covariance_record_hash_table,
		LIST *anchor_date_time_key_list );
#endif
