/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_vegetation/upload_photo.c	*/
/* ----------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "basename.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "date_convert.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define DESTINATION_FILESPEC	"%s/appaserver/%s/photos/%s/%s"
/*				 ^	       ^         ^  ^		*/
/*				 |	       |	 |  |		*/
/*				 |	       |	 | filename	*/
/*			  document_root   application  subdirectory	*/
/* -------------------------------------------------------------------- */

#define DESTINATION_DIRSPEC	"%s/appaserver/%s/photos/%s"
#define RELATIVE_FILESPEC	"photos/%s/%s"
#define ATTRIBUTE_NAME		"filename"

/* Prototypes */
/* ---------- */
boolean get_utms(		char *UTMX,
				char *UTMY,
				char *application_name,
				int cell_id );

boolean cell_exists(		char *application_name,
				int cell_id );

char *get_subdirectory(		char *date_taken_string );

boolean insert_photo_table(	char *application_name,
				int cell_id,
				char *subdirectory,
				char *filename );

boolean upload_photo(		char *application_name,
				char *document_root_directory,
				int cell_id,
				char *date_taken_string,
				char *source_filename_directory_session );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				int cell_id );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int cell_id;
	char *date_taken_string;
	char title[ 128 ];
	char sub_title[ 128 ];
	FILE *input_file;
	char *source_filename_directory_session;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name date_taken cell_id filename\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	date_taken_string = argv[ 3 ];
	cell_id = atoi( argv[ 4 ] );
	source_filename_directory_session = argv[ 5 ];

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

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new( "", application_name );
	document_set_output_content_type( document );
	
	document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
	document_output_body(
				document->application_name,
				document->onload_control_string );

	get_title_and_sub_title(
			title,
			sub_title,
			process_name,
			cell_id );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !source_filename_directory_session
	||   !( input_file = fopen( source_filename_directory_session, "r" ) ) )
	{
		printf( "<h3>ERROR: please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}
	fclose( input_file );

	if ( upload_photo(	application_name,
				appaserver_parameter_file->
					document_root,
				cell_id,
				date_taken_string,
				source_filename_directory_session ) )
	{
		printf( "<h3>Process complete.</h3>\n" );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				int cell_id )
{
	format_initial_capital( title, process_name );

	sprintf(	sub_title,
			"Cell ID = %d",
			cell_id );

} /* get_title_and_sub_title() */

boolean upload_photo(		char *application_name,
				char *document_root_directory,
				int cell_id,
				char *date_taken_string,
				char *source_filename_directory_session )
{
	char *full_filename_directory;
	char *filename;
	char tmp_buffer[ 256 ];
	char sys_string[ 1024 ];
	char destination[ 256 ];
	char *subdirectory;

	if ( !cell_exists( application_name, cell_id ) )
	{
		printf( "<h3>Error: the cell does not exist.</h3>\n" );
		return 0;
	}

	if ( !*source_filename_directory_session
	||   strcmp( source_filename_directory_session, "filename" ) == 0 )
	{
		printf( "<h3>Error: please transmit a file.</h3>\n" );
		return 0;
	}

	strcpy( tmp_buffer, source_filename_directory_session );

	full_filename_directory =
		right_trim_session(
			tmp_buffer );

	filename = basename_get_filename( full_filename_directory );

	if ( ! ( subdirectory = get_subdirectory( date_taken_string ) ) )
	{
		printf( "<h3>Error: invalid date format.</h3>\n" );
		return 0;
	}

	/* Make sure the destination directory exists */
	/* ------------------------------------------ */
	sprintf( destination,
		 DESTINATION_DIRSPEC,
		 document_root_directory,
		 application_name,
		 subdirectory );

	if ( !timlib_file_exists( destination ) )
	{
		sprintf( sys_string,
			 "mkdir %s",
			 destination );

		system( sys_string );

		sprintf( sys_string,
		 	"chmod g+w %s",
		 	destination );

		system( sys_string );
	}

	/* Copy over the photo */
	/* ------------------- */
	sprintf( destination,
		 DESTINATION_FILESPEC,
		 document_root_directory,
		 application_name,
		 subdirectory,
		 filename );
		 
	sprintf( sys_string,
		 "cp %s %s",
		 source_filename_directory_session,
		 destination );

	system( sys_string );

	sprintf( sys_string,
		 "chmod g+w %s",
		 destination );

	system( sys_string );

	if ( !insert_photo_table(	application_name,
					cell_id,
					subdirectory,
					filename ) )
	{
		return 0;
	}
	else
	{
		return 1;
	}

} /* upload_photo() */

boolean insert_photo_table(	char *application_name,
				int cell_id,
				char *subdirectory,
				char *filename )
{
	char sys_string[ 1024 ];
	char UTMX[ 128 ];
	char UTMY[ 128 ];
	char *table_name;
	char *field_list;
	char filespec[ 128 ];

	table_name = get_table_name( application_name, "photo" );
	field_list = "UTMX,UTMY,filename,cell_id";

	if ( !get_utms( UTMX, UTMY, application_name, cell_id ) )
	{
		printf( "<h3>ERROR in %s/%s()/%d: cannot get UTMs.</h3>\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return 0;
	}

	sprintf( filespec,
		 RELATIVE_FILESPEC,
		 subdirectory,
		 filename );

	sprintf( sys_string,
		 "echo \"%s^%s^%s^%d\"			|"
		 "insert_statement.e	table_name=%s	 "
		 "			field=%s	 "
		 "			del='^'		|"
		 "sql.e					 ",
		 UTMX,
		 UTMY,
		 filespec,
		 cell_id,
		 table_name,
		 field_list );

	system( sys_string );

	return 1;

} /* insert_photo_table() */

char *get_subdirectory( char *date_taken_string )
{
	char subdirectory[ 32 ];
	char international_format[ 16 ];
	char trimmed[ 16 ];

	if ( !date_convert_source_unknown(
				international_format /* return_string */,
				international /* destination_format */,
				date_taken_string ) )
	{
		return (char *)0;
	}

	trim_character(
		trimmed /* destination */,
		'-',
		international_format /* source */ );

	sprintf( subdirectory,
		 "%s_TP",
		 trimmed );

	return strdup( subdirectory );

} /* get_subdirectory() */

boolean cell_exists(		char *application_name,
				int cell_id )
{
	char sys_string[ 1024 ];
	char *select;
	char where[ 128 ];

	select = "count(*)";

	sprintf( where,
		 "cell_id = %d",
		 cell_id );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=cell_50x50	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	return atoi( pipe2string( sys_string ) );

} /* cell_exists() */

boolean get_utms(	char *UTMX,
			char *UTMY,
			char *application_name,
			int cell_id )
{
	char sys_string[ 1024 ];
	char *select;
	char where[ 128 ];
	char *results;

	select = "UTMX,UTMY";

	sprintf( where,
		 "cell_id = %d",
		 cell_id );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=cell_50x50	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece( UTMX, FOLDER_DATA_DELIMITER, results, 0 );
	piece( UTMY, FOLDER_DATA_DELIMITER, results, 1 );

	return 1;

} /* get_utms() */
