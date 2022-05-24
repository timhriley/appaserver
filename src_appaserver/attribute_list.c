/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/attribute_list.c	*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "appaserver_error.h"
#include "environ.h"
#include "boolean.h"
#include "folder_attribute.h"

/* Prototypes */
/* ---------- */
void attribute_name_list_sort(
			LIST *attribute_name_list );

void attribute_list_with_null(
			char *folder_name );

void attribute_list_folder(
			char *folder_name );

/* For <Insert> <Relation>: only attributes for the inserted folder. */
/* ----------------------------------------------------------------- */
void attribute_list_dictionary(
			char *dictionary_string );

int main( int argc, char **argv )
{

	environ_exit_application_name( argv[ 0 ] );

/*
	char *application_name;
	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );
*/

	/* -------------------------------------------- */
	/* Usage: attribute_list folder			*/
	/*    or: attribute_list ignored $dictionary	*/
	/* -------------------------------------------- */

	if ( argc == 3 )
	{
		if ( strcmp( argv[ 2 ], "$dictionary" ) != 0 )
		{
			attribute_list_dictionary( argv[ 2 ] );
		}
	}
	else
	if ( argc == 2 )
	{
		attribute_list_folder(
			argv[ 1 ] /* folder_name */ );
	}

	return 0;
}

void attribute_name_list_sort(
			LIST *attribute_name_list )
{
	FILE *output_pipe;

	output_pipe = popen( "sort -u", "w" );

	if ( list_rewind( attribute_name_list ) )
	{
		do {
			fprintf(output_pipe,
				 "%s\n",
				(char *)list_get(
					attribute_name_list ) );

		} while ( list_next( attribute_name_list ) );
	}
	pclose( output_pipe );
}

void attribute_list_folder( char *folder_name )
{
	printf( "%s\n",
		list_display_line(
			folder_attribute_name_list(
				(char *)0 /* folder_name */,
				folder_attribute_list(
				   folder_name,
				   (LIST *)0 /* exclude_attribute... */,
				   0 /* not fetch_attribute */ ) ) ) );
}

/* For <Insert> <Relation>: only attributes for the inserted folder. */
/* ----------------------------------------------------------------- */
void attribute_list_dictionary( char *dictionary_string )
{
	DICTIONARY *dictionary;
	char *results;

	dictionary =
		dictionary_string_new(
			dictionary_string );

	if ( ( results =
			dictionary_fetch(
				"one2m_folder_name_for_process",
				dictionary ) ) )
	{
		if ( strcmp( results, "relation" ) == 0 )
		{
			if ( ( results =
					dictionary_fetch(
						"folder",
						dictionary ) ) )
			{
				attribute_list_with_null(
					results /* folder_name */ );
			}
		}
	}
}

void attribute_list_with_null( char *folder_name )
{
	LIST *attribute_list =
		folder_attribute_list(
			folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			0 /* not fetch_attribute */ );

	list_set(
		attribute_list,
		folder_attribute_new(
			folder_name,
			"null" ) );

	attribute_name_list_sort(
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			attribute_list ) );
}
