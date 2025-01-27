/* invest2:$root/library/tim/variable.c */
/* ----------------------------------- */

#include <stdio.h>
#include "variable.h"

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

int set_variable( v, name, value )
VARIABLE *v;
char *name;
char *value;
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
}



char *get_value( v, name )
VARIABLE *v;
char *name;
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
}

void display_variable_table( VARIABLE *v, char *output_file )
{
	display_hash_table( v->hash_table, output_file );
}


char *replace_all_variables( char *destination, char *s, VARIABLE *v )
{
	char *d = destination;

	while( *s )
	{
		/* --------------------------------------- */
		/* If not a colon followed by a non space, */
		/* or a colon followed by the end of line, */
		/* then simply copy it over.		   */
		/* --------------------------------------- */
		if ( ( *s != ':' ) 
		||   ( isspace( *(s + 1) ) ) 
		||   ( !*(s + 1) ) )
		{
			*d++ = *s++;
		}
		else
		/* ----------------------------------- */
		/* Otherwise, extract the variable and */
		/* convert it to it's contents.        */
		/* ----------------------------------- */
		{
			char variable[ 128 ];
			char *variable_ptr = variable;
			char *contents;

			/* Copy to variable */
			/* ---------------- */
			s++;
			while( *s )
				if ( isalnum( *s ) 
				||   *s == '_' )
					*variable_ptr++ = *s++;
				else
					break;

			*variable_ptr = '\0';

			/* Get the contents */
			/* ---------------- */
			contents = get_value( v, variable );

			if ( !contents ) contents = "\"\"";

			/* Copy the variable contents to the buffer */
			/* ---------------------------------------- */
			while( *contents ) *d++ = *contents++;
		}
	}

	*d = '\0';
	return destination;

} /* replace_all_variables() */
