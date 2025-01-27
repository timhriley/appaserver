/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/appaserver_upgrade.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "environ.h"
#include "application.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "appaserver_upgrade.h"

LIST *appaserver_upgrade_application_list(
		LIST *application_name_list,
		LIST *appaserver_upgrade_version_list )
{
	LIST *list;
	char *application_name;
	APPASERVER_UPGRADE_APPLICATION *appaserver_upgrade_application;

	if ( !list_rewind( application_name_list )
	||   !list_length( appaserver_upgrade_version_list ) )
	{
		return (LIST *)0;
	}

	list = list_new();

	do {
		application_name =
			list_get(
				application_name_list );

		appaserver_upgrade_application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			appaserver_upgrade_application_new(
				application_name,
				appaserver_upgrade_version_list );

		list_set(
			list,
			appaserver_upgrade_application );

	} while ( list_next( application_name_list ) );

	return list;
}

APPASERVER_UPGRADE_APPLICATION *appaserver_upgrade_application_new(
		char *application_name,
		LIST *appaserver_upgrade_version_list )
{
	APPASERVER_UPGRADE_APPLICATION *appaserver_upgrade_application;

	if ( !application_name
	||   !list_length( appaserver_upgrade_version_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	appaserver_upgrade_application =
		appaserver_upgrade_application_calloc();

	appaserver_upgrade_application->application_name = application_name;

	appaserver_upgrade_application->appaserver_upgrade_table_list =
		appaserver_upgrade_table_list(
			application_name,
			appaserver_upgrade_version_list );

	return appaserver_upgrade_application;
}

APPASERVER_UPGRADE_APPLICATION *appaserver_upgrade_application_calloc( void )
{
	APPASERVER_UPGRADE_APPLICATION *appaserver_upgrade_application;

	appaserver_upgrade_application =
		calloc( 1,
			sizeof ( APPASERVER_UPGRADE_APPLICATION ) );

	if ( !appaserver_upgrade_application )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	return appaserver_upgrade_application;
}

char *appaserver_upgrade_application_system_string(
		char *appaserver_upgrade_root,
		char *appaserver_version,
		char *upgrade_script )
{
	char system_string[ 1024 ];

	if ( !appaserver_upgrade_root
	||   !appaserver_version
	||   !upgrade_script )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	sprintf(system_string,
		"%s/%s/%s",
		appaserver_upgrade_root,
		appaserver_version,
		upgrade_script );

	return strdup( system_string );
}

LIST *appaserver_upgrade_file_list(
		char *appaserver_upgrade_root,
		char *appaserver_version )
{
	LIST *list;
	char *root;
	char *system_string;
	FILE *input_pipe;
	char upgrade_script[ 128 ];
	APPASERVER_UPGRADE_FILE *appaserver_upgrade_file;

	if ( !appaserver_upgrade_root
	||   !appaserver_version )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	list = list_new();

	root =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_upgrade_file_root(
			appaserver_upgrade_root,
			appaserver_version );

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_upgrade_file_system_string(
			root );

	input_pipe =
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( upgrade_script, input_pipe, 128 ) )
	{
		appaserver_upgrade_file =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			appaserver_upgrade_file_new(
				appaserver_version,
				root,
				strdup( upgrade_script ) );

		list_set(
			list,
			appaserver_upgrade_file );
	}

	pclose( input_pipe );
	return list;
}

char *appaserver_upgrade_file_root(
		char *appaserver_upgrade_root,
		char *appaserver_version )
{
	static char root[ 128 ];

	if ( !appaserver_upgrade_root
	||   !appaserver_version )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	sprintf(root,
		"%s/%s",
		appaserver_upgrade_root,
		appaserver_version );

	return root;
}

char *appaserver_upgrade_file_system_string(
		char *appaserver_upgrade_file_root )
{
	static char system_string[ 256 ];

	if ( !appaserver_upgrade_file_root )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_upgrade_file_root is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	sprintf(system_string,
		"cd %s && ls -1",
		appaserver_upgrade_file_root );

	return system_string;
}

APPASERVER_UPGRADE_FILE *appaserver_upgrade_file_new(
		char *appaserver_version,
		char *appaserver_upgrade_file_root,
		char *upgrade_script )
{
	APPASERVER_UPGRADE_FILE *appaserver_upgrade_file;

	if ( !appaserver_version
	||   !upgrade_script )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	appaserver_upgrade_file = appaserver_upgrade_file_calloc();
	appaserver_upgrade_file->upgrade_script = upgrade_script;

	appaserver_upgrade_file->execute_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_upgrade_file_execute_system_string(
			appaserver_upgrade_file_root,
			upgrade_script );

	appaserver_upgrade_file->insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_upgrade_file_insert_statement(
			APPASERVER_UPGRADE_SCRIPT_TABLE,
			APPASERVER_UPGRADE_SCRIPT_INSERT,
			appaserver_version,
			upgrade_script );

	return appaserver_upgrade_file;
}

char *appaserver_upgrade_file_execute_system_string(
		char *appaserver_upgrade_file_root,
		char *upgrade_script )
{
	char system_string[ 256 ];

	if ( !appaserver_upgrade_file_root
	||   !upgrade_script )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	sprintf(system_string,
		"%s/%s",
		appaserver_upgrade_file_root,
		upgrade_script );

	return strdup( system_string );
}

APPASERVER_UPGRADE_FILE *appaserver_upgrade_file_calloc( void )
{
	APPASERVER_UPGRADE_FILE *appaserver_upgrade_file;

	if ( ! ( appaserver_upgrade_file =
			calloc( 1,
				sizeof ( APPASERVER_UPGRADE_FILE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	return appaserver_upgrade_file;
}

LIST *appaserver_upgrade_script_list( char *appaserver_version )
{
	LIST *list;
	char *where;
	FILE *input_pipe;
	char upgrade_script[ 128 ];
	APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script;

	if ( !appaserver_version )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	list = list_new();

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_upgrade_script_where(
			appaserver_version );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				"upgrade_script" /* select */,
				APPASERVER_UPGRADE_SCRIPT_TABLE,
				where ) );

	while ( string_input( upgrade_script, input_pipe, 128 ) )
	{
		appaserver_upgrade_script =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			appaserver_upgrade_script_new(
				appaserver_version,
				strdup( upgrade_script ) );

		list_set(
			list,
			appaserver_upgrade_script );
	}

	pclose( input_pipe );

	return list;
}

char *appaserver_upgrade_script_where( char *appaserver_version )
{
	static char where[ 128 ];

	if ( !appaserver_version )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_version is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	sprintf(where,
		"appaserver_version = '%s'",
		appaserver_version );

	return where;
}

APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script_new(
		char *appaserver_version,
		char *upgrade_script )
{
	APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script;

	if ( !appaserver_version
	||   !upgrade_script )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	appaserver_upgrade_script = appaserver_upgrade_script_calloc();
	appaserver_upgrade_script->upgrade_script = upgrade_script;

	return appaserver_upgrade_script;
}

APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script_calloc( void )
{
	APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script;

	if ( ! ( appaserver_upgrade_script =
			calloc( 1,
				sizeof ( APPASERVER_UPGRADE_SCRIPT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	return appaserver_upgrade_script;
}

boolean appaserver_upgrade_script_exists(
		char *application_name,
		char *appaserver_version,
		char *upgrade_script,
		LIST *appaserver_upgrade_application_list )
{
	APPASERVER_UPGRADE_APPLICATION *appaserver_upgrade_application;
	APPASERVER_UPGRADE_TABLE *appaserver_upgrade_table;
	APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script;

	if ( !application_name
	||   !appaserver_version
	||   !upgrade_script )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	if ( !list_rewind( appaserver_upgrade_application_list ) ) return 0;

	do {
		appaserver_upgrade_application =
			list_get(
				appaserver_upgrade_application_list );

		if ( strcmp(
			appaserver_upgrade_application->application_name,
			application_name ) != 0 )
		{
			continue;
		}

		if ( !list_rewind(
			appaserver_upgrade_application->
				appaserver_upgrade_table_list ) )
		{
			continue;
		}

		do {
			appaserver_upgrade_table =
				list_get(
					appaserver_upgrade_application->
						appaserver_upgrade_table_list );

			if ( strcmp(
				appaserver_upgrade_table->
					appaserver_version,
				appaserver_version ) != 0 )
			{
				continue;
			}

			appaserver_upgrade_script =
				appaserver_upgrade_script_seek(
					upgrade_script,
					appaserver_upgrade_table->
					     appaserver_upgrade_script_list );

			if ( appaserver_upgrade_script )
				return 1;
			else
				return 0;

		} while ( list_next( 
				appaserver_upgrade_application->
					appaserver_upgrade_table_list ) );

	} while ( list_next( appaserver_upgrade_application_list ) );

	return 0;
}

APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script_seek(
		char *upgrade_script,
		LIST *appaserver_upgrade_script_list )
{
	APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script;

	if ( !upgrade_script )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	if ( !list_rewind( appaserver_upgrade_script_list ) ) return NULL;

	do {
		appaserver_upgrade_script =
			list_get(
				appaserver_upgrade_script_list );

		if (  strcmp(
			appaserver_upgrade_script->upgrade_script,
			upgrade_script ) == 0 )
		{
			return appaserver_upgrade_script;
		}

	} while ( list_next( appaserver_upgrade_script_list ) );

	return NULL;
}

char *appaserver_upgrade_file_insert_statement(
		char *appaserver_upgrade_script_table,
		char *appaserver_upgrade_script_insert,
		char *appaserver_version,
		char *upgrade_script )
{
	char insert_statement[ 256 ];

	if ( !appaserver_upgrade_script_table
	||   !appaserver_version
	||   !upgrade_script )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	sprintf(insert_statement,
		"insert into %s (%s) values ('%s','%s');",
		appaserver_upgrade_script_table,
		appaserver_upgrade_script_insert,
		appaserver_version,
		upgrade_script );

	return strdup( insert_statement );
}

APPASERVER_UPGRADE_DIRECTORY *appaserver_upgrade_directory_fetch(
		char *appaserver_upgrade_root,
		char *appaserver_version )
{
	APPASERVER_UPGRADE_DIRECTORY *appaserver_upgrade_directory;

	if ( !appaserver_upgrade_root
	||   !appaserver_version )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	appaserver_upgrade_directory = appaserver_upgrade_directory_calloc();
	appaserver_upgrade_directory->appaserver_version = appaserver_version;

	appaserver_upgrade_directory->appaserver_upgrade_file_list =
		appaserver_upgrade_file_list(
			appaserver_upgrade_root,
			appaserver_version );

	if ( !list_length( 
		appaserver_upgrade_directory->
			appaserver_upgrade_file_list ) )
	{
		free( appaserver_upgrade_directory );
		return NULL;
	}

	return appaserver_upgrade_directory;
}

APPASERVER_UPGRADE_DIRECTORY *appaserver_upgrade_directory_calloc( void )
{
	APPASERVER_UPGRADE_DIRECTORY *appaserver_upgrade_directory;

	if ( ! ( appaserver_upgrade_directory =
			calloc( 1,
				sizeof ( APPASERVER_UPGRADE_DIRECTORY ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	return appaserver_upgrade_directory;
}

char *appaserver_upgrade_directory_system_string(
		char *appaserver_upgrade_root )
{
	static char system_string[ 256 ];

	if ( !appaserver_upgrade_root )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_upgrade_root is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	sprintf(system_string,
		"cd %s && ls -1d * | sort_delimited_numbers.e '.' n",
		appaserver_upgrade_root );

	return system_string;
}

APPASERVER_UPGRADE *appaserver_upgrade_fetch(
		char *application_log_extension,
		int argc,
		char **argv,
		char *application_name,
		boolean step_boolean,
		char *log_directory,
		char *mount_point )
{
	APPASERVER_UPGRADE *appaserver_upgrade;

	if ( !application_log_extension
	||   !argc
	||   !argv
	||   !log_directory
	||   !mount_point )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	appaserver_upgrade = appaserver_upgrade_calloc();

	if ( !application_name && !step_boolean )
	{
		appaserver_upgrade->application_list_argv =
			appaserver_upgrade_application_list_argv(
				argc,
				argv );
	}

	appaserver_upgrade->application_name_list =
		appaserver_upgrade_application_name_list(
			application_log_extension,
			application_name,
			log_directory,
			appaserver_upgrade->application_list_argv );

	if ( !list_length( appaserver_upgrade->application_name_list ) )
	{
		return appaserver_upgrade;
	}

	appaserver_upgrade->root =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_upgrade_root(
			mount_point );

	appaserver_upgrade->appaserver_upgrade_directory_list =
		appaserver_upgrade_directory_list(
			appaserver_upgrade->root );

	if ( list_length(
		appaserver_upgrade->appaserver_upgrade_directory_list ) )
	{
		appaserver_upgrade->version_list =
			appaserver_upgrade_version_list(
				appaserver_upgrade->
					appaserver_upgrade_directory_list );

		appaserver_upgrade->appaserver_upgrade_application_list =
			appaserver_upgrade_application_list(
				appaserver_upgrade->application_name_list,
				appaserver_upgrade->version_list );

		appaserver_upgrade->appaserver_upgrade_execute_list =
			appaserver_upgrade_execute_list(
				step_boolean,
				appaserver_upgrade->application_name_list,
				appaserver_upgrade->
					appaserver_upgrade_directory_list,
				appaserver_upgrade->
					appaserver_upgrade_application_list );
	}

	return appaserver_upgrade;
}

APPASERVER_UPGRADE *appaserver_upgrade_calloc( void )
{
	APPASERVER_UPGRADE *appaserver_upgrade;

	if ( ! ( appaserver_upgrade =
			calloc( 1,
			sizeof ( APPASERVER_UPGRADE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	return appaserver_upgrade;
}

char *appaserver_upgrade_root( char *mount_point )
{
	static char root[ 128 ];

	if ( !mount_point )
	{
		char message[ 128 ];

		sprintf(message, "mount_point is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	sprintf(root,
		"%s/upgrade",
		mount_point );

	return root;
}

LIST *appaserver_upgrade_application_name_list(
		char *application_log_extension,
		char *application_name,
		char *log_directory,
		LIST *list_argv )
{
	if ( application_name )
	{
		LIST *application_name_list = list_new();
		list_set( application_name_list, application_name );
		return application_name_list;
	}
	else
	if ( list_length( list_argv ) )
	{
		return list_argv;
	}
	else
	{
		return
		application_name_list(
			application_log_extension,
			log_directory );
	}

}

LIST *appaserver_upgrade_execute_list(
		boolean step_boolean,
		LIST *application_name_list,
		LIST *appaserver_upgrade_directory_list,
		LIST *appaserver_upgrade_application_list )
{
	LIST *list;
	char *application_name;
	APPASERVER_UPGRADE_EXECUTE *appaserver_upgrade_execute;

	if ( !list_rewind( application_name_list ) ) return NULL;

	list = list_new();

	do {
		application_name =
			list_get(
				application_name_list );

		appaserver_upgrade_execute =
			appaserver_upgrade_execute_new(
				step_boolean,
				application_name,
				appaserver_upgrade_directory_list,
				appaserver_upgrade_application_list );

		if ( appaserver_upgrade_execute )
		{
			list_set(
				list,
				appaserver_upgrade_execute );
		}

	} while ( list_next( application_name_list ) );

	return list;
}

APPASERVER_UPGRADE_EXECUTE *appaserver_upgrade_execute_new(
		boolean step_boolean,
		char *application_name,
		LIST *appaserver_upgrade_directory_list,
		LIST *appaserver_upgrade_application_list )
{
	APPASERVER_UPGRADE_EXECUTE *appaserver_upgrade_execute;
	APPASERVER_UPGRADE_DIRECTORY *appaserver_upgrade_directory;
	APPASERVER_UPGRADE_FILE *appaserver_upgrade_file;
	boolean all_done = 0;
	boolean script_exists;

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	if ( !list_rewind( appaserver_upgrade_directory_list ) ) return NULL;

	appaserver_upgrade_execute = appaserver_upgrade_execute_calloc();

	appaserver_upgrade_execute->application_name = application_name;
	appaserver_upgrade_execute->system_string_list = list_new();
	appaserver_upgrade_execute->insert_statement_list = list_new();

	do {
		appaserver_upgrade_directory =
			list_get(
				appaserver_upgrade_directory_list );

		if ( !list_rewind(
			appaserver_upgrade_directory->
				appaserver_upgrade_file_list ) )
		{
			continue;
		}

		do {
			appaserver_upgrade_file =
				list_get(
					appaserver_upgrade_directory->
						appaserver_upgrade_file_list );

			script_exists =
				appaserver_upgrade_script_exists(
					application_name,
					appaserver_upgrade_directory->
						appaserver_version,
					appaserver_upgrade_file->upgrade_script,
					appaserver_upgrade_application_list );

			if ( script_exists ) continue;

			list_set(
				appaserver_upgrade_execute->
					system_string_list,
				appaserver_upgrade_file->
					execute_system_string );

			list_set(
				appaserver_upgrade_execute->
					insert_statement_list,
				appaserver_upgrade_file->
					insert_statement );

			if ( step_boolean )
			{
				all_done = 1;
				break;
			}

		} while ( list_next( appaserver_upgrade_directory->
					appaserver_upgrade_file_list ) );

		if ( all_done ) break;

	} while ( list_next( appaserver_upgrade_directory_list ) );

	if ( !list_length( appaserver_upgrade_execute->system_string_list ) )
	{
		list_free( appaserver_upgrade_execute->system_string_list );
		list_free( appaserver_upgrade_execute->insert_statement_list );
		free( appaserver_upgrade_execute );

		return NULL;
	}

	return appaserver_upgrade_execute;
}

APPASERVER_UPGRADE_EXECUTE *appaserver_upgrade_execute_calloc( void )
{
	APPASERVER_UPGRADE_EXECUTE *appaserver_upgrade_execute;

	if ( ! ( appaserver_upgrade_execute =
			calloc( 1,
				sizeof ( APPASERVER_UPGRADE_EXECUTE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	return appaserver_upgrade_execute;
}

void appaserver_upgrade_execute_list_system(
		char *application_name,
		LIST *system_string_list,
		LIST *insert_statement_list )
{
	char *system_string;
	char *insert_statement;
	FILE *output_pipe;

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "application_name is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	if ( !list_rewind( system_string_list ) ) return;

	if (	list_length( system_string_list ) !=
		list_length( insert_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"length()=%d != length()=%d.\n",
			list_length( system_string_list ),
			list_length( insert_statement_list ) );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	do {
		system_string = list_get( system_string_list );

		printf(	"%s: %s\n",
			application_name,
			system_string );
		fflush( stdout );

		if ( system( system_string ) ){}
		fflush( stdout );

	} while ( list_next( system_string_list ) );

	output_pipe =
		appaserver_output_pipe(
			"sql.e" );

	list_rewind( insert_statement_list );

	do {
		insert_statement = list_get( insert_statement_list );

		fprintf(output_pipe,
			"%s\n",
			insert_statement );

	} while ( list_next( insert_statement_list ) );

	pclose( output_pipe );
}

LIST *appaserver_upgrade_table_list(
		char *application_name,
		LIST *appaserver_upgrade_version_list )
{
	LIST *list;
	char *appaserver_version;
	APPASERVER_UPGRADE_TABLE *appaserver_upgrade_table;

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "application_name is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	if ( !list_rewind( appaserver_upgrade_version_list ) ) return NULL;

	list = list_new();

	environment_database_set( application_name );

	do {
		appaserver_version =
			list_get(
				appaserver_upgrade_version_list );

		appaserver_upgrade_table =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			appaserver_upgrade_table_new(
				appaserver_version );

		list_set(
			list,
			appaserver_upgrade_table );

	} while ( list_next( appaserver_upgrade_version_list ) );

	return list;
}

APPASERVER_UPGRADE_TABLE *appaserver_upgrade_table_new(
		char *appaserver_version )
{
	APPASERVER_UPGRADE_TABLE *appaserver_upgrade_table;

	if ( !appaserver_version )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_version is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	appaserver_upgrade_table = appaserver_upgrade_table_calloc();
	appaserver_upgrade_table->appaserver_version = appaserver_version;

	appaserver_upgrade_table->appaserver_upgrade_script_list =
		appaserver_upgrade_script_list(
			appaserver_version );

	return appaserver_upgrade_table;
}

APPASERVER_UPGRADE_TABLE *appaserver_upgrade_table_calloc( void )
{
	APPASERVER_UPGRADE_TABLE *appaserver_upgrade_table;

	if ( ! ( appaserver_upgrade_table =
			calloc( 1,
				sizeof ( APPASERVER_UPGRADE_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	return appaserver_upgrade_table;
}

LIST *appaserver_upgrade_directory_list( char *appaserver_upgrade_root )
{
	LIST *list;
	char *system_string;
	char appaserver_version[ 128 ];
	FILE *input_pipe;
	APPASERVER_UPGRADE_DIRECTORY *appaserver_upgrade_directory;

	if ( !appaserver_upgrade_root )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty.\n" );

		fprintf(stderr,
			"ERROR: %s/%s()/%d: %s",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	list = list_new();

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_upgrade_directory_system_string(
			appaserver_upgrade_root );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( appaserver_version, input_pipe, 128 ) )
	{
		appaserver_upgrade_directory =
			appaserver_upgrade_directory_fetch(
				appaserver_upgrade_root,
				strdup( appaserver_version ) );

		if ( appaserver_upgrade_directory )
		{
			list_set(
				list,
				appaserver_upgrade_directory );
		}
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
		return NULL;
	else
		return list;
}

LIST *appaserver_upgrade_version_list(
		LIST *appaserver_upgrade_directory_list )
{
	LIST *appaserver_version_list;
	APPASERVER_UPGRADE_DIRECTORY *appaserver_upgrade_directory;

	if ( !list_rewind( appaserver_upgrade_directory_list ) ) return NULL;

	appaserver_version_list = list_new();

	do {
		appaserver_upgrade_directory =
			list_get(
				appaserver_upgrade_directory_list );

		if ( !appaserver_upgrade_directory->appaserver_version )
		{
			char message[ 128 ];

			sprintf(message,
	"appaserver_upgrade_directory->appaserver_version is empty.\n" );

			fprintf(stderr,
				"ERROR: %s/%s()/%d: %s",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );

			exit( 1 );
		}

		list_set(
			appaserver_version_list,
			appaserver_upgrade_directory->
				appaserver_version );

	} while ( list_next( appaserver_upgrade_directory_list ) );

	return appaserver_version_list;
}

LIST *appaserver_upgrade_application_list_argv(
		int argc,
		char **argv )
{
	return
	list_argv( argc, argv );
}

