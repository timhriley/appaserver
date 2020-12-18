/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_choose_folder.c			*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "query.h"
#include "related_folder.h"
#include "appaserver.h"
#include "session.h"
#include "appaserver_user.h"
#include "application.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "dictionary.h"
#include "post2dictionary.h"
#include "role.h"
#include "lookup_before_drop_down.h"

#define TABLE_TARGET_FRAME		PROMPT_FRAME
#define INSERT_UPDATE_KEY		"prompt"

/* Prototypes */
/* ---------- */
char *prompt_edit_form_get_sys_string(
					char *login_name,
					char *application_name,
					char *session,
					char *folder_name,
					char *role_name,
					char *state );

char *prompt_insert_form_sys_string(
					char *login_name,
					char *application_name,
					char *session,
					char *folder_name,
					char *role_name,
					char *state );

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *session;
	char *folder_name;
	char *state;
	char *role_name;
	char *form;
	char sys_string[ 1024 ];
	LIST *isa_related_folder_list;
	APPASERVER *appaserver;
	ROLE *role;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name application session folder role state\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	state = argv[ 6 ];

	environ_set_environment(
		APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
		application_name );

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

	if ( !*folder_name )
	{
		DICTIONARY *post_dictionary;

		post_dictionary =
			post2dictionary(
				stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ );

		folder_name =
			dictionary_get_string(
				post_dictionary,
				CHOOSE_FOLDER_MENU_NAME );
	}

	role = role_new_role(	application_name,
				role_name );

	appaserver =
		appaserver_new_appaserver(
			application_name,
			session,
			folder_name );

	appaserver->folder->mto1_related_folder_list = 
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			appaserver->application_name,
			appaserver->session,
			appaserver->folder->folder_name,
			role_name,
			0 /* isa_flag */,
			related_folder_prompt_recursive_only,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	appaserver->folder->one2m_related_folder_list = 
		related_folder_1tom_related_folder_list(
			appaserver->application_name,
			appaserver->session,
			appaserver->folder->folder_name,
			role_name,
			other,
			(LIST *)0 /* primary_data_list */,
			list_new() /* related_folder_list */,
			0 /* dont omit_isa_relations */,
			related_folder_no_recursive,
			(LIST *)0 /* parent_primary_attribute_name_list */,
			(LIST *)0 /* original_primary_attribute_name_list */,
			(char *)0 /* prior_related_attribute_name */ );

	if ( related_folder_exists_pair_1tom(
				appaserver->
					folder->
					one2m_related_folder_list ) )
	{
		form = "prompt";
	}
	else
	{
		sprintf(sys_string,
		 	"form4folder.sh %s %s 2>>%s",
		 	application_name,
		 	folder_name,
		 	appaserver_error_get_filename( application_name ) );
		form = pipe2string( sys_string );
	}

	if ( !form || !*form ) form = "table";

	if ( strcmp( state, "insert" ) == 0 )
	{
		if ( ( isa_related_folder_list =
			related_folder_get_isa_related_folder_list(
			application_name,
			session,
			folder_name,
			role_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			related_folder_no_recursive ) ) )
		{
			RELATED_FOLDER *isa_related_folder;

			isa_related_folder = (RELATED_FOLDER *)
				list_get_first_pointer(
					isa_related_folder_list );

			sprintf( sys_string, 
"output_choose_isa_drop_down %s %s %s %s %s %s %s 2>>%s",
		 	 login_name,
			 application_name,
			 session,
			 folder_name,
			 isa_related_folder->folder->folder_name,
			 role_name, state, 
			 appaserver_error_get_filename( application_name ) );
		}
		else
		if ( strcmp( form, "prompt" ) == 0 )
		{
			strcpy( sys_string,
				prompt_insert_form_sys_string(
					login_name,
					application_name,
					session,
					folder_name,
					role_name,
					state ) );
		}
		else
		/* Must be form == "table" */
		{
			sprintf( sys_string,
"output_insert_table_form '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s'",
			 	login_name,
				application_name,
			 	session,
			 	folder_name,
			 	role_name,
			 	state,
			 	TABLE_TARGET_FRAME /* insert_update_key */,
			 	TABLE_TARGET_FRAME );
		}
	}
	else
	/* ------------------------------- */
	/* Must be either update or lookup */
	/* ------------------------------- */
	{
		if ( strcmp( form, "table" ) == 0 )
		{
			sprintf( sys_string,
"output_edit_table_form %s %s %s %s %s '' %s %s 2>>%s",
			 	 login_name,
				 application_name,
				 session,
				 folder_name,
				 role_name,
				 TABLE_TARGET_FRAME,
				 TABLE_TARGET_FRAME,
				 appaserver_error_get_filename(
					 application_name ) );
		}
		else
		if ( strcmp( form, "prompt" ) == 0 )
		{
			strcpy( sys_string,
				prompt_edit_form_get_sys_string(
					login_name,
					application_name,
					session,
					folder_name,
					role_name,
					state ) );
		}
	}

	if ( system( sys_string ) ){}

	return 0;
}

char *prompt_edit_form_get_sys_string(
					char *login_name,
					char *application_name,
					char *session,
					char *folder_name,
					char *role_name,
					char *state )
{
	static char sys_string[ 1024 ];
	DICTIONARY *lookup_before_drop_down_dictionary;
	char *unfulfilled_folder_name;
	LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down;

	lookup_before_drop_down_dictionary = dictionary_small_new();

	lookup_before_drop_down_set_dictionary_base_name(
		lookup_before_drop_down_dictionary,
		folder_name );

	lookup_before_drop_down =
		lookup_before_drop_down_new(
			application_name,
			lookup_before_drop_down_dictionary,
			state );

	if ( ! ( unfulfilled_folder_name =
		lookup_before_drop_down_get_unfulfilled_folder_name(
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list ) ) )
	{
		/* ---------------- */
		/* If no prelookups */
		/* ---------------- */
		sprintf( sys_string,
"output_prompt_edit_form %s %s %s %s %s %s '' 2>>%s",
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
	{
		/* -------------- */
		/* Has prelookups */
		/* -------------- */
		dictionary_set_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_STATE_KEY,
			LOOKUP_BEFORE_DROP_DOWN_INITIAL_STATE );

		sprintf( sys_string,
"output_prompt_edit_form %s %s %s %s %s %s '' '%s' 2>>%s",
	 		login_name,
			application_name,
	 		session,
	 		unfulfilled_folder_name,
	 		role_name,
	 		state,
			/* ----------------------------------------- */
			/* Set lookup_before_drop_down inital state. */
			/* ----------------------------------------- */
	 		dictionary_display_delimited(
				dictionary_add_prefix(
					lookup_before_drop_down_dictionary,
					LOOKUP_BEFORE_DROP_DOWN_PREFIX ),
				'&' ),
	 		appaserver_error_get_filename(
		 		application_name ) );
	}

	return sys_string;

}

char *prompt_insert_form_sys_string(
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *state )
{
	static char sys_string[ 1024 ];
	DICTIONARY *lookup_before_drop_down_dictionary;
	char *unfulfilled_folder_name;
	LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down;

	lookup_before_drop_down_dictionary = dictionary_small_new();

	lookup_before_drop_down_set_dictionary_base_name(
		lookup_before_drop_down_dictionary,
		folder_name );

	lookup_before_drop_down =
		lookup_before_drop_down_new(
			application_name,
			lookup_before_drop_down_dictionary,
			state );

{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: boolean = %d\n",
__FILE__,
__FUNCTION__,
__LINE__,
lookup_before_drop_down->omit_lookup_before_drop_down );
m2( application_name, msg );
}
	if ( !lookup_before_drop_down->omit_lookup_before_drop_down
	&&   ( lookup_before_drop_down->insert_pair_base_folder_name =
		lookup_before_drop_down_get_insert_pair_base_folder_name(
			application_name,
			folder_name,
			lookup_before_drop_down->
				base_folder->
				pair_one2m_related_folder_list ) ) )
	{
		lookup_before_drop_down_set_dictionary_insert_folder_name(
			lookup_before_drop_down_dictionary,
			folder_name );

		lookup_before_drop_down_set_dictionary_base_name(
			lookup_before_drop_down_dictionary,
			lookup_before_drop_down->insert_pair_base_folder_name );

		lookup_before_drop_down_append_fulfilled_folder_name(
			lookup_before_drop_down_dictionary,
			folder_name );

		lookup_before_drop_down =
			lookup_before_drop_down_new(
				application_name,
				lookup_before_drop_down_dictionary,
				(char *)0 /* state */ );
	}

	if ( ! ( unfulfilled_folder_name =
		lookup_before_drop_down_get_unfulfilled_folder_name(
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list ) ) )
	{
		/* If no prelookups */
		/* ---------------- */
		sprintf( sys_string, 
"output_prompt_insert_form %s %s %s %s %s %s n 2>>%s",
		 	login_name,
			application_name,
		 	session,
		 	folder_name,
		 	role_name,
			state, 
		 	appaserver_error_get_filename(
			application_name ) );

		return sys_string;
	}

	/* Has prelookups */
	/* -------------- */
	sprintf( sys_string,
"output_prompt_edit_form %s %s %s %s %s %s '' '%s' 2>>%s",
	 	login_name,
		application_name,
	 	session,
	 	unfulfilled_folder_name,
	 	role_name,
	 	state,
	 	dictionary_display_delimited(
			dictionary_add_prefix(
				lookup_before_drop_down_dictionary,
				LOOKUP_BEFORE_DROP_DOWN_PREFIX ),
			'&' ),
	 	appaserver_error_get_filename(
		 	application_name ) );

	return sys_string;

}

