/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/create_application.c			*/
/* --------------------------------------------------------------------	*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "boolean.h"
#include "String.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "application.h"

typedef struct
{
	LIST *table_name_list;
	LIST *folder_name_list;
	LIST *missing_name_list;
	LIST *command_line_list;
} CREATE_APPLICATION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CREATE_APPLICATION *create_application_new(
		void );

/* Process */
/* ------- */
CREATE_APPLICATION *create_application_calloc(
		void );

/* Usage */
/* ----- */
LIST *create_application_table_name_list(
		void );

/* Usage */
/* ----- */
LIST *create_application_folder_name_list(
		void );

/* Usage */
/* ----- */
LIST *create_application_missing_name_list(
		LIST *create_application_table_name_list,
		LIST *create_application_folder_name_list );

/* Usage */
/* ----- */
LIST *create_application_command_line_list(
		LIST *create_application_missing_name_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *create_application_command_line(
		char *missing_name );

CREATE_APPLICATION *create_application_new( void )
{
	CREATE_APPLICATION *create_application;

	create_application = create_application_calloc();

	create_application->table_name_list =
		create_application_table_name_list();

	create_application->folder_name_list =
		create_application_folder_name_list();

	create_application->missing_name_list =
		create_application_missing_name_list(
			create_application->table_name_list,
			create_application->folder_name_list );

	create_application->command_line_list =
		create_application_command_line_list(
			create_application->missing_name_list );

	return create_application;
}

CREATE_APPLICATION *create_application_calloc( void )
{
	CREATE_APPLICATION *create_application;

	if ( ! ( create_application =
			calloc( 1,
				sizeof ( CREATE_APPLICATION ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return create_application;
}

LIST *create_application_table_name_list( void )
{
	char *system_string =
		"table_list '' | "			\
		"grep -v appaserver_application | "	\
		"grep -v appaserver_table | "		\
		"grep -v appaserver_column";

	return
	string_pipe_list( system_string );
}

LIST *create_application_folder_name_list( void )
{
	char *system_string =
		"folder_list.sh | "		\
		"grep -v application | "	\
		"grep -v table | "		\
		"grep -v column";

	return
	string_pipe_list( system_string );
}

LIST *create_application_missing_name_list(
		LIST *table_name_list,
		LIST *folder_name_list )
{
	return
	list_subtract(
		folder_name_list /* list */,
		table_name_list /* subtract_this */ );
}

LIST *create_application_command_line_list(
		LIST *missing_name_list )
{
	LIST *list = list_new();
	char *missing_name;

	if ( list_rewind( missing_name_list ) )
	do {
		missing_name = list_get( missing_name_list );

		list_set(
			list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			create_application_command_line(
				missing_name ) );

	} while ( list_next( missing_name_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

char *create_application_command_line( char *missing_name )
{
	char command_line[ 128 ];

	snprintf(
		command_line,
		sizeof ( command_line ),
		"create_table '' %s y",
		missing_name );

	return strdup( command_line );
}

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	boolean execute_boolean;
	CREATE_APPLICATION *create_application;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_stderr( argc, argv );

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s process execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	execute_boolean = ( *argv[ 2 ] == 'y' );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title_string */ );

	/* Safely returns */
	/* -------------- */
	create_application = create_application_new();

	if ( execute_boolean )
	{
		list_system_string_list_execute(
			create_application->command_line_list
				/* system_string_list */ );

		printf( "<h3>Process complete</h3>\n" );
	}
	else
	{
		list_html_display( create_application->command_line_list );
	}

	document_close();

	return 0;
}
