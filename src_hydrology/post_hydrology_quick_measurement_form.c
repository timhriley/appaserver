/* -----------------------------------------------------	*/
/* src_hydrology/post_hydrology_quick_measurement_form.c	*/
/* -----------------------------------------------------	*/
/*								*/
/* This script is attached to the submit button on 		*/
/* the hydrology quick measurement form.			*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "dictionary2file.h"
#include "folder.h"
#include "operation.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "insert_database.h"
#include "update_database.h"
#include "query.h"
#include "document.h"
#include "process.h"
#include "post2dictionary.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "measurement_update_parameter.h"
#include "measurement_backup.h"
#include "measurement_validation.h"
#include "session.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */
#define QUICK_MEASUREMENT_ESTIMATION_METHOD	"eye_ball"

/* Prototypes */
/* ---------- */
void get_measurement_record(
			char **station,
			char **datatype,
			char **measurement_date,
			char **measurement_time,
			char **measurement_value,
			UPDATE_FOLDER *update_folder );

void insert_measurement_backup(	FILE *insert_pipe,
				char *measurement_update_date,
				char *measurement_update_time,
				char *measurement_update_method,
				char *login_name,
				LIST *update_row_list );

int main( int argc, char **argv )
{
	char *person, *application_name, *session, *folder_name;
	char *insert_update_key = EDIT_FRAME;
	char sys_string[ 4096 ];
	DICTIONARY *post_dictionary;
	DICTIONARY *file_dictionary;
	UPDATE_DATABASE *update_database;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter;
	char *datatype;
	char *station;
	char *begin_date;
	char *end_date;
	MEASUREMENT_BACKUP *measurement_backup;
	pid_t dictionary_process_id;
	char *database_string = {0};
	char *results_string;
	LIST *additional_update_attribute_name_list;
	LIST *additional_update_data_list;
	LIST *attribute_list;
	DICTIONARY_APPASERVER *dictionary_appaserver;

	if ( argc != 11 )
	{
		fprintf( stderr, 
"Usage: %s person application session folder [4 ignored parameters] dictionary_process_id ignored_parameter\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	person = argv[ 1 ];
	application_name = argv[ 2 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	dictionary_process_id = atoi( argv[ 9 ] );

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

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				person );
	}

	if ( !session_access(	application_name,
				session,
				person ) )
	{
		session_access_failed_message_and_exit(
			application_name, session, person );
	}

	appaserver_parameter_file = new_appaserver_parameter_file();

	post_dictionary =
		post2dictionary(stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ );

	attribute_list =
		attribute_get_attribute_list(
			application_name,
			"measurement" /* folder_name */,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				post_dictionary,
				application_name,
				attribute_list,
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	dictionary_appaserver->row_dictionary =
		dictionary_appaserver_get_row_dictionary_multi_row(
			dictionary_appaserver->
				working_post_dictionary,
			attribute_get_attribute_name_list(
				attribute_list ),
			(LIST *)0 /* operation_name_list */ );

	/* Get the file dictionary */
	/* ----------------------- */
	file_dictionary =
		dictionary2file_get_dictionary(
			dictionary_process_id,
			appaserver_parameter_file->appaserver_data_directory,
			insert_update_key,
			folder_name,
			(char *)0 /* optional_related_attribute_name */,
			ELEMENT_DICTIONARY_DELIMITER );

	dictionary_appaserver_parse_multi_attribute_keys(
		file_dictionary,
		(char *)0 /* prefix */ );

	if ( !file_dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get file_dictionary\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( dictionary_get_index_data(
				&station,
				post_dictionary,
				"station",
				0 ) == -1 )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot find station in post_dictionary\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	dictionary_set_string(	file_dictionary,
				"station_0",
				station );

	if ( dictionary_get_index_data(
				&datatype,
				post_dictionary,
				"datatype",
				0 ) == -1 )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot find datatype in post_dictionary\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	dictionary_set_string(	file_dictionary,
				"datatype_0",
				datatype );

	if ( dictionary_get_index_data(
				&begin_date,
				post_dictionary,
				"begin_date",
				0 ) == -1 )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot find begin_date in post_dictionary\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( dictionary_get_index_data(
				&end_date,
				post_dictionary,
				"end_date",
				0 ) == -1 )
	{
		end_date = begin_date;
	}

	update_database =
		update_database_new(
			application_name,
			session,
			person,
			(char *)0 /* role_name */,
			folder_name,
			dictionary_appaserver->row_dictionary,
			file_dictionary );

	update_database->update_row_list =
		update_database_update_row_list(
			update_database->post_dictionary,
			update_database->file_dictionary,
			update_database->folder->folder_name,
			update_database->folder->attribute_list,
			(LIST *)0 /* one2m_recursive_relation_list */,
			(LIST *)0 /* mto1_isa_recursive_relation_list */,
			update_database->folder->post_change_process );

	additional_update_attribute_name_list = list_new();
	additional_update_data_list = list_new();

	list_append_pointer(
		additional_update_attribute_name_list,
			"measurement_update_method" );
	list_append_pointer(
		additional_update_data_list,
			QUICK_MEASUREMENT_ESTIMATION_METHOD );
	list_append_pointer(
		additional_update_attribute_name_list,
			"last_validation_date" );
	list_append_pointer(
		additional_update_data_list,
			"null" );
	list_append_pointer(
		additional_update_attribute_name_list,
			"last_person_validating" );
	list_append_pointer(
		additional_update_data_list,
			"null" );
	list_append_pointer(
		additional_update_attribute_name_list,
			"last_validation_process" );
	list_append_pointer(
		additional_update_data_list,
			MEASUREMENT_VALIDATION_UNVALIDATION_PROCESS );

	results_string =
		update_database_execute(
			update_database->application_name,
			update_database->session,
			update_database->update_row_list,
			update_database->post_dictionary,
			update_database->login_name,
			update_database->role_name,
			additional_update_attribute_name_list,
			additional_update_data_list,
			1 /* abort_if_first_update_failed */ );

	if ( results_string && *results_string )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->appaserver_mount_point );
		printf( "<p>Update failed: %s\n", results_string );
		document_close();
		exit( 0 );
	}

	measurement_update_parameter =
		measurement_update_parameter_new(
			update_database->application_name,
			station,
			datatype,
			QUICK_MEASUREMENT_ESTIMATION_METHOD,
			person,
			(DICTIONARY *)0 /* parameter_dictionary */,
			(char *)0 /* notes */ );

	measurement_backup = 
		measurement_backup_new(
				application_name, 
				QUICK_MEASUREMENT_ESTIMATION_METHOD,
				person,
				measurement_update_parameter->now_date,
				measurement_update_parameter->now_time );

	measurement_backup->insert_pipe =
		measurement_backup_open_insert_pipe(
					application_name );

	insert_measurement_backup(
			measurement_backup->insert_pipe,
			measurement_backup->measurement_update_date,
			measurement_backup->measurement_update_time,
			measurement_backup->measurement_update_method,
			measurement_backup->login_name,
			update_database->update_row_list );

	measurement_backup_close( measurement_backup->insert_pipe );

	measurement_update_parameter_save( measurement_update_parameter );

	/* Output the hydrology quick measurement form (again) */
	/* --------------------------------------------------- */
	post_dictionary = dictionary_new_dictionary();
	dictionary_set_string(	post_dictionary,
				"station",
				station );

	dictionary_set_string(	post_dictionary,
				"datatype",
				datatype );

	dictionary_set_string(	post_dictionary,
				"begin_date",
				begin_date );

	dictionary_set_string(	post_dictionary,
				"end_date",
				end_date );

	sprintf( sys_string, 
"hydrology_quick_measurement_form %s %s %s \"%s\" 2>>%s",
	 	 person,
		 application_name,
		 session,
		 dictionary_display_delimiter( post_dictionary, '&' ),
		 appaserver_error_get_filename(
			application_name ) );

	system( sys_string );

	exit( 0 );

} /* main() */

void insert_measurement_backup(	FILE *insert_pipe,
				char *measurement_update_date,
				char *measurement_update_time,
				char *measurement_update_method,
				char *login_name,
				LIST *update_row_list )
{
	char comma_delimited_record[ 1024 ];
	UPDATE_ROW *update_row;
	UPDATE_FOLDER *update_folder;
	char *station;
	char *datatype;
	char *measurement_date;
	char *measurement_time;
	char *measurement_value;

	if ( !list_rewind( update_row_list ) )
		return;

	do {
		update_row = list_get_pointer( update_row_list );

		if ( !update_row->update_folder_list
		||   !list_rewind( update_row->update_folder_list ) )
		{
			continue;
		}

		do {

			update_folder =
				list_get_pointer(
					update_row->update_folder_list );
	
			get_measurement_record(	&station,
						&datatype,
						&measurement_date,
						&measurement_time,
						&measurement_value,
						update_folder );
	
			sprintf(comma_delimited_record,
				"%s,%s,%s,%s,%s",
				station,
				datatype,
				measurement_date,
				measurement_time,
				measurement_value );
	
			measurement_backup_insert(
				insert_pipe,
				measurement_update_date,
				measurement_update_time,
				measurement_update_method,
				login_name,
				comma_delimited_record,
				'y' /* really_yn */,
				',' );

		} while( list_next( update_row->update_folder_list ) );

	} while( list_next( update_row_list ) );
} /* insert_measurement_backup() */

void get_measurement_record(
			char **station,
			char **datatype,
			char **measurement_date,
			char **measurement_time,
			char **measurement_value,
			UPDATE_FOLDER *update_folder )
{
	CHANGED_ATTRIBUTE *changed_attribute;
	WHERE_ATTRIBUTE *where_attribute;
	LIST *where_attribute_list;
	LIST *changed_attribute_list;

	where_attribute_list =
		update_folder->
			where_attribute_list;

	changed_attribute_list =
		update_folder->
			changed_attribute_list;

	if ( !list_reset( where_attribute_list ) ) return;

	do {
		where_attribute =
			list_get_pointer(
				where_attribute_list );

		if ( strcmp( where_attribute->attribute_name,
			     "station" ) == 0 )
		{
			*station = where_attribute->data;
		}
		else
		if ( strcmp( where_attribute->attribute_name,
			     "datatype" ) == 0 )
		{
			*datatype = where_attribute->data;
		}
		else
		if ( strcmp( where_attribute->attribute_name,
			     "measurement_date" ) == 0 )
		{
			*measurement_date = where_attribute->data;
		}
		else
		if ( strcmp( where_attribute->attribute_name,
			     "measurement_time" ) == 0 )
		{
			*measurement_time = where_attribute->data;
		}

	} while( list_next( where_attribute_list ) );

	if ( !list_reset( changed_attribute_list ) ) return;

	do {
		changed_attribute =
			list_get_pointer(
				changed_attribute_list );

		*measurement_value = changed_attribute->old_data;

	} while( list_next( changed_attribute_list ) );
} /* get_measurement_record() */

