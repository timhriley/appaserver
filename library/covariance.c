/* $APPASERVER_HOME/library/covariance.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "covariance.h"
#include "list.h"
#include "piece.h"
#include "hash_table.h"

COVARIANCE_RECORD *covariance_new_covariance_record(
					char *date_time_key,
					double value )
{
	COVARIANCE_RECORD *c;

	c = calloc( 1, sizeof( COVARIANCE_RECORD ) );
	c->date_time_key = date_time_key;
	c->value = value;
	return c;
}

COVARIANCE *covariance_new_covariance( char *anchor_datatype_group_name )
{
	COVARIANCE *c;

	c = calloc( 1, sizeof( COVARIANCE ) );
	c->anchor_datatype_group =
		covariance_new_covariance_datatype(
			anchor_datatype_group_name );

	c->compare_covariance_datatype_list = list_new();

	/* Insure that the anchor is the first to output */
	/* --------------------------------------------- */
	covariance_set_covariance_datatype(
		c->compare_covariance_datatype_list,
		anchor_datatype_group_name );

	return c;
}

COVARIANCE_DATATYPE *covariance_new_covariance_datatype(
						char *datatype_name )
{
	COVARIANCE_DATATYPE *c;

	c = calloc( 1, sizeof( COVARIANCE_DATATYPE ) );
	c->datatype_name = datatype_name;
	c->covariance_record_hash_table = hash_table_new( hash_table_medium );
	return c;
}

COVARIANCE_DATATYPE *covariance_set_covariance_datatype(
					LIST *compare_covariance_datatype_list,
					char *datatype_name )
{
	COVARIANCE_DATATYPE *covariance_datatype;

	if ( list_item_exists(	compare_covariance_datatype_list,
				datatype_name,
				covariance_datatype_compare ) )
	{
		return
		list_get(
			compare_covariance_datatype_list );
	}
	covariance_datatype =
		covariance_new_covariance_datatype( datatype_name );

	list_set(	compare_covariance_datatype_list,
			covariance_datatype );

	return covariance_datatype;
}

int covariance_datatype_compare(	COVARIANCE_DATATYPE *c1,
					char *compare_datatype_name )
{
	return strcasecmp( c1->datatype_name, compare_datatype_name );
}

void covariance_set_double(	COVARIANCE_DATATYPE *anchor_datatype_group,
				LIST *compare_covariance_datatype_list,
				char *datatype_name,
				char *date_time_key,
				double value )
{
	COVARIANCE_DATATYPE *compare_datatype;
	COVARIANCE_RECORD *covariance_record;

	if ( !date_time_key || !*date_time_key ) date_time_key = "null";

	covariance_record =
		covariance_new_covariance_record(
			date_time_key, value );

	if ( strcasecmp(	anchor_datatype_group->datatype_name,
				datatype_name ) == 0 )
	{
		hash_table_set_pointer(
			anchor_datatype_group->covariance_record_hash_table,
			covariance_record->date_time_key,
			(char *)covariance_record );
	}

	compare_datatype =
		covariance_set_covariance_datatype(
				compare_covariance_datatype_list,
				datatype_name );

	hash_table_add_pointer(	compare_datatype->covariance_record_hash_table,
				covariance_record->date_time_key,
				covariance_record );
}

int covariance_get_statistics(	double *average,
				int *count,
				double *standard_deviation,
				HASH_TABLE *covariance_record_hash_table,
				LIST *date_time_key_list )
{
	char sys_string[ 1024 ];
	LIST *results_list;
	char *results_string;
	char *date_time_key;
	FILE *p;
	char tmp_file[ 128 ];
	char statistics_identifier[ 32 ];
	char statistics_results[ 32 ];
	COVARIANCE_RECORD *covariance_record;

	*average = 0.0;
	*count = 0;
	*standard_deviation = 0.0;

	if ( !list_rewind( date_time_key_list ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s(): no date_time_key list\n",
			 __FILE__,
			 __FUNCTION__ );
		return 0;
	}

	sprintf( tmp_file, "/tmp/covariance_%d.dat", getpid() );
	sprintf( sys_string,
		 "statistics_weighted.e ',' > %s", tmp_file );

	p = popen( sys_string, "w" );
		 
	do {
		date_time_key = list_get( date_time_key_list );

		covariance_record =
			(COVARIANCE_RECORD *)
				hash_table_get_pointer(
					covariance_record_hash_table,
					date_time_key );

		fprintf( p, "%lf,1\n", covariance_record->value );

	} while( list_next( date_time_key_list ) );

	pclose( p );

	sprintf( sys_string, "cat %s", tmp_file );
	results_list = pipe2list( sys_string );

	if ( !list_rewind( results_list ) ) return 0;

	do {
		results_string = list_get( results_list );

		piece( statistics_identifier, ':', results_string, 0 );
		piece( statistics_results, ':', results_string, 1 );

		if ( strcmp( statistics_identifier, "Average" ) == 0 )
		{
			*average = atof( statistics_results );
		}
		else
		if ( strcmp( statistics_identifier, "Standard Deviation" ) == 0)
		{
			*standard_deviation = atof( statistics_results );
		}
		else
		if ( strcmp( statistics_identifier, "Count" ) == 0)
		{
			*count = atoi( statistics_results );
		}

	} while( list_next( results_list ) );

	sprintf( sys_string, "rm %s", tmp_file );
	if ( system( sys_string ) ){}

	return 1;
}

double covariance_get_sum_of_products(
		double anchor_average,
		double compare_average,
		HASH_TABLE *anchor_datatype_group_covariance_record_hash_table,
		HASH_TABLE *compare_datatype_covariance_record_hash_table,
		LIST *anchor_date_time_key_list )
{
	double results = 0;
	COVARIANCE_RECORD *anchor_covariance_record;
	COVARIANCE_RECORD *compare_covariance_record;
	char *date_time_key;

	if ( !list_rewind( anchor_date_time_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): empty anchor_date_time_key_list\n",
			__FILE__,
			__FUNCTION__ );
		exit( 1 );
	}

	do {
		date_time_key = list_get( anchor_date_time_key_list );

		anchor_covariance_record =
			hash_table_get_pointer(
			anchor_datatype_group_covariance_record_hash_table,
			date_time_key );

		compare_covariance_record =
			hash_table_get_pointer(
				compare_datatype_covariance_record_hash_table,
				date_time_key );

		if ( compare_covariance_record )
		{
			results +=	( anchor_covariance_record->value -
						anchor_average ) *
					( compare_covariance_record->value -
						compare_average );
		}
	} while( list_next( anchor_date_time_key_list ) );

	return results;

}

int covariance_get_results(	double *covariance,
				int *count,
				double *average,
				double *standard_deviation,
				double *correlation,
				COVARIANCE_DATATYPE *anchor_datatype_group,
				LIST *compare_covariance_datatype_list,
				char *datatype_name )
{
	COVARIANCE_DATATYPE *compare_datatype;
	double anchor_average;
	int anchor_count;
	double anchor_standard_deviation;
	LIST *anchor_date_time_key_list;
	LIST *compare_date_time_key_list;
	LIST *date_time_key_list;

	if ( list_item_exists(	compare_covariance_datatype_list,
				datatype_name,
				covariance_datatype_compare ) )
	{
		compare_datatype =
			list_get(
				compare_covariance_datatype_list );
	}
	else
	{
		fprintf( stderr,
			 "%s/%s(): cannot find datatype = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 datatype_name );
		return 0;
	}

	anchor_date_time_key_list =
		hash_table_get_key_list(
			anchor_datatype_group->covariance_record_hash_table );

	compare_date_time_key_list =
		hash_table_get_key_list(
			compare_datatype->covariance_record_hash_table );

	date_time_key_list =
		list_intersect_string_list(
			anchor_date_time_key_list,
			compare_date_time_key_list );

	if ( list_length( date_time_key_list ) )
	{
		covariance_get_statistics(
				average,
				count,
				standard_deviation,
				compare_datatype->covariance_record_hash_table,
				date_time_key_list );
	}
	else
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: datatype = %s vs. %s: there are no common date/time combinations.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 anchor_datatype_group->datatype_name,
			 compare_datatype->datatype_name );
		*count = 0;
	}

	if ( *count <= 1 ) return 0;

	covariance_get_statistics(
			&anchor_average,
			&anchor_count,
			&anchor_standard_deviation,
			anchor_datatype_group->covariance_record_hash_table,
			date_time_key_list );

	if ( anchor_count != *count )
	{
		fprintf( stderr,
"ERROR in %s/%s(): anchor = %s and compare = %s counts don't match %d != %d\n",
			__FILE__,
			__FUNCTION__,
			anchor_datatype_group->datatype_name,
			compare_datatype->datatype_name,
			anchor_count,
			*count );
		return 0;
	}

	if ( anchor_count > 1 )
	{
		*covariance =
			covariance_get_sum_of_products(
				anchor_average,
				*average,
				anchor_datatype_group->
					covariance_record_hash_table,
				compare_datatype->
					covariance_record_hash_table,
				date_time_key_list ) /
				( anchor_count - 1 );
	}
	else
	{
		*covariance = 0.0;
	}

	if ( *standard_deviation && anchor_standard_deviation )
	{
		*correlation =	*covariance /
				( *standard_deviation *
				anchor_standard_deviation );
	}
	else
	{
		*correlation = 0.0;
	}
	return 1;
}

