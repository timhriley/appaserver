/* library/pivot_table.c				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include "pivot_table.h"
#include "timlib.h"
#include "piece.h"
#include <stdlib.h>
#include <string.h>

boolean pivot_table_set_string(	FILE *output_pipe,
				int *anchor_column_count,
				char *prior_anchor_string,
				LIST *datatype_list,
				char delimiter,
				char *string )
{
	char anchor_string[ PIVOT_TABLE_ANCHOR_STRING_SIZE ];
	char datatype_name[ 128 ];
	char value_string[ 128 ];
	PIVOT_TABLE_DATATYPE *datatype;

	/* If first time */
	/* ------------- */
	if ( !*anchor_column_count )
	{
		int column_count;

		column_count = character_count( delimiter, string ) + 1;

		if ( column_count < 3 )
		{
			fprintf( stderr,
"Error in %s/%s()/%d: expecting at least 3 columns with delimiter = (%c) in (%s)\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	delimiter,
			 	string );
			exit( 1 );
		}

		*anchor_column_count = column_count - 2;

		piece_multiple(
			anchor_string,
			delimiter, 
			string,
			*anchor_column_count );

		/* If missing datatype */
		/* ------------------- */
		if ( !piece(		datatype_name,
					delimiter,
					string,
					*anchor_column_count )
		||   !*datatype_name )
		{
			fprintf(	output_pipe,
					"%s%c\n",
					anchor_string,
					delimiter );
			*prior_anchor_string = '\0';
			*anchor_column_count = 0;

			/* No extra output needed. */
			/* ----------------------- */
			return 0;
		}
		else
		/* --------------- */
		/* If has datatype */
		/* --------------- */
		{
			if ( ( datatype = pivot_table_seek_datatype(
							datatype_list,
							datatype_name ) ) )
			{
				piece(		value_string,
						delimiter,
						string,
						(*anchor_column_count) + 1 );

				datatype->value = strdup( value_string );
			}

			timlib_strcpy(	prior_anchor_string,
					anchor_string,
					PIVOT_TABLE_ANCHOR_STRING_SIZE );

			/* Extra output needed. */
			/* -------------------- */
			return 1;
		}

	} /* If first time */

	piece_multiple(	anchor_string,
			delimiter, 
			string,
			*anchor_column_count );

	/* If missing datatype */
	/* ------------------- */
	if ( !piece(		datatype_name,
				delimiter,
				string,
				*anchor_column_count )
	||   !*datatype_name )
	{
		pivot_table_output(	output_pipe,
					prior_anchor_string,
					datatype_list,
					delimiter );

		fprintf(	output_pipe,
				"%s\n",
				anchor_string );

		*prior_anchor_string = '\0';
		*anchor_column_count = 0;
		pivot_table_cleanup( datatype_list );

		return 0;
	}

	/* If a new group */
	/* -------------- */
	if ( timlib_strcmp(	anchor_string,
				prior_anchor_string ) != 0 )
	{
		pivot_table_output(	output_pipe,
					prior_anchor_string,
					datatype_list,
					delimiter );

		pivot_table_cleanup(	datatype_list );

		timlib_strcpy(	prior_anchor_string,
				anchor_string,
				PIVOT_TABLE_ANCHOR_STRING_SIZE );
	}

	if ( ( datatype = pivot_table_seek_datatype(
					datatype_list,
					datatype_name ) ) )
	{
		piece(		value_string,
				delimiter,
				string,
				(*anchor_column_count) + 1 );

		datatype->value = strdup( value_string );
	}

	/* Extra output needed. */
	/* -------------------- */
	return 1;
}

PIVOT_TABLE *pivot_table_calloc( void )
{
	PIVOT_TABLE *pivot_table;

	if ( ! ( pivot_table = calloc( 1, sizeof( PIVOT_TABLE ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return pivot_table;
}

PIVOT_TABLE *pivot_table_new( char delimiter )
{
	PIVOT_TABLE *pivot_table = pivot_table_calloc();

	pivot_table->delimiter = delimiter;
	pivot_table->datatype_list = list_new();
	return pivot_table;
}

PIVOT_TABLE_DATATYPE *pivot_table_datatype_new( char *datatype_name )
{
	PIVOT_TABLE_DATATYPE *pivot_table_datatype;

	if ( ! ( pivot_table_datatype =
		calloc( 1, sizeof( PIVOT_TABLE_DATATYPE ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	pivot_table_datatype->datatype_name = datatype_name;

	return pivot_table_datatype;
}

void pivot_table_set_datatype(
				LIST *datatype_list,
				char *datatype_name )
{
	PIVOT_TABLE_DATATYPE *pivot_table_datatype;

	pivot_table_datatype = pivot_table_datatype_new(
				datatype_name );

	list_append_pointer( datatype_list, pivot_table_datatype );
}

void pivot_table_cleanup(	LIST *datatype_list )
{
	PIVOT_TABLE_DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) ) return;

	do {
		datatype = list_get_pointer( datatype_list );

		if ( datatype->value ) free( datatype->value );
		datatype->value = (char *)0;
	} while( list_next( datatype_list ) );
}

PIVOT_TABLE_DATATYPE *pivot_table_seek_datatype(
				LIST *datatype_list,
				char *datatype_name )
{
	PIVOT_TABLE_DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) ) return (PIVOT_TABLE_DATATYPE *)0;

	do {
		datatype = list_get_pointer( datatype_list );

		if ( timlib_strcmp(	datatype->datatype_name,
					datatype_name ) == 0 )
		{
			return datatype;
		}
	} while( list_next( datatype_list ) );

	return (PIVOT_TABLE_DATATYPE *)0;
}

void pivot_table_output(	FILE *output_pipe,
				char *prior_anchor_string,
				LIST *datatype_list,
				char delimiter )
{
	PIVOT_TABLE_DATATYPE *datatype;

	if ( !prior_anchor_string || !*prior_anchor_string ) return ;

/*
fprintf( stderr, "%s/%s()/%d: got anchor_string = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
prior_anchor_string );
*/

	if ( !list_rewind( datatype_list ) ) return;

	fprintf( output_pipe, "%s", prior_anchor_string );

	do {
		datatype = list_get_pointer( datatype_list );

		fprintf( output_pipe, "%c", delimiter );

		if ( datatype->value )
		{
			fprintf(output_pipe,
				"%s",
				datatype->value );
		}

	} while( list_next( datatype_list ) );

	fprintf( output_pipe, "\n" );
}

