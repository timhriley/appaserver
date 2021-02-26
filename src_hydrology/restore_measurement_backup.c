/* ---------------------------------------------------	*/
/* src_hydrology/restore_measurement_backup.c		*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_backup.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"restore_measurement"
#define STATION_PIECE		0
#define DATATYPE_PIECE		1
#define DATE_PIECE		2
#define TIME_PIECE		3
#define VALUE_PIECE		4

#define MEASUREMENT_BACKUP_SELECT_LIST		"station,datatype,measurement_date,measurement_time,original_value"
#define UPDATE_MEASUREMENT_UPDATE_METHOD_SELECT_LIST	  						"station,datatype,measurement_date,measurement_time"
#define UPDATE_LIST				 "station,datatype,measurement_date,measurement_time"
#define DELETE_LIST				 "station,datatype,measurement_date,measurement_time"
#define INSERT_LIST				 "station,datatype,measurement_date,measurement_time,measurement_value"

/* Prototypes */
/* ---------- */
char *get_update_measurement_update_method_sys_string(
			char *application_name,
			char *station,
			char *datatype,
			char *measurement_update_date,
			char *measurement_update_time,
			char *measurement_update_method,
			char *login_name );

char *get_insert_measurement_sys_string(
			int *restore_count,
			char *application_name,
			char *station,
			char *datatype,
			char *measurement_update_date,
			char *measurement_update_time,
			char *measurement_update_method,
			char *login_name,
			int really_yn );

char *get_update_measurement_sys_string(
			int *restore_count,
			char *application_name,
			char *station,
			char *datatype,
			char *measurement_update_date,
			char *measurement_update_time,
			char *measurement_update_method,
			char *login_name,
			int really_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *station, *datatype;
	char *measurement_update_date;
	char *measurement_update_time;
	char *measurement_update_method;
	char *login_name;
	int really_yn;
	char *insert_sys_string = {0};
	char *update_sys_string = {0};
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int restore_count = 0;
	char *database_string = {0};
				
	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s ignored application station datatype measurement_update_date measurement_update_time measurement_update_method login_name really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	/* session = argv[ 1 ]; */
	application_name = argv[ 2 ];
	station = argv[ 3 ];
	datatype = argv[ 4 ];
	measurement_update_date = argv[ 5 ];
	measurement_update_time = argv[ 6 ];
	measurement_update_method = argv[ 7 ];
	login_name = argv[ 8 ];
	really_yn = *argv[ 9 ];

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

	appaserver_parameter_file = new_appaserver_parameter_file();

	insert_sys_string =
		get_insert_measurement_sys_string(	
			&restore_count,
			application_name,
			station,
			datatype,
			measurement_update_date,
			measurement_update_time,
			measurement_update_method,
			login_name,
			really_yn );

	if ( strcmp(	measurement_update_method,
			DELETE_MEASUREMENT_UPDATE_METHOD ) != 0 )
	{
		update_sys_string =
			get_update_measurement_sys_string(	
				&restore_count,
				application_name,
				station,
				datatype,
				measurement_update_date,
				measurement_update_time,
				measurement_update_method,
				login_name,
				really_yn );
	}

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

	system( insert_sys_string );

	if ( update_sys_string ) system( update_sys_string );

	if ( really_yn == 'y' )
	{
		char *sys_string;

		sys_string = get_update_measurement_update_method_sys_string(	
					application_name,
					station,
					datatype,
					measurement_update_date,
					measurement_update_time,
					measurement_update_method,
					login_name );

		system( sys_string );
	}

	if ( really_yn == 'y' )
	{
		printf( "<p>Restored %d measurements.\n", restore_count );

		process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( "<p>Did not restore %d measurements.\n", restore_count);
	}

	document_close();
	exit( 0 );
} /* main() */

char *get_insert_measurement_sys_string(
			int *restore_count,
			char *application_name,
			char *station,
			char *datatype,
			char *measurement_update_date,
			char *measurement_update_time,
			char *measurement_update_method,
			char *login_name,
			int really_yn )
{
	char sys_string[ 4096 ];
	char where_clause[ 1024 ];
	char *measurement_backup_table_name = {0};
	char *measurement_table_name;
	char insert_process[ 1024 ];
	char delete_process[ 1024 ];
	char *sql_process;
	char *results_string;

	measurement_backup_table_name = 
		get_table_name( application_name, "measurement_backup" );

	measurement_table_name = 
		get_table_name( application_name, "measurement" );

	sprintf( where_clause,
		 "    station = '%s'				"
		 "and datatype = '%s'				"
		 "and measurement_update_date = '%s'			"
		 "and measurement_update_time = '%s'			"
		 "and measurement_update_method = '%s'			"
		 "and login_name = '%s'				",
		 station,
		 datatype,
		 measurement_update_date,
		 measurement_update_time,
		 measurement_update_method,
		 login_name );

	/* Get the restore count */
	/* --------------------- */
	sprintf( sys_string, 
	"echo \"select count(*) from %s where %s;\"		|"
	"sql.e							 ",
		 measurement_backup_table_name,
		 where_clause );

	results_string = pipe2string( sys_string );
	if ( !results_string )
		*restore_count = 0;
	else
		*restore_count = atoi( results_string );

	if ( really_yn == 'y' )
		sql_process = "sql.e";
	else
		sql_process = "html_paragraph_wrapper";

	sprintf(	delete_process,
			"delete_statement.e %s %s '%c'",
			measurement_table_name,
			DELETE_LIST,
			FOLDER_DATA_DELIMITER );

	sprintf(	insert_process,
			"insert_statement.e %s %s '%c'",
			measurement_table_name,
			INSERT_LIST,
			FOLDER_DATA_DELIMITER );

	sprintf( sys_string,
	"get_folder_data	application=%s				 "
	"			folder=%s				 "
	"			select='%s'				 "
	"			where=\"%s\"				|"
	"%s								|"
	"%s								 ",
		 application_name,
		 measurement_backup_table_name,
		 MEASUREMENT_BACKUP_SELECT_LIST,
		 where_clause,
		 delete_process,
		 sql_process );

	sprintf( sys_string + strlen( sys_string ),
	";get_folder_data	application=%s				 "
	"			folder=%s				 "
	"			select='%s'				 "
	"			where=\"%s\"				|"
	"%s								|"
	"%s								 ",
		 application_name,
		 measurement_backup_table_name,
		 MEASUREMENT_BACKUP_SELECT_LIST,
		 where_clause,
		 insert_process,
		 sql_process );

	return strdup( sys_string );

} /* get_insert_measurement_sys_string() */

char *get_update_measurement_sys_string(
			int *restore_count,
			char *application_name,
			char *station,
			char *datatype,
			char *measurement_update_date,
			char *measurement_update_time,
			char *measurement_update_method,
			char *login_name,
			int really_yn )
{
	char sys_string[ 4096 ];
	char where_clause[ 1024 ];
	char *measurement_backup_table_name = {0};
	char *measurement_table_name;
	char update_process[ 1024 ];
	char *sql_process;
	char *results_string;

	measurement_backup_table_name = 
		get_table_name( application_name, "measurement_backup" );

	measurement_table_name = 
		get_table_name( application_name, "measurement" );

	sprintf( where_clause,
		 "    station = '%s'					"
		 "and datatype = '%s'					"
		 "and measurement_update_date = '%s'			"
		 "and measurement_update_time = '%s'			"
		 "and measurement_update_method = '%s'			"
		 "and login_name = '%s'					",
		 station,
		 datatype,
		 measurement_update_date,
		 measurement_update_time,
		 measurement_update_method,
		 login_name );

	/* Get the restore count */
	/* --------------------- */
	sprintf( sys_string, 
	"echo \"select count(*) from %s where %s;\"		|"
	"sql.e							 ",
		 measurement_backup_table_name,
		 where_clause );

	results_string = pipe2string( sys_string );
	if ( !results_string )
		*restore_count = 0;
	else
		*restore_count = atoi( results_string );

	if ( really_yn == 'y' )
		sql_process = "sql.e";
	else
		sql_process = "html_paragraph_wrapper";

	sprintf(update_process,
			"sed 's/,/|measurement_value=/%d'		|"
			"update_statement.e %s %s			|"
			"%s						 ",
			VALUE_PIECE,
			measurement_table_name,
			UPDATE_LIST,
			sql_process );

	sprintf( sys_string, 
	"get_folder_data	application=%s				  "
	"			folder=%s				  "
	"			select='%s'				  "
	"			where=\"%s\"				 |"
	"tr '%c' ','							 |"
	"%s								  ",
		 application_name,
		 measurement_backup_table_name,
		 MEASUREMENT_BACKUP_SELECT_LIST,
		 where_clause,
		 FOLDER_DATA_DELIMITER,
		 update_process );

	return strdup( sys_string );

} /* get_update_measurement_sys_string() */

char *get_update_measurement_update_method_sys_string(
			char *application_name,
			char *station,
			char *datatype,
			char *measurement_update_date,
			char *measurement_update_time,
			char *measurement_update_method,
			char *login_name )
{
	char sys_string[ 4096 ];
	char where_clause[ 1024 ];
	char *measurement_backup_table_name = {0};
	char *measurement_table_name;
	char update_process[ 1024 ];

	measurement_backup_table_name = 
		get_table_name( application_name, "measurement_backup" );

	measurement_table_name = 
		get_table_name( application_name, "measurement" );

	sprintf( where_clause,
		 "    station = '%s'					"
		 "and datatype = '%s'					"
		 "and measurement_update_date = '%s'			"
		 "and measurement_update_time = '%s'			"
		 "and measurement_update_method = '%s'			"
		 "and login_name = '%s'					",
		 station,
		 datatype,
		 measurement_update_date,
		 measurement_update_time,
		 measurement_update_method,
		 login_name );

	sprintf(update_process,
			"sed 's/$/|measurement_update_method=/'		|"
			"update_statement.e %s %s			|"
			"sql.e						 ",
			measurement_table_name,
			UPDATE_LIST );

	sprintf( sys_string,
	"get_folder_data	application=%s				  "
	"			folder=%s				  "
	"			select='%s'				  "
	"			where=\"%s\"				 |"
	"tr '%c' ','							 |"
	"%s 								  ",
		 application_name,
		 measurement_backup_table_name,
		 UPDATE_MEASUREMENT_UPDATE_METHOD_SELECT_LIST,
		 where_clause,
		 FOLDER_DATA_DELIMITER,
		 update_process );

	return strdup( sys_string );

} /* get_update_measurement_update_method_sys_string() */

