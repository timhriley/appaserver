/* library/variable.c */
/* ------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "variable.h"
#include "hash_table.h"

VARIABLE *variable_init()
{
	VARIABLE *v;

	v = (VARIABLE *)calloc( 1, sizeof( VARIABLE ) );
	if ( !v )
	{
		fprintf( stderr, 
			 "variable_init(): Memory allocation error.\n" );
		exit( 1 );
	}

	v->hash_table = hash_table_init();

	return v;

} /* variable_init() */



variable_free( v )
VARIABLE *v;
{
	hash_table_free( v->hash_table );
	free( v );
	return 1;
}


int set_name( v, name )
VARIABLE *v;
char *name;
{
	return hash_table_insert(  v->hash_table, 
				   name			/* key        */,
				   "" 			/* other_data */ );
}

int set_variable( VARIABLE *v, char *name, char *value )
{
	char *other_data;
	int duplicate_indicator;
	int return_value;

	other_data = hash_table_retrieve_other_data(
					v->hash_table,
					name,
					&duplicate_indicator );

	if ( !other_data ) set_name( v, name );

	return_value = hash_table_insert_back(
				v->hash_table, 
				name		/* key        */,
				value		/* other_data */ );

	return return_value;

} /* set_variable() */


char *get_value( VARIABLE *v, char *name )
{
	return get_variable( v, name );
}

char *get_variable( VARIABLE *v, char *name )
{
	char *other_data;
	int duplicate_indicator;

	other_data = hash_table_retrieve_other_data(
					v->hash_table,
					name,
					&duplicate_indicator );

	if ( !other_data )
		return (char *)0;
	else
		return other_data;

} /* get_variable() */

void display_variable_table( VARIABLE *v, char *output_file )
{
	display_hash_table( v->hash_table, output_file );
}


char *replace_all_variables( char *destination, char *source, VARIABLE *v )
{
	char search_string[ 128 ];
	char *replace_string;
	char *search_ptr, *replace_ptr;
	char *ptr;
	int more_to_do = 1;

	strcpy( destination, source );

	while( more_to_do )
	{
		more_to_do = 0;
		ptr = destination;

		while( *ptr )
		{
			/* If a colon followed by a letter */
			/* ------------------------------- */
			if ( *ptr == ':' 
			&&   isalpha( *(ptr + 1 ) ) )
			{
				/* Source in function.c */
				/* -------------------- */
				if ( is_function( ptr ) )
				{
					ptr++;
					continue;
				}

				search_ptr = search_string;

				/* Copy the colon */
				/* -------------- */
				*search_ptr++ = *ptr++;

				while( *ptr )
				{
					if ( !isalnum( *ptr ) && *ptr != '_' )
					{
						break;
					}
					else
					{
						*search_ptr++ = *ptr++;
					}
				}
				*search_ptr = '\0';

				/* Get the contents (skip the colon) */
				/* --------------------------------- */
				replace_string = 
					get_variable( v, search_string + 1 );

				if ( !replace_string )
				{
					search_replace(	search_string, 
							"NULL", 
							destination );
					return destination;
				}

				search_replace(	search_string, 
						replace_string, 
						destination );

				more_to_do = 1;
				break;
			}
			ptr++;
		}
	}

	return destination;

} /* replace_all_variables() */



void variable_to_memory( char *key[], char *data[], VARIABLE *v )
{
	int i, count;
	LIST *list;
	CELL *cell_ptr;

	for ( count = 0, i = 0; i < MAX_CELLS; i++ )
	{
		if ( v->hash_table->cell_array[ i ] )
		{
			list = v->hash_table->cell_array[ i ];
			go_head( list );
			do
			{
				if ( count == MAX_CELLS )
				{
					fprintf(stderr,
		"variable_to_memory() exceeded max of %d\n", MAX_CELLS );
					exit( 1 );
				}

				cell_ptr = (CELL *)retrieve_item_ptr( list );
				key[ count ] = cell_ptr->key;
				data[ count ] = cell_ptr->other_data;
				count++;
			} while( next_item( list ) );
		}
	}

	key[ count ] = (char *)0;
	data[ count ] = (char *)0;

} /* variable_to_memory() */


