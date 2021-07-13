/* -------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/upgrade_appaserver_databaase.c      	*/
/* -------------------------------------------------------------- 	*/
/* 						       			*/
/* Run this program after upgrading to a new version			*/
/* of Appaserver.							*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "environ.h"
#include "list.h"
#include "application.h"
#include "basename.h"
#include "appaserver_library.h"
#include "dictionary.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */
#define MINIMUM_VERSION		"6.30"

/* Prototypes */
/* ---------- */
DICTIONARY *get_upgrade_scripts_dictionary( char *application_name );

void do_upgrade(	char *application_name,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file );

char *get_next_version( LIST *version_list,
			char *original_this_version );

LIST *get_version_list( char *appaserver_mount_point );

int main( int argc, char **argv )
{
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	/* Exits upon error. */
	/* ----------------- */
	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( argc == 1 )
	{
		LIST *application_name_list;
		char *application_name;

		application_name_list =
			appaserver_library_application_name_list(
				appaserver_parameter_file->
					appaserver_error_directory );

		if ( list_rewind( application_name_list ) )
		{
			do {
				application_name =
					list_get_pointer(
						application_name_list );

				do_upgrade(	application_name,
						appaserver_parameter_file );

			} while( list_next( application_name_list ) );
		}
		else
		{
			do_upgrade(	environ_get_application_name(
						argv[ 0 ] ),
					appaserver_parameter_file );
		}
	}
	else
	{
		while( --argc )
		{
			do_upgrade(	*++argv,
					appaserver_parameter_file );
		}
	}

	return 0;

}

void do_upgrade(	char *application_name,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file )

{
	char *database_string = {0};
	DICTIONARY *upgrade_scripts_dictionary;
	char *latest_version = "0.0";
	char *this_version;
	FILE *input_pipe;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char upgrade_script_with_directory[ 512 ];
	char *upgrade_script;
	char dictionary_key[ 256 ];
	char *upgrade_scripts_table_name;
	LIST *version_list;

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	application_reset();
	upgrade_scripts_table_name =
		get_table_name( application_name, "upgrade_scripts" );

	upgrade_scripts_dictionary =
		get_upgrade_scripts_dictionary( application_name );

	version_list =
		get_version_list(
			appaserver_parameter_file->appaserver_mount_point );

	this_version = application_version( application_name );

	if ( strcmp( this_version, "0.0" ) == 0 ) return;

	do {
		if ( strcmp( this_version, MINIMUM_VERSION ) <= 0 ) continue;

		sprintf(sys_string,
			"ls -1 %s/upgrade/%s/*.sh 2>/dev/null",
			appaserver_parameter_file->
				appaserver_mount_point,
			this_version );
		input_pipe = popen( sys_string, "r" );

		sprintf(sys_string,
			"insert_statement.e table=%s field=%s delimiter='|' |"
			"sql.e",
			upgrade_scripts_table_name,
			"appaserver_version,upgrade_script" );
		output_pipe = popen( sys_string, "w" );

		while( get_line( upgrade_script_with_directory, input_pipe ) )
		{
			upgrade_script =
				basename_get_filename(
					upgrade_script_with_directory );

			sprintf(dictionary_key,
				"%s%c%s",
				this_version,
				FOLDER_DATA_DELIMITER,
				upgrade_script );

			if ( dictionary_exists_key(
					upgrade_scripts_dictionary,
					dictionary_key ) )
			{
				continue;
			}

			sprintf(sys_string,
				"%s/upgrade/%s/%s %s",
				appaserver_parameter_file->
					appaserver_mount_point,
				this_version,
				upgrade_script,
				application_name );

			printf( "%s\n", sys_string );
			if ( system( sys_string ) ){};

			fprintf(output_pipe,
				"%s|%s\n",
				this_version,
				upgrade_script );
		}
		pclose( input_pipe );
		pclose( output_pipe );
		latest_version = this_version;

	} while( ( this_version = get_next_version(
					version_list,
					this_version ) ) );

	sprintf(sys_string,
		"echo \"update %s_application		 "
		"	set appaserver_version = '%s'	 "
		"	where application = '%s';\"	|"
		"sql.e",
		application_name,
		latest_version,
		application_name );

	if ( system( sys_string ) ){};

}

DICTIONARY *get_upgrade_scripts_dictionary( char *application_name )
{
	char sys_string[ 1024 ];
	char *select = "appaserver_version,upgrade_script";

	sprintf(sys_string,
		"get_folder_data	application=%s		"
		"			folder=upgrade_scripts	"
		"			select=%s		",
		application_name,
		select );

	return pipe2dictionary( sys_string, '\0' /* delimiter */ );

}

char *get_next_version(	LIST *version_list,
			char *original_this_version )
{
	char *this_version;
	
	if ( !list_rewind( version_list ) ) return (char *)0;

	do {
		this_version = list_get_pointer( version_list );

		if ( strcmp( this_version, original_this_version ) > 0 )
		{
			return this_version;
		}
	} while( list_next( version_list ) );

	return (char *)0;

}

LIST *get_version_list( char *appaserver_mount_point )
{
	LIST *version_list;
	char *version;
	char sys_string[ 1024 ];
	char buffer[ 128 ];
	FILE *input_pipe;

	sprintf(sys_string,
		"ls -1 %s/upgrade 2>/dev/null		|"
		"grep '[0-9]$'				 ",
		appaserver_mount_point );

	input_pipe = popen( sys_string, "r" );
	version_list = list_new();

	while( get_line( buffer, input_pipe ) )
	{
		version = basename_get_filename( buffer );
		list_append_pointer(
			version_list,
			strdup( version ) );
	}
	pclose( input_pipe );
	return version_list;
}

