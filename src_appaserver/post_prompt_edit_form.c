/* -------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_prompt_edit_form.c	*/
/* -------------------------------------------------------	*/
/*								*/
/* This script is attached to the submit button on 		*/
/* the lookup form.						*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "document.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_user.h"
#include "folder.h"
#include "related_folder.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "environ.h"
#include "query.h"
#include "post2dictionary.h"
#include "session.h"
#include "role.h"
#include "appaserver_parameter_file.h"
#include "lookup_before_drop_down.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean execute_radio_button_process_maybe(
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *state,
			char *insert_update_key,
			char *target_frame,
			pid_t dictionary_process_id,
			char *lookup_option_radio_button,
			DICTIONARY_APPASERVER *dictionary_appaserver );

void post_prompt_edit_form_lookup_before_drop_down(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			char *application_name,
			char *session,
			ROLE *role,
			char *login_name,
			char *state,
			char *fulfilled_folder_name );

boolean all_required_attributes_populated(
				LIST *lookup_required_attribute_name_list,
				DICTIONARY *query_dictionary );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *database_string = {0};
	char *role_name, *state;
	char *insert_update_key;
	char *target_frame;
	DICTIONARY *original_post_dictionary = {0};
	char *lookup_option_radio_button;
	pid_t dictionary_process_id;
	ROLE *role;
	LIST *attribute_list;
	LIST *lookup_required_attribute_name_list;
	char *escaped_dictionary_string;
	char *base_folder_name = {0};
	DICTIONARY_APPASERVER *dictionary_appaserver;
	char sys_string[ MAX_INPUT_LINE ];

	if ( argc == 2 )
	{
		char buffer[ 65536 ];
		char piece_buffer[ 4096 ];

		timlib_strcpy( buffer, argv[ 1 ], 65536 );

		unescape_string( buffer );

		if ( character_count( '^', buffer ) != 9 )
		{
			fprintf( stderr,
				 "Usage error for %s.\n",
				 argv[ 0 ] );
			exit( 1 );
		}

		piece( piece_buffer, '^', buffer, 0 );
		login_name = strdup( piece_buffer );

		piece( piece_buffer, '^', buffer, 1 );
		application_name = strdup( piece_buffer );

		piece( piece_buffer, '^', buffer, 2 );
		session = strdup( piece_buffer );

		piece( piece_buffer, '^', buffer, 3 );
		folder_name = strdup( piece_buffer );

		piece( piece_buffer, '^', buffer, 4 );
		role_name = strdup( piece_buffer );

		piece( piece_buffer, '^', buffer, 5 );
		state = strdup( piece_buffer );

		piece( piece_buffer, '^', buffer, 6 );
		insert_update_key = strdup( piece_buffer );

		piece( piece_buffer, '^', buffer, 7 );
		target_frame = strdup( piece_buffer );

		piece( piece_buffer, '^', buffer, 8 );
		dictionary_process_id = atoi( piece_buffer );

		piece( piece_buffer, '^', buffer, 9 );

		original_post_dictionary =
			dictionary_string2dictionary(
				piece_buffer );
	}
	else
	if ( argc < 10 )
	{
		fprintf( stderr, 
"Usage: %s login_name application session folder role state insert_update_key target_frame dictionary_process_id [dictionary]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}
	else
	{
		login_name = argv[ 1 ];
		application_name = argv[ 2 ];
		session = argv[ 3 ];
		folder_name = argv[ 4 ];
		role_name = argv[ 5 ];
		state = argv[ 6 ];
		insert_update_key = argv[ 7 ];
		target_frame = argv[ 8 ];
		dictionary_process_id = atoi( argv[ 9 ] );
	
		if ( argc == 11
		&&   *argv[ 10 ]
		&&   strcmp( argv[ 10 ], "null" ) != 0
		&&   strcmp( argv[ 10 ], "ignored" ) != 0 )
		{
			original_post_dictionary =
				dictionary_string2dictionary(
					argv[ 10 ] );
		}
	}

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

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );
	environ_appaserver_home();

	role =
		role_new_role(
			application_name,
			role_name );

	/* Promote source frame */
	/* -------------------- */
	insert_update_key = target_frame;

	if ( !original_post_dictionary )
	{
		original_post_dictionary =
			post2dictionary(
				stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ );
	}

	attribute_list =
		attribute_get_attribute_list(
			application_name,
			folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				original_post_dictionary,
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

	if ( dictionary_length( dictionary_appaserver->
					lookup_before_drop_down_dictionary ) )
	{
		base_folder_name =
			lookup_before_drop_down_dictionary_get_base_folder_name(
				dictionary_appaserver->
					lookup_before_drop_down_dictionary );

		if ( strcmp( base_folder_name, folder_name ) != 0 )
		{
			/* Copy over the preprompt dictionary */
			/* ---------------------------------- */
			dictionary_append_dictionary(
				dictionary_appaserver->
					preprompt_dictionary,
				dictionary_appaserver->
					non_prefixed_dictionary );

			post_prompt_edit_form_lookup_before_drop_down(
				dictionary_appaserver,
				application_name,
				session,
				role,
				login_name,
				state,
				folder_name );
			/* ---------------- */
			/* ^                */
			/* | Does an exit() */
			/* ---------------- */
		}

	}

	/* Copy over the query dictionary */
	/* ------------------------------ */
	dictionary_append_dictionary(
		dictionary_appaserver->query_dictionary,
		dictionary_appaserver->non_prefixed_dictionary );

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_access_folder(
				application_name,
				session,
				folder_name,
				role_name,
				state ) )
	{
		if ( !session_access_folder(
				application_name,
					session,
					folder_name,
					role_name,
					"lookup" ) )
		{
			session_access_failed_message_and_exit(
					application_name, session, login_name );
		}
		else
		{
			state = "lookup";
		}
	}

	if ( !appaserver_user_exists_role(
		login_name,
		role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	lookup_required_attribute_name_list =
		attribute_lookup_required_attribute_name_list(
			attribute_list );

	if ( !( lookup_option_radio_button =
			dictionary_get_string(
				dictionary_appaserver->
					non_prefixed_dictionary,
				LOOKUP_OPTION_RADIO_BUTTON_NAME ) ) )
	{
		lookup_option_radio_button = "";
	}

	if ( ( strcmp(	lookup_option_radio_button, 
			STATISTICS_PUSH_BUTTON_NAME ) != 0 && 
	       strcmp(	lookup_option_radio_button,
			TRANSMIT_PUSH_BUTTON_NAME ) != 0 &&
	       strcmp(	lookup_option_radio_button,
			RADIO_NEW_PUSH_BUTTON_NAME ) != 0 &&
	       strcmp(	lookup_option_radio_button,
			GROUP_PUSH_BUTTON_NAME ) != 0 &&
	       strcmp(	lookup_option_radio_button,
			HISTOGRAM_PUSH_BUTTON_NAME ) != 0 )
	&&   lookup_required_attribute_name_list
	&&   list_length( lookup_required_attribute_name_list ) )
	{
		if ( !all_required_attributes_populated(
				lookup_required_attribute_name_list,
				dictionary_appaserver->query_dictionary ) )
		{
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
			char buffer[ 1024 ];

			appaserver_parameter_file =
				new_appaserver_parameter_file();

			document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );
			printf(
"<h3>Warning: This lookup might drain too many resources. Please include at least the following in your search: %s.\n</h3>",
				format_initial_capital(
					buffer,
					list_display(
					lookup_required_attribute_name_list ) )
			);
			document_close();
			exit( 0 );
		}
	}

	if ( !execute_radio_button_process_maybe(
			application_name,
			login_name,
			session,
			folder_name,
			role_name,
			state,
			insert_update_key,
			target_frame,
			dictionary_process_id,
			lookup_option_radio_button,
			dictionary_appaserver ) )
	{
		char msg[ 1024 ];
		sprintf( msg,
		"ERROR in %s/%s()/%d: got invalid radio button data = (%s)",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			lookup_option_radio_button );
		m( msg );
		exit( 1 );
	}

	escaped_dictionary_string =
		dictionary_appaserver_escaped_send_dictionary_string(
			dictionary_appaserver,
			0 /* not with_non_prefixed_dictionary */ );

	sprintf(sys_string,
"echo \"%s\"								|"
"output_edit_table_form '%s' '%s' '%s' '%s' '%s' '' '%s' '%s'	 	 ",
		escaped_dictionary_string,
 		login_name,
		application_name,
 		session,
 		folder_name,
		role_name,
		insert_update_key,
		target_frame );

	if ( system( sys_string ) ) {};

	exit( 0 );

} /* main() */

boolean all_required_attributes_populated(
				LIST *lookup_required_attribute_name_list,
				DICTIONARY *query_dictionary )
{
	char *lookup_required_attribute_name;
	char key[ 128 ];


	if ( !list_rewind( lookup_required_attribute_name_list ) ) return 1;

	do {
		lookup_required_attribute_name =
			list_get_pointer( 
				lookup_required_attribute_name_list );

		sprintf( key, "%s_0", lookup_required_attribute_name );

		if ( !dictionary_exists_key( query_dictionary, key ) )
		{
			sprintf( key, "%s_1", lookup_required_attribute_name );

			if ( !dictionary_exists_key( query_dictionary, key ) )
			{
				sprintf( key,
					 "%s%s_0",
					 QUERY_FROM_STARTING_LABEL,
					 lookup_required_attribute_name );

				if ( !dictionary_exists_key(
					query_dictionary,
					key ) )
				{
					return 0;
				}
			}
		}
	} while( list_next( lookup_required_attribute_name_list ) );

	return 1;

} /* all_required_attributes_populated() */

void post_prompt_edit_form_lookup_before_drop_down(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			char *application_name,
			char *session,
			ROLE *role,
			char *login_name,
			char *state,
			char *fulfilled_folder_name )
{
	LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down;
	char *unfulfilled_folder_name;

	lookup_before_drop_down =
		lookup_before_drop_down_new(
			application_name,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			state );

	if ( strcmp( state, "insert" ) == 0 )
	{
		lookup_before_drop_down->insert_folder_name =
		      lookup_before_drop_down_get_dictionary_insert_folder_name(
				dictionary_appaserver->
					lookup_before_drop_down_dictionary );
	}

	dictionary_set_pointer(
		dictionary_appaserver->
			lookup_before_drop_down_dictionary,
		LOOKUP_BEFORE_DROP_DOWN_STATE_KEY,
		LOOKUP_BEFORE_DROP_DOWN_NON_INITIAL_STATE );

	lookup_before_drop_down->lookup_before_drop_down_state =
		lookup_before_drop_down_get_state(
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			dictionary_appaserver->preprompt_dictionary,
			0 /* folder_lookup_before_drop_down */ );

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state !=
			lookup_skipped )
	{
		lookup_before_drop_down_set_fulfilled_folder_name(
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list,
			fulfilled_folder_name );

		lookup_before_drop_down_set_dictionary_fulfilled(
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list );

		lookup_before_drop_down->lookup_before_drop_down_state =
			lookup_before_drop_down_get_state(
				lookup_before_drop_down->
					lookup_before_drop_down_folder_list,
				dictionary_appaserver->
					lookup_before_drop_down_dictionary,
				dictionary_appaserver->preprompt_dictionary,
				0 /* folder_lookup_before_drop_down */ );
	}

	unfulfilled_folder_name =
		lookup_before_drop_down_get_unfulfilled_folder_name(
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list );

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state != lookup_skipped
	&&   unfulfilled_folder_name )
	{
		/* Execute another pre-lookup form. */
		/* -------------------------------- */
		form_execute_output_prompt_edit_form(
			application_name,
			login_name,
			session,
			unfulfilled_folder_name,
			role->role_name,
			state,
			appaserver_error_get_filename(
					application_name ),
			dictionary_appaserver,
			PROMPT_FRAME /* target_frame */ );
		exit( 0 );
	}

	/* If lookup is skipped or all fulfilled. */
	/* -------------------------------------- */
	if ( strcmp( state, "lookup" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		/* If skipped */
		/* ---------- */
		if ( unfulfilled_folder_name )
		{
			dictionary_set_pointer(
				dictionary_appaserver->
					lookup_before_drop_down_dictionary,
				LOOKUP_BEFORE_DROP_DOWN_STATE_KEY,
				LOOKUP_BEFORE_DROP_DOWN_SKIPPED_STATE );
		}

		/* Execute the base folder's lookup form. */
		/* -------------------------------------- */
		form_execute_output_prompt_edit_form(
			application_name,
			login_name,
			session,
			lookup_before_drop_down->base_folder->folder_name,
			role->role_name,
			state,
			appaserver_error_get_filename(
					application_name ),
			dictionary_appaserver,
			EDIT_FRAME /* target_frame */ );
		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0 )
	{
		char *folder_name;

		/* If skipped */
		/* ---------- */
		if ( unfulfilled_folder_name )
		{
			dictionary_set_pointer(
				dictionary_appaserver->
					lookup_before_drop_down_dictionary,
				LOOKUP_BEFORE_DROP_DOWN_STATE_KEY,
				LOOKUP_BEFORE_DROP_DOWN_SKIPPED_STATE );
		}

		folder_name =
		      lookup_before_drop_down_get_dictionary_insert_folder_name(
				dictionary_appaserver->
					lookup_before_drop_down_dictionary );

		if ( !folder_name )
		{
			folder_name =
				lookup_before_drop_down->
					base_folder->
					folder_name;
		}

		/* Execute the base folder's insert form. */
		/* -------------------------------------- */
		form_execute_output_prompt_insert_form(
			application_name,
			login_name,
			session,
			folder_name,
			role->role_name,
			state,
			appaserver_error_get_filename(
					application_name ),
			dictionary_appaserver );
		exit( 0 );
	}

	fprintf(stderr,
		"ERROR in %s/%s()/%d: logic error.\n",
		__FILE__,
		__FUNCTION__,
		__LINE__ );
	exit( 1 );

} /* post_prompt_edit_form_lookup_before_drop_down() */

boolean execute_radio_button_process_maybe(
				char *application_name,
				char *login_name,
				char *session,
				char *folder_name,
				char *role_name,
				char *state,
				char *insert_update_key,
				char *target_frame,
				pid_t dictionary_process_id,
				char *lookup_option_radio_button,
				DICTIONARY_APPASERVER *dictionary_appaserver )
{
	char *escaped_dictionary_string;
	char sys_string[ MAX_INPUT_LINE ];

	*sys_string = '\0';

	if ( dictionary_data_exists_index_zero(
				dictionary_appaserver->
					non_prefixed_dictionary,
				EMAIL_OUTPUT_NAME ) )
	{
		escaped_dictionary_string =
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ );

		sprintf(sys_string,
"echo \"%s\"								|"
"output_email %s %s %s %s %s %s dictionary_stdin  		 	 ",
			escaped_dictionary_string,
		 	login_name,
			application_name,
		 	session,
		 	folder_name,
			role_name,
			state );
	}
	else
	if ( *lookup_option_radio_button
	&&    strcmp( lookup_option_radio_button, "lookup" ) != 0 )
	{
		if ( strcmp(	lookup_option_radio_button,
				STATISTICS_PUSH_BUTTON_NAME ) == 0 )
		{
			escaped_dictionary_string =
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ );

			sprintf( sys_string,
			"statistics_folder %s %s %s %s %s \"%s\" 2>>%s",
		 		login_name,
				application_name,
		 		session,
		 		folder_name,
				role_name,
				escaped_dictionary_string,
				appaserver_error_get_filename(
					application_name ) );
		}
		else
		if ( strcmp(	lookup_option_radio_button,
				DELETE_PUSH_BUTTON_NAME ) == 0 )
		{
			escaped_dictionary_string =
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ );

			sprintf( sys_string,
		"post_delete_folder_block %s %s %s %s %s one \"%s\" 2>>%s",
		 		login_name,
				application_name,
		 		session,
		 		folder_name,
				role_name,
				escaped_dictionary_string,
				appaserver_error_get_filename(
					application_name ) );
		}
		else
		if ( strcmp(	lookup_option_radio_button,
				SORT_ORDER_PUSH_BUTTON_NAME ) == 0 )
		{
			escaped_dictionary_string =
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ );

			sprintf( sys_string,
		"post_change_sort_order %s %s %s %s %s one \"%s\" 2>>%s",
		 		login_name,
				application_name,
		 		session,
		 		folder_name,
				role_name,
				escaped_dictionary_string,
				appaserver_error_get_filename(
					application_name ) );
		}
		else
		if ( strcmp(	lookup_option_radio_button,
				RADIO_NEW_PUSH_BUTTON_NAME ) == 0 )
		{
			escaped_dictionary_string =
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ );

			state = "insert";

			sprintf( sys_string,
	"echo \"%s\" 							  |"
"post_prompt_insert_form %s %s %s %s '%s' '%s' '%s' '%s' %d '' 2>>%s   ",
				escaped_dictionary_string,
		 		login_name,
				application_name,
		 		session,
		 		folder_name,
				role_name,
				state,
				insert_update_key,
				target_frame,
				dictionary_process_id,
				appaserver_error_get_filename(
					application_name ) );
		}
		else
		if ( strcmp(	lookup_option_radio_button,
				TRANSMIT_PUSH_BUTTON_NAME ) == 0 )
		{
			escaped_dictionary_string =
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ );


			sprintf(sys_string,
"echo \"%s\"								|"
"output_transmit %s %s %s %s %s %s dictionary_stdin 2>>%s	 	 ",
				escaped_dictionary_string,
		 		login_name,
				application_name,
		 		session,
		 		folder_name,
				role_name,
				state,
				appaserver_error_get_filename(
					application_name ) );
		}
		else
		if ( strcmp(	lookup_option_radio_button,
				GRACE_CHART_PUSH_BUTTON_NAME ) == 0 )
		{
			escaped_dictionary_string =
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ );

			sprintf(sys_string,
"echo \"%s\"								|"
"output_grace_chart %s %s %s %s %s %s dictionary_stdin 2>>/%s 	 	 ",
				escaped_dictionary_string,
		 		login_name,
				application_name,
		 		session,
		 		folder_name,
				role_name,
				state,
				appaserver_error_get_filename(
					application_name ) );
		}
		else
		if ( strcmp(	lookup_option_radio_button,
				GOOGLE_CHART_PUSH_BUTTON_NAME ) == 0 )
		{
			escaped_dictionary_string =
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ );

			sprintf(sys_string,
"echo \"%s\"								|"
"output_google_chart %s %s %s %s %s %s dictionary_stdin 2>>/%s 	 	 ",
				escaped_dictionary_string,
		 		login_name,
				application_name,
		 		session,
		 		folder_name,
				role_name,
				state,
				appaserver_error_get_filename(
					application_name ) );
		}
		else
		if ( strcmp(	lookup_option_radio_button,
				HISTOGRAM_PUSH_BUTTON_NAME ) == 0 )
		{
			escaped_dictionary_string =
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ );

			sprintf(sys_string,
"echo \"%s\"								|"
"output_histogram %s %s %s %s %s %s dictionary_stdin 2>>%s	 	 ",
				escaped_dictionary_string,
		 		login_name,
				application_name,
		 		session,
		 		folder_name,
				role_name,
				state,
				appaserver_error_get_filename(
					application_name ) );
		}
		else
		if ( strcmp(	lookup_option_radio_button,
				GROUP_PUSH_BUTTON_NAME ) == 0 )
		{
			escaped_dictionary_string =
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ );

			sprintf(sys_string,
			"echo \"%s\" | output_group %s %s %s %s 2>>%s",
				escaped_dictionary_string,
				application_name,
		 		folder_name,
				login_name,
				role_name,
				appaserver_error_get_filename(
					application_name ) );
		}
		else
		{
			return 0;
		}
	}

	if ( *sys_string )
	{
		if ( system( sys_string ) ) {};
		exit( 0 );
	}

	return 1;

} /* execute_radio_button_process_maybe() */

