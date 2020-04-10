/* ----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_prompt_process_form.c	*/
/* ----------------------------------------------------------	*/
/*								*/
/* This script is attached to the submit button on 		*/
/* the prompt process form.					*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "application.h"
#include "appaserver.h"
#include "list.h"
#include "list_usage.h"
#include "dictionary.h"
#include "document.h"
#include "query.h"
#include "process.h"
#include "process_parameter_list.h"
#include "appaserver_parameter_file.h"
#include "appaserver_user.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "post2dictionary.h"
#include "role.h"
#include "session.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */
#define WITH_CHECK_EXECUTABLE_OK 	1

/* Prototypes */
/* ---------- */
void set_folder_name_to_null(	LIST *attribute_list );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session;
	char *process_or_process_set_name;
	char *role_name;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	DICTIONARY *original_post_dictionary = {0};
	DICTIONARY *parameter_dictionary;
	PROCESS *process;
	PROCESS_SET *process_set;
	PROCESS_PARAMETER_LIST *process_parameter_list;
	LIST *prompt_list;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	ROLE *role;
	char passed_preprompt_yn;
	char *database_string = {0};

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name application session process role passed_preprompt_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	session = argv[ 3 ];
	process_or_process_set_name = argv[ 4 ];
	role_name = argv[ 5 ];
	passed_preprompt_yn = *argv[ 6 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
		environ_set_environment(
			"DATABASE",
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
		environ_set_environment(
			"DATABASE",
			application_name );
	}

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_appaserver_home_to_environment();
	add_local_bin_to_path();
	add_relative_source_directory_to_path( application_name );

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_access_process(
				application_name,
				session,
				process_or_process_set_name,
				login_name,
				role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}

	if ( !appaserver_user_exists_role(
					application_name,
					login_name,
					role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	role = role_new_role(	application_name,
				role_name );

	original_post_dictionary =
		post2dictionary(
			stdin,
			appaserver_parameter_file->
				appaserver_data_directory,
			session );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				original_post_dictionary,
				application_name,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	appaserver_library_dictionary_convert_dates(
		dictionary_appaserver->non_prefixed_dictionary,
		application_name );

	dictionary_appaserver_parse_multi_attribute_keys(
		dictionary_appaserver->non_prefixed_dictionary,
		QUERY_RELATION_OPERATOR_STARTING_LABEL );

	process_parameter_list = 
		process_parameter_list_new(
			login_name,
			application_name,
			process_or_process_set_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			1 /* with_preprompt */,
			(DICTIONARY *)0,
			role_name,
			session,
			0 /* not with_populate_drop_downs */ );

	if ( process_parameter_list
	&&   list_length( process_parameter_list->process_parameter_list )
	&&   passed_preprompt_yn != 'y' )
	{
		char sys_string[ 65536 ];

		/* State two: had preprompt and filled it out. */
		/* ------------------------------------------- */
		dictionary_appaserver->
			preprompt_dictionary =
				dictionary_copy_dictionary(
					dictionary_appaserver->
						non_prefixed_dictionary );

		sprintf( sys_string, 
		"output_prompt_process_form %s %s %s \"%s\" %s y \"%s\" 2>>%s",
		 	 login_name,
			 application_name,
			 session,
			 process_or_process_set_name,
			 role_name,
			 dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				1 /* not with_non_prefixed_dictionary */ ),
			 appaserver_error_get_filename( application_name ) );

		exit( system( sys_string ) );
	}

	process_parameter_list = 
		process_parameter_list_new(
				login_name,
				application_name,
				process_or_process_set_name,
				role_get_override_row_restrictions(
					role->
					override_row_restrictions_yn ),
				0 /* not with_preprompt */,
				(DICTIONARY *)0,
				role_name,
				session,
				0 /* not with_populate_drop_downs */ );

	process_parameter_list_dictionary_set_drop_down_prompt_optional_display(
			dictionary_appaserver->non_prefixed_dictionary,
			process_parameter_list->process_parameter_list );

	process = process_new_process(
			application_name,
			session,
			process_or_process_set_name,
			dictionary_appaserver->
				non_prefixed_dictionary,
			role_name,
			WITH_CHECK_EXECUTABLE_OK );
	
	if ( !process )
	{
		document = document_new(
				"",
				application_name );

		document->output_content_type = 1;

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->
				appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );

		process_set =
			process_new_process_set(
				application_name,
				BOGUS_SESSION,
				process_or_process_set_name );

		if ( !process_set_load(
				&process_set->notepad,
				&process_set->html_help_file_anchor,
				&process_set->post_change_javascript,
				&process_set->prompt_display_text,
				&process_set->preprompt_help_text,
				&process_set->prompt_display_bottom,
				application_name,
				session,
				process_or_process_set_name ) )
		{
			printf( "<h3>Error occurred. Check log.</h3>\n" );
			document_close();
			exit( 0 );
		}

		if ( process_set->prompt_display_text
		&&   *process_set->prompt_display_text )
		{
			char buffer[ 256 ];

			if ( *process_set->prompt_display_text == 'a'
			||   *process_set->prompt_display_text == 'e'
			||   *process_set->prompt_display_text == 'i'
			||   *process_set->prompt_display_text == 'o'
			||   *process_set->prompt_display_text == 'u' )
			{
				printf (
				"<h2>Error: an %s must be selected.</h2>\n",
				format_initial_capital(
					buffer,
					process_set->prompt_display_text ) );
			}
			else
			{
				printf (
				"<h2>Error: a %s must be selected.</h2>\n",
				format_initial_capital(
					buffer,
					process_set->prompt_display_text ) );
				}
		}
		else
		{
			printf (
			"<h2>Error: a process must be selected.</h2>\n");
		}
		document_close();
		exit( 1 );
	}

	/* The process might be a process set */
	/* ---------------------------------- */
	process_set = process_new_process_set(
				application_name,
				BOGUS_SESSION,
				process_or_process_set_name );

	process_for_folder_or_attribute_parameters_populate_attribute_list(
			process->attribute_list,
			application_name,
			process_parameter_list->process_parameter_list );

	prompt_list =
		process_get_prompt_list(
			process_parameter_list->process_parameter_list );

	set_folder_name_to_null( process->attribute_list );

	if ( !process->executable
	||   !*process->executable )
	{
		document = document_new(
				"",
				application_name );

		document->output_content_type = 1;

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->
				appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );

		printf (
	"<h2>Error: This process doesn't have the command line set.</h2>\n");
		document_close();
		exit( 1 );
	}

	/* Start with the query_dictionary */
	/* ------------------------------- */
	parameter_dictionary =
		dictionary_copy(
			dictionary_appaserver->
				non_prefixed_dictionary );

	/* Append the preprompt_dictionary without colliding keys. */
	/* ------------------------------------------------------- */
	dictionary_append_dictionary(
		parameter_dictionary,
		dictionary_appaserver->
			preprompt_dictionary );

	process_convert_parameters(
		&process->executable,
		application_name,
		session,
		(char *)0 /* state */,
		login_name,
		(char *)0 /* folder */,
		role_name,
		(char *)0 /* target_frame */,
		parameter_dictionary,
		parameter_dictionary /* where_clause_dictionary */,
		process->attribute_list,
		prompt_list,
		(LIST *)0 /* primary_attribute_name_list */,
		(LIST *)0 /* primary_data_list */,
		0 /* row */,
		process->process_name,
		process_set,
		(char *)0
		/* one2m_folder_name_for_processes */,
		(char *)0 /* operation_row_count_string */,
		(char *)0 /* prompt */ );

	fflush( stdout );
	fflush( stderr );

	if ( !list_length( process_parameter_list->process_parameter_list )
	&&   appaserver_get_frameset_menu_horizontal(
					application_name,
					login_name ) )
	{
		char sys_string[ 1024 ];

		sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '%s' %s 2>>%s",
			application_name,
			session,
			login_name,
			role_name,
			"" /* title */,
			"y" /* content_type_yn */,
			appaserver_error_get_filename( application_name ) );
		if ( system( sys_string ) ){};
	}

/*
appaserver_output_error_message(
	application_name, process->executable, login_name );
*/

	if ( system( process->executable ) ){};

	exit( 0 );

} /* main() */

/* ----------------------------------------------------- */
/* When building a where clause for the $where variable, */
/* the folder name is assumed unknown.			 */
/* ----------------------------------------------------- */
void set_folder_name_to_null( LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( !list_rewind( attribute_list ) ) return;

	do {
		attribute =
			(ATTRIBUTE *)
				list_get_pointer( attribute_list );
		attribute->folder_name = (char *)0;
	} while( list_next( attribute_list ) );
} /* set_folder_name_to_null() */

