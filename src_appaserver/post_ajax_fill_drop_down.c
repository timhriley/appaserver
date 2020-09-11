/* $APPASERVER_HOME/src_appaserver/post_ajax_fill_drop_down.c	*/
/* ------------------------------------------------------------	*/
/* This process is triggered on the <submit> button of a	*/
/* forked ajax window.						*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "form.h"
#include "folder.h"
#include "session.h"
#include "related_folder.h"
#include "appaserver.h"
#include "operation_list.h"
#include "application.h"
#include "appaserver_user.h"
#include "query.h"
#include "query_attribute_statistics_list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"

/* Constants */
/* --------- */
#define CHECK_SESSION		1
#define DATA_DELIMITER		'&'

/* Prototypes */
/* ---------- */
char *format_results(		char *results );

void output_process_results(
				char *application_name,
				char *folder_name,
				PROCESS *populate_drop_down_process,
				LIST *attribute_list,
				char *one2m_folder_name,
				char *value );

void output_folder_results(
			char *application_name,
			LIST *foreign_attribute_name_list,
			char *value,
			LIST *primary_attribute_name_list,
			char *folder_name );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session;
	char *role_name;
	char *mto1_folder_name;
	char *one2m_folder_name;
	char *value;
	FOLDER *mto1_folder;
	RELATED_FOLDER *related_folder;
	char *database_string = {0};

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s application login_name role session mto1_folder one2m_folder value\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

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

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	session = argv[ 4 ];
	mto1_folder_name = argv[ 5 ];
	one2m_folder_name = argv[ 6 ];
	value = argv[ 7 ];

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );

	if ( CHECK_SESSION
	&&   session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( CHECK_SESSION
	&&   strcmp(	login_name,
			session_get_login_name(
					application_name,
					session ) ) != 0 )
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

	mto1_folder =
		folder_with_load_new(
				application_name,
				session,
				mto1_folder_name,
				role_new( application_name, role_name ) );

	form_output_content_type();

	if ( ! ( related_folder =
			related_folder_mto1_seek(
				mto1_folder->mto1_related_folder_list,
				one2m_folder_name ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot seek one2m_folder_name = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 one2m_folder_name );
		exit( 1 );
	}

	if ( mto1_folder->populate_drop_down_process )
	{
		output_process_results(
			application_name,
		 	mto1_folder->folder_name,
			mto1_folder->populate_drop_down_process,
			mto1_folder->attribute_list,
			one2m_folder_name,
			value );
	}
	else
	{
		output_folder_results(
			application_name,
			related_folder->foreign_attribute_name_list,
			value,
		 	mto1_folder->primary_attribute_name_list,
		 	mto1_folder->folder_name );
	}

	return 0;

} /* main() */

void output_folder_results(
			char *application_name,
			LIST *foreign_attribute_name_list,
			char *value,
			LIST *primary_attribute_name_list,
			char *folder_name )
{
	char *where;
	char *results;
	char sys_string[ 1024 ];
	char *formatted_results;

	where = query_get_simple_where_clause(
			(FOLDER *)0,
			foreign_attribute_name_list
				/* where_attribute_name_list */,
			list_string2list(
				value,
				'^' )
				/* where_attribute_data_list */,
			(char *)0 /* login_name */ );

	sprintf( sys_string,
		 "get_folder_data	application=%s	 "
		 "			select=%s	 "
		 "			folder=%s	 "
		 "			where=\"%s\"	 "
		 "			order=select	|"
		 "joinlines.e '%c'			 ",
		 application_name,
		 list_display( primary_attribute_name_list ),
		 folder_name,
		 where,
		 DATA_DELIMITER );

	results = pipe2string( sys_string );

	if ( results && *results )
	{
		formatted_results = format_results( results );

		printf( "%s|%s\n",
			results,
			formatted_results );
	}
	else
	{
		printf( "\n" );
	}

} /* output_folder_results() */

void output_process_results(
				char *application_name,
				char *folder_name,
				PROCESS *populate_drop_down_process,
				LIST *attribute_list,
				char *one2m_folder_name,
				char *value )
{
	LIST *results_list;
	char *results;
	char *formatted_results;

	DICTIONARY *parameter_dictionary;
	char piece_buffer[ 512 ];

	if ( !populate_drop_down_process )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty populate_drop_down_process.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	parameter_dictionary = dictionary_small_new();

	dictionary_set_pointer(
		parameter_dictionary,
		one2m_folder_name,
		value );

	results_list =
		    folder_get_drop_down_process_list(
			application_name,
			BOGUS_SESSION,
			folder_name,
			(char *)0 /* login_name */,
			populate_drop_down_process,
			(char *)0 /* role_name */,
			parameter_dictionary,
			parameter_dictionary /* where_clause_dictionary */,
			(char *)0 /* state */,
			(char *)0 /* one2m_folder_name_for_processes */,
			attribute_list,
			0 /* piece_multi_attribute_data_label_delimiter */,
			populate_drop_down_process->process_name,
			(char *)0 /* prompt */ );

	if ( !list_length( results_list ) )
	{
		printf( "\n" );
		return;
	}

/*
	list_append_pointer( results_list, SELECT_OPERATOR );
*/

	list_rewind( results_list );

	do {
		results = list_get_pointer( results_list );

		if ( character_exists( results, '|' ) )
		{
			piece( piece_buffer, '|', results, 0 );
			printf( "%s", piece_buffer );
		}
		else
		{
			printf( "%s", results );
		}

		if ( !list_at_tail( results_list ) )
			printf( "%c", DATA_DELIMITER );

	} while( list_next( results_list ) );

	printf( "|" );

	list_rewind( results_list );

	do {
		results = list_get_pointer( results_list );

		if ( character_exists( results, '|' ) )
		{
			piece( piece_buffer, '|', results, 1 );

			formatted_results = format_results( piece_buffer );
		}
		else
		{
			formatted_results = format_results( results );
		}

		printf( "%s", formatted_results );

		if ( !list_at_tail( results_list ) )
			printf( "%c", DATA_DELIMITER );

	} while( list_next( results_list ) );

	printf( "\n" );

} /* output_process_results() */

char *format_results( char *results )
{
	static char formatted_results[ 1024 ];
	char search_string[ 2 ];

	*search_string = '^';
	*(search_string + 1) = '\0';

	format_initial_capital( formatted_results, results );

	search_replace_string(
		formatted_results,
		search_string,
		ELEMENT_MULTI_ATTRIBUTE_DISPLAY_DELIMITER );

	return formatted_results;

} /* format_results() */
