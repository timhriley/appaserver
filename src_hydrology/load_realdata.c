/* --------------------------------------------------- 	*/
/* load_realdata.c			    	   	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "appaserver_parameter_file.h"
#include "timlib.h"
#include "basename.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "application.h"
#include "process.h"
#include "session.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"load_realdata"

/* Prototypes */
/* ---------- */
char *get_station_from_file( 	char *application_name,
				char *file_with_path );

/* char *datatype2extension( char *datatype ); */

void load_realdata(
		char *source_directory,
		char *email_address,
		char really_yn,
		char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *source_directory;
	char *email_address;
	char really_yn;
	DOCUMENT *document;
	pid_t pid;
	char execution_directory[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *session;
	char *login_name;
	char *role_name;
	char *database_string = {0};

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s application session login_name role source_directory email_address really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	source_directory = argv[ 5 ];
	email_address = argv[ 6 ];
	really_yn = *argv[ 7 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );
	add_python_library_path();

	appaserver_parameter_file = new_appaserver_parameter_file();

	/* cd to the execution directory */
	/* ----------------------------- */
	sprintf(execution_directory,
		"%s/%s",
		appaserver_parameter_file->appaserver_mount_point,
		application_relative_source_directory( application_name));

	chdir( execution_directory );

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	if ( *email_address && strcmp( email_address, "email_address" ) != 0 )
	{
		/* Send to background */
		/* ------------------ */
		if ( !( pid = fork() ) )
		{
			load_realdata(	source_directory,
					email_address,
					really_yn,
					application_name );
			exit( 0 );
		}
	}
	else
	{
		load_realdata(	source_directory,
				email_address,
				really_yn,
				application_name );
	}

	if ( *email_address && strcmp( email_address, "email_address" ) != 0 )
	{
		if ( really_yn == 'y' )
		{
			printf( 
		"<p>In progress. Check email for error messages.\n" );
			process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
		}
		else
		{
			printf( 
		"<p>Process NOT executed. Check email for output.\n" );
		}
	}
	else
	{
		if ( really_yn == 'y' )
		{
			printf( "<p>Process complete.\n" );
			process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
		}
		else
		{
			printf( "<p>Process NOT executed.\n" );
		}
	}

	document_close();
	exit( 0 );
} /* main() */

void load_realdata(
		char *source_directory,
		char *email_address,
		char really_yn,
		char *application_name )
{
	char sys_string[ 4096 ];
	char *station;
	LIST *datatype_list;
	LIST *file_with_path_list;
	char *file_with_path;
	char *datatype;
	char mail_process[ 512 ];
	char archive_process[ 512 ];

	datatype_list = list_new();
	list_append_string( datatype_list, "stage" );
	list_append_string( datatype_list, "flow" );
	list_append_string( datatype_list, "rain" );
	list_append_string( datatype_list, "head_water" );
	list_append_string( datatype_list, "tail_water" );

	list_rewind( datatype_list );
	do {
		datatype = list_get_string( datatype_list );

		sprintf( sys_string,
			 "ls -1 %s/%s/*.dat 2>/dev/null",
			 source_directory,
			 datatype );

		file_with_path_list = pipe2list( sys_string );
		if ( list_rewind( file_with_path_list ) )
		{
			do {
				file_with_path = 
					list_get_string( file_with_path_list );

				printf( "<p>%s = ", file_with_path );

				station =
					get_station_from_file( 
							application_name,
							file_with_path );

				if ( !station )
				{
					printf( "Cannot get station!\n" );
					continue;
				}

				printf( "%s\n", station );

				if ( really_yn == 'y' )
				{
					sprintf( archive_process,
					 	"mv %s %s/%s/loaded",
					 	file_with_path,
					 	source_directory,
						datatype );
				}
				else
				{
					strcpy( archive_process,
						"echo > /dev/null" );
				}

				if ( *email_address
				&&   strcmp( email_address, 
					     "email_address" ) != 0 )
				{
					if ( really_yn == 'y' )
					{
						sprintf( mail_process, 
"mailx -s \"%s: realdata upload errors\" %s",
		 			 	basename_get_base_name(
							file_with_path,0),
					 	email_address );
					}
					else
					{
						sprintf( mail_process, 
"mailx -s \"%s: realdata upload output\" %s",
		 			 	basename_get_base_name(
							file_with_path,0),
					 	email_address );
					}
				}
				else
				{
					strcpy( mail_process, 
						"html_paragraph_wrapper" );
				}

				sprintf( sys_string,
		"realdata_load_station.sh %s %s %s %s %c 2>&1		|"
		"%s						 	;"
		"%s						 	 ",
						 application_name,
						 file_with_path,
						 station,
						 datatype,
						 really_yn,
						 mail_process,
						 archive_process );

				system( sys_string );

			} while( list_next( file_with_path_list ) );
		} /* if list_rewind( file_with_path_list ) */
	} while( list_next( datatype_list ) );
} /* load_realdata() */

char *get_station_from_file( 	char *application_name,
				char *file_with_path )
{
	char sys_string[ 1024 ];
	char *station;

	sprintf( sys_string, 
		 "head -1 %s						|"
		 "sed 's/YYYY MM DD/DATE/g'				|"
		 "toupper.e						|"
		 "column.e 1						|"
		 "station_alias_to_station %s 0 '|' 2>/dev/null		 ",
		 file_with_path,
		 application_name );

	station = pipe2string( sys_string );
	if ( !station || !*station  )
		return (char *)0;
	else
		return up_string( station );
}

