/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/keys_match_sum.c		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef KEYS_MATCH_SUM
#define KEYS_MATCH_SUM

LIST *keys_match_sum_get_match_sum_key_list(
		LIST *key_list,
		LIST *value_double_list,
		double match_sum );

LIST *keys_match_sum_dereference_array_index_list(
		LIST *array_index_list,
		LIST *key_list );

/* ------------------------------------------------------------ */
/* Sample binary_count_string: 00000000000000000000000000000011 */
/* ------------------------------------------------------------ */
LIST *keys_match_sum_get_double_array_match_sum_key_list(
		char *binary_count_string,
		double match_sum,
		double *double_array,
		int length,
		LIST *key_list );

/* ------------------------------------------------------------- */
/* Sample binary_count_reverse: 11000000000000000000000000000000 */
/* ------------------------------------------------------------- */
LIST *keys_match_sum_get_binary_reverse_array_index_list(
		char *binary_count_reverse,
		double match_sum,
		double *double_array,
		int length );

#endif
