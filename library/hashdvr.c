/* library/hashdvr.c			 */
/* ------------------------------------- */
/* Test drive program for hash table ADT */
/* ------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

typedef struct
{
	char *trading_account_num;
	char *invest_account_num;
	char *csc_rep_num;
} OTHER_DATA;

typedef struct
{
	char *energy_value_key;
	double energy_level;
	void *wang_landau_information;
} ENERGY_LEVEL;

ENERGY_LEVEL *energy_level_new_energy_level( double energy_level_double );
char *energy_level_get_energy_value_key( double energy_level );
void test_energy_level( void );

void test_1( void );
void test_3( void );
void test_variable( void );
void test_duplicate_indicator( void );

int main()
{
	/* test_1(); */
	/* test_3(); */
	test_duplicate_indicator();
	/* test_variable(); */
	/* test_energy_level(); */
	return 0;
} /* main() */

ENERGY_LEVEL *energy_level_new_energy_level( double energy_level_double )
{
	ENERGY_LEVEL *energy_level;

	energy_level = (ENERGY_LEVEL *)calloc( 1, sizeof( ENERGY_LEVEL ) );
	energy_level->energy_level = energy_level_double;
	energy_level->energy_value_key =
			strdup( energy_level_get_energy_value_key(
				energy_level->energy_level ) );
	return energy_level;
}

char *energy_level_get_energy_value_key( double energy_level )
{
	static char energy_value_key[ 128 ];

	sprintf(	energy_value_key,
			"%.5lf",
			energy_level );
	return energy_value_key;
}

void test_energy_level()
{
	char *energy_value_key;
	ENERGY_LEVEL *energy_level;
	HASH_TABLE *energy_level_hash_table =
		hash_table_new_hash_table( hash_table_large );

	energy_level = energy_level_new_energy_level( 1.234 );

	hash_table_insert(	energy_level_hash_table,
				energy_level->energy_value_key,
				(char *)energy_level );

	energy_level = energy_level_new_energy_level( 5.678 );

	hash_table_insert(	energy_level_hash_table,
				energy_level->energy_value_key,
				(char *)energy_level );

	energy_value_key = energy_level_get_energy_value_key( 1.234 );

	energy_level = (ENERGY_LEVEL *)
				hash_table_fetch( 
					energy_level_hash_table,
					energy_value_key );
	if ( !energy_level )
		printf( "Wrong. Item not found\n" );
	else
		printf( "Good. Found energy_level = %s\n",
			energy_level->energy_value_key );

	energy_value_key = energy_level_get_energy_value_key( 5.678 );

	energy_level = (ENERGY_LEVEL *)
				hash_table_fetch( 
					energy_level_hash_table,
					energy_value_key );
	if ( !energy_level )
		printf( "Wrong. Item not found\n" );
	else
		printf( "Good. Found energy_level = %s\n",
			energy_level->energy_value_key );

	energy_value_key = energy_level_get_energy_value_key( 9.123 );

	energy_level = (ENERGY_LEVEL *)
				hash_table_fetch( 
					energy_level_hash_table,
					energy_value_key );
	if ( !energy_level )
		printf( "Good. Item %s not found\n", energy_value_key );
	else
		printf( "Wrong. Thinks found energy_level = %s\n",
			energy_value_key );

	hash_table_free( energy_level_hash_table );

} /* test_energy_level() */

void test_1()
{
	OTHER_DATA *item;
	HASH_TABLE *h = hash_table_init( hash_table_small );
	int duplicate_indicator;

	item = (OTHER_DATA *)calloc( 1, sizeof( OTHER_DATA ) );
	item->trading_account_num = "E5-00000";
	item->invest_account_num = "I00000";
	item->csc_rep_num = "R0";
	hash_table_insert( h, item->trading_account_num, (char *)item );

	item = (OTHER_DATA *)calloc( 1, sizeof( OTHER_DATA ) );
	item->trading_account_num = "E5-00001";
	item->invest_account_num = "I00001";
	item->csc_rep_num = "R1";
	hash_table_insert( h, item->trading_account_num, (char *)item );

	item = (OTHER_DATA *)calloc( 1, sizeof( OTHER_DATA ) );
	item->trading_account_num = "E5-00002";
	item->invest_account_num = "I00002";
	item->csc_rep_num = "R2";
	hash_table_insert( h, item->trading_account_num, (char *)item );

	item = (OTHER_DATA *)hash_table_retrieve_other_data( 
					h, 
					"E5-00000",
					&duplicate_indicator );
	if ( !item )
		printf( "Wrong. Item not found\n" );
	else
		printf( "E5-00000 has IC# %s and csc_rep_num %s\n",
			item->invest_account_num,
			item->csc_rep_num );

	item = (OTHER_DATA *)hash_table_retrieve_other_data( 
					h, "E5-00001", &duplicate_indicator );
	if ( !item )
		printf( "Wrong. Item not found\n" );
	else
		printf( "E5-00001 has IC# %s and csc_rep_num %s\n",
			item->invest_account_num,
			item->csc_rep_num );

	item = (OTHER_DATA *)hash_table_retrieve_other_data( 
					h, "E5-00009", &duplicate_indicator );
	if ( !item )
		printf( "Good. E5-00009 is not found.\n" );
	else
		printf( "Wrong. Returned E5-00009 is found.\n" );

	hash_table_free( h );

} /* test_1() */

void test_3()
{
	OTHER_DATA *item;
	HASH_TABLE *h = hash_table_init( hash_table_small );
	int duplicate_indicator;

	item = (OTHER_DATA *)calloc( 1, sizeof( OTHER_DATA ) );
	item->trading_account_num = "E5-00000";
	item->invest_account_num = "I00000";
	item->csc_rep_num = "R0";

	hash_table_insert( h, item->trading_account_num, (char *)item );

	item = (OTHER_DATA *)hash_table_retrieve_other_data( 
					h, 
					"E5-00000",
					&duplicate_indicator );
	if ( !item )
		printf( "Wrong. Item not found\n" );
	else
		printf( "E5-00000 has IC# %s and csc_rep_num %s\n",
			item->invest_account_num,
			item->csc_rep_num );

	hash_table_free( h );

} /* test_3() */

void test_duplicate_indicator()
{
	OTHER_DATA *item;
	HASH_TABLE *h = hash_table_init( hash_table_small );
	int duplicate_indicator;

	/* Insert an item */
	/* -------------- */
	item = (OTHER_DATA *)calloc( 1, sizeof( OTHER_DATA ) );
	item->trading_account_num = "E5-00000";
	item->invest_account_num = "I00000";
	item->csc_rep_num = "R0";
	hash_table_insert( h, item->trading_account_num, (char *)item );

	item = (OTHER_DATA *)hash_table_retrieve_other_data( 
					h, 
					"E5-00000",
					&duplicate_indicator );
	if ( duplicate_indicator )
		printf( "Wrong. Duplicate indicator should not be set.\n" );
	else
		printf(
		"Good. Duplicate indicator is not set. Got account = (%s)\n",
			item->invest_account_num );

	/* Insert the same item */
	/* -------------------- */
	item = (OTHER_DATA *)calloc( 1, sizeof( OTHER_DATA ) );
	item->trading_account_num = "E5-00000";
	item->invest_account_num = "I00001";
	item->csc_rep_num = "R1";
	hash_table_insert( h, item->trading_account_num, (char *)item );

	item = (OTHER_DATA *)hash_table_retrieve_other_data( 
					h, 
					"E5-00000",
					&duplicate_indicator );
	if ( duplicate_indicator )
		printf(
"Good. Duplicate indicator found the duplication. Got account = (%s)\n",
			item->invest_account_num );
	else
		printf( "Wrong. Duplicate indicator should be set.\n" );

	hash_table_free( h );

} /* test_duplicate_indicator() */



typedef struct
{
	char *key;
	char *other_data;
} VARIABLE;

void test_variable()
{
	VARIABLE *a;
	HASH_TABLE *h = hash_table_init( hash_table_small );
	int duplicate_indicator;

	a = (VARIABLE *)calloc( 1, sizeof( VARIABLE ) );
	a->key = "x";
	a->other_data = "5";

	hash_table_insert( h, a->key, (char *)a );

	a = (VARIABLE *)hash_table_retrieve_other_data( 
					h, 
					"x",
					&duplicate_indicator );
	if ( !a )
		printf( "Wrong! variable (%s) is not found.\n", "x" );
	else
		printf( "%s=%s\n",
			a->key,
			a->other_data );

	hash_table_free( h );

} /* test_variable() */

