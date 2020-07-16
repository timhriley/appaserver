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

char *string_enforce_utf16(	char *destination,
				char *source )
{
	LIST *negative_occurrance_list;
	STRING_OCCURRANCE *occurrance;
	char *skip_to;

	strcpy( destination, source );

	negative_occurrance_list =
		string_negative_sequence_occurrance_list(
			destination );

	if ( !list_go_tail( negative_occurrance_list ) )
		return destination;

	do {
		occurrance = list_get( negative_occurrance_list );

		if ( occurrance->occurrance > 2 )
		{
			skip_to = occurrance->ptr + occurrance->occurrance;

			strcpy( occurrance->ptr, skip_to );
		}

	} while ( list_previous( negative_occurrance_list ) );

	return destination;
}

LIST *string_negative_sequence_occurrance_list( char *source )
{
	char *ptr;
	boolean inside_negative_sequence;
	LIST *occurrance_list;
	STRING_OCCURRANCE *string_occurrance = {0};

	if ( !*source ) return (LIST *)0;

	occurrance_list = list_new();

	inside_negative_sequence = 0;

fprintf( stderr, "%s/%s()/%d: source = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
source );
fflush( stderr );

	ptr = source;

	while ( *ptr++ )
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

{
char buffer[ 65536 ];
fprintf( stderr, "%s/%s()/%d: occurrance_list = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
string_occurrance_list_display( buffer, occurrance_list ) );
fflush( stderr );
}

	return occurrance_list;
}

