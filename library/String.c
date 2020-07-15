/* $APPASERVER_HOME/library/string.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "boolean.h"
#include "list.h"
#include "String.h"

STRING_OCCURRANCE *string_occurrance_new( char *ptr )
{
	STRING_OCCURRANCE *a;

	if ( ! ( a = (STRING_OCCURRANCE *)
			calloc( 1, sizeof ( STRING_OCCURRANCE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	a->ptr = ptr;
	return a;
}

char *string_enforce_utf16(	char *destination,
				char *source )
{
	LIST *negative_occurrance_list;

	negative_occurrance_list =
		string_negative_sequence_occurrance_list(
			source );

	string_occurrance_list_display(
		destination,
		negative_occurrance_list );

	return destination;
}

LIST *string_negative_sequence_occurrance_list(
				char *source )
{
	char *ptr;
	boolean inside_negative_sequence;
	LIST *occurrance_list;
	STRING_OCCURRANCE *string_occurrance = {0};

	if ( !*source ) return (LIST *)0;

	occurrance_list = list_new();

	inside_negative_sequence = 0;

	while ( (ptr = source++) )
	{
		if ( *ptr < 0 )
		{
			if ( inside_negative_sequence )
			/* ------------ */
			/* Still inside */
			/* ------------ */
			{
				if ( !string_occurrance )
				{
					fprintf( stderr,
			"ERROR in %s/%s()/%d: empty string_occurrance.\n",
						 __FILE__,
						 __FUNCTION__,
						 __LINE__ );
					exit( 1 );
				}

				string_occurrance->occurrance++;
			}
			else
			/* ------------ */
			/* Newly inside */
			/* ------------ */
			{
				string_occurrance =
					string_occurrance_new(
						ptr );

				string_occurrance->occurrance = 1;

				list_set(	occurrance_list,
						string_occurrance );

			}
		}
		else
		/* Must be *ptr >= 0 */
		{
			if ( inside_negative_sequence )
			{
				/* ---------------- */
				/* No longer inside */
				/* ---------------- */
				inside_negative_sequence = 0;
			}
			else
			{
				/* Do nothing */
			}
		}
	}
	return occurrance_list;
}

char *string_occurrance_list_display(
				char *destination,
				LIST *occurrance_list )
{
	STRING_OCCURRANCE *string_occurrance;
	char *ptr = destination;

	*ptr = '\0';

	if ( list_rewind( occurrance_list ) )
	{
		do {
			string_occurrance = list_get( occurrance_list );

			if ( ptr == destination )
			{
				ptr += sprintf( ptr,
						"\nat: [%s] are %d\n",
						string_occurrance->ptr,
						string_occurrance->occurrance );
			}
			else
			{
				ptr += sprintf( ptr,
						"at: [%s] are %d\n",
						string_occurrance->ptr,
						string_occurrance->occurrance );
			}
		} while ( list_next( occurrance_list ) );
	}
	return destination;
}
