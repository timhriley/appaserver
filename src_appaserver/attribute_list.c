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
#include "attribute.h"

/* Prototypes */
/* ---------- */
void attribute_list_attribute_name(
			char *folder_name );

void attribute_list_folder_attribute(
			void );

void attribute_name_list_display(
			LIST *attribute_name_list,
			boolean with_sort );

void attribute_list_with_null(
			char *folder_name );

void attribute_list_folder(
			char *folder_name );

void attribute_list_without_folder(
			void );

/* For <Insert> <Relation>: only attributes for the inserted folder. */
/* ----------------------------------------------------------------- */
void attribute_list_dictionary(
			char *dictionary_string );


int main( int argc, char **argv )
{
	char *application_name;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	/* -------------------------------------------- */
	/* Usage: attribute_list [folder | with_table]  */
	/*    or: attribute_list ignored dictionary	*/
	/* -------------------------------------------- */

	if ( argc == 3 )
	{
		attribute_list_dictionary( argv[ 2 ] );
	}
	else
	if ( argc == 2 )
	{
		if ( strcmp( argv[ 1 ], "with_table" ) == 0 )
		{
			attribute_list_folder_attribute();
		}
		else
		{
			attribute_list_attribute_name( argv[ 1 ] );
		}
	}
	else
	{
		attribute_list_without_folder();
	}

	return 0;
}

void attribute_name_list_display(
			LIST *attribute_name_list,
			boolean with_sort )
{
	FILE *output_pipe;

	if ( with_sort )
	{
		output_pipe = popen( "sort -u", "w" );
	}
	else
	{
		output_pipe = popen( "cat", "w" );
	}

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
	ATTRIBUTE *attribute = attribute_calloc();

	attribute->attribute_list =
		attribute_list( folder_name );

	attribute_name_list_display(
		attribute_name_list(
			attribute->attribute_list ),
		0 /* not with_sort */ );
}

void attribute_list_folder_attribute( void )
{
	ATTRIBUTE *attribute = attribute_calloc();

	attribute->attribute_list =
		attribute_list(
			(char *)0 /* folder_name */  );

	attribute_name_list_display(
		attribute_folder_attribute_name_list(
			attribute->attribute_list ),
		1 /* with_sort */ );
}

void attribute_list_without_folder( void )
{
	ATTRIBUTE *attribute = attribute_calloc();

	attribute->attribute_list =
		attribute_list( (char *)0 /* folder_name */ );

	attribute_name_list_display(
		attribute_name_list(
			attribute->attribute_list ),
		1 /* with_sort */ );
}

/* For <Insert> <Relation>: only attributes for the inserted folder. */
/* ----------------------------------------------------------------- */
void attribute_list_dictionary( char *dictionary_string )
{
	DICTIONARY *dictionary;
	char *results;

	dictionary =
		dictionary_string_dictionary(
			dictionary_string );

	if ( ( results =
			dictionary_fetch(
				dictionary,
				"one2m_folder_name_for_process" ) ) )
	{
		if ( strcmp( results, "relation" ) == 0 )
		{
			if ( ( results =
					dictionary_fetch(
						dictionary,
						"folder" ) ) )
			{
				attribute_list_with_null( results );
				return;
			}
		}
	}
	attribute_list_folder( (char *)0 /* folder_name */ );
}

void attribute_list_with_null( char *folder_name )
{
	ATTRIBUTE *attribute = attribute_calloc();

	attribute->attribute_list =
		attribute_list( folder_name );

	list_set(
		attribute->attribute_list,
		attribute_new( "null" ) );

	attribute_name_list_display(
		attribute_name_list(
			attribute->attribute_list ),
		1 /* with_sort */ );
}

void attribute_list_attribute_name( char *folder_name )
{
	ATTRIBUTE *attribute = attribute_calloc();

	attribute->attribute_list =
		attribute_list( folder_name );

	attribute_name_list_display(
		attribute_name_list(
			attribute->attribute_list ),
		0 /* not with_sort */ );
}

