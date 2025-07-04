/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_system/attribute_list.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

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

void attribute_list_with_null_stdout(
		char *folder_name );

/* For <Insert> <Relation>: only attributes for the inserted folder. */
/* ----------------------------------------------------------------- */
void attribute_list_dictionary(
		char *dictionary_string );

int main( int argc, char **argv )
{
	char *application_name;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	/* -------------------------------------------- */
	/* Usage: attribute_list folder			*/
	/*    or: attribute_list ignored $dictionary	*/
	/*    or: attribute_list ignored		*/
	/*    or: attribute_list			*/
	/* -------------------------------------------- */

	if ( argc == 2
	&&   strcmp( argv[ 1 ], "ignored" ) != 0 )
	{
		attribute_folder_name_list_stdout(
			argv[ 1 ] /* folder_name */ );
		return 0;
	}

	if ( argc == 3
	&&   strcmp( argv[ 1 ], "ignored" ) == 0
	&&   *argv[ 2 ]
	&&   strcmp( argv[ 2 ], "$dictionary" ) != 0 )
	{
		attribute_list_dictionary( argv[ 2 ] );
		return 0;
	}

	attribute_name_list_stdout(
		ATTRIBUTE_TABLE,
		ATTRIBUTE_PRIMARY_KEY );

	return 0;
}

/* For <Insert> <Relation>: only attributes for the inserted folder. */
/* ----------------------------------------------------------------- */
void attribute_list_dictionary( char *dictionary_string )
{
	DICTIONARY *dictionary;
	char *results;

	dictionary =
		dictionary_string_new(
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER,
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
				attribute_list_with_null_stdout(
					results /* folder_name */ );

				return;
			}
		}
	}

	attribute_name_list_stdout(
		ATTRIBUTE_TABLE,
		ATTRIBUTE_PRIMARY_KEY );
}

void attribute_list_with_null_stdout( char *folder_name )
{
	LIST *attribute_list =
		folder_attribute_list(
			folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			0 /* not fetch_attribute */,
			0 /* not cache_boolean */ );

	list_set(
		attribute_list,
		folder_attribute_new(
			folder_name,
			"null" ) );

	attribute_name_list_sort_stdout(
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			attribute_list ) );
}
