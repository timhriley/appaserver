/* $APPASERVER_HOME/src_appaserver/output_choose_isa_drop_down.c	*/
/* --------------------------------------------------------------------	*/
/*									*/
/* This is the drop down that displays to select a relation that	*/
/* another folder inherents the attributes of.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"
#include "timlib.h"
#include "list.h"
#include "attribute.h"
#include "document.h"
#include "application.h"
#include "folder.h"
#include "relation.h"
#include "dictionary.h"
#include "form.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "appaserver.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "role.h"
#include "session.h"
#include "choose_isa_drop_down.h"

/* Constants */
/* --------- */
#define INSERT_UPDATE_KEY		"prompt"

/* Prototypes */
/* ---------- */
LIST *output_choose_isa_drop_down_element_list(	
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			char *isa_related_folder_name,
			char *role_name,
			LIST *attribute_list,
			LIST *primary_key_list,
			PROCESS *populate_drop_down_process );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *session_key;
	char *folder_name;
	char *one2m_isa_folder_name;
	char *role_name;
	SESSION *session;
	CHOOSE_ISA_DROP_DOWN *choose_isa_drop_down;
	FORM *form;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FOLDER *folder;
	ROLE *role;
	boolean with_dynarch_menu;
	char subtitle_string[ 128 ];
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( argc != 6 )
	{
		fprintf( stderr, 
	"Usage: %s login_name session folder one2m_isa_folder_name role\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name =
		environ_exit_application_name(
			argv[ 0 ] );

	login_name = argv[ 1 ];
	session_key = argv[ 2 ];
	folder_name = argv[ 3 ];
	one2m_isa_folder_name = security_sql_injection_escape( argv[ 4 ] );
	role_name = argv[ 5 ];

	session =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* --------------------------------------------- */
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			login_name,
			session_key,
			folder_name,
			role_name,
			"insert" /* state */ );

	if ( !session )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: session_folder_integrity_exit() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	choose_isa_drop_down =
		choose_isa_drop_down_fetch(
			session->login_name,
			session->session_key,
			session->folder_name,
			one2m_folder_name,
			session->role_name );

	if ( !choose_isa_drop_down )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: choose_isa_drop_down_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	form =
		form_new(
			"insert" /* form_name */,
			application_title_string(
				application_name ) );

	form->state = "insert";
	form->login_name = login_name;
	form->application_name = application_name;
	form->session = session_key;
	form->folder_name = folder_name;
	form->role_name = role_name;
	form->drop_down_number_columns = 2;

	form->regular_element_list =
		output_choose_isa_drop_down_element_list(
			login_name,
			application_name,
			session,
			folder_name,
			isa_related_folder_name,
			role_name,
			attribute_list,
			primary_key_list,
			folder->populate_drop_down_process );

	form_set_post_process( form, "post_choose_isa_drop_down" );
	form->target_frame = PROMPT_FRAME;
	form->output_row_zero_only = 1;
	form->html_help_file_anchor = folder->html_help_file_anchor;

	document =
		document_new(
			application_title_string( application_name ),
			application_name );


	document->output_content_type = 1;

	with_dynarch_menu =
		appaserver_frameset_menu_horizontal(
			application_name,
			login_name );

	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "form" );

	document_output_head(
		document->application_name,
		document->title,
		document->output_content_type,
		appaserver_parameter_file->appaserver_mount_point,
		document->javascript_module_list,
		document->stylesheet_filename,
		application_relative_source_directory(
			application_name ),
		with_dynarch_menu );

	if ( appaserver_frameset_menu_horizontal(
			application_name,
			login_name ) )
	{
		char sys_string[ 1024 ];

		document_output_dynarch_html_body(
				DOCUMENT_DYNARCH_MENU_ONLOAD_CONTROL_STRING,
				document->onload_control_string );

		printf( "<ul id=menu>\n" );

		sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '%s' %c %c 2>>%s",
				application_name,
				session,
				login_name,
				role_name,
				"" /* title */,
				'n' /* not content_type_yn */,
				'y' /* omit_html_head_yn */,
				appaserver_error_get_filename(
					application_name ) );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );
		printf( "</ul>\n" );
	}
	else
	{
		document_output_body(
			document->application_name,
			document->onload_control_string );
	}

	/* The beginning carrot makes the subtitle <h3> */
	/* -------------------------------------------- */
	sprintf( subtitle_string,
		 "^Is this %s an existing %s?",
		 format_initial_capital( buffer1, folder_name ),
		 format_initial_capital( buffer2, isa_related_folder_name ) );

	form->subtitle_string = subtitle_string;

	form_output_title(
		form->application_title,
		form->state,
		form->form_title,
		form->folder_name,
		form->subtitle_string,
		0 /* not omit_format_initial_capital */ );

	form->table_border = 1;

	form_output_heading(
		form->login_name,
		form->application_name,
		form->session,
		form->form_name,
		form->post_process,
		form->action_string,
		form->folder_name,
		form->role_name,
		form->state,
		form->target_frame,
		form->target_frame,
		0 /* no output_submit_reset_buttons */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* post_change_javascript */ );

	form_output_body(
		&form->current_reference_number,
		form->hidden_name_dictionary,
		form->output_row_zero_only,
		form->row_dictionary_list,
		form->regular_element_list,
		(LIST *)0 /* viewonly_element_list */,
		(char *)0 /* spool_filename */,
		0 /* row_level_non_owner_view_only */,
		application_name,
		login_name,
		(char *)0 /* attribute_not_null_string */,
		(char *)0 /* appaserver_user_foreign_login_name */,
		(LIST *)0 /* non_edit_folder_name_list */ );

	printf( "</table>\n" );
	printf( "<table border=0>\n" );

	form_output_trailer(
		1 /* output_submit_reset_buttons */,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		with_dynarch_menu /* with_back_to_top_button */,
		0 /* form_number */,
		(LIST *)0 /* form_button_list */,
		(char *)0 /* post_change_javascript */ );

	printf( "</table>\n" );
	printf( "</form>\n" );

	document_close();

	return 0;
}

LIST *output_choose_isa_drop_down_element_list(
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			char *isa_related_folder_name,
			char *role_name,
			LIST *attribute_list,
			LIST *primary_key_list,
			PROCESS *populate_drop_down_process )
{
	LIST *return_list;
	ELEMENT_APPASERVER *element;
	char element_name[ 256 ];
	char buffer[ 256 ];

	return_list = list_new();

	/* Create the line break */
	/* --------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append( 	return_list, 
			element, 
			sizeof( ELEMENT_APPASERVER ) );

	/* Create the prompt element */
	/* ------------------------- */
	sprintf( element_name,
		 "%s%s",
		 APPASERVER_ISA_PROMPT_PREFIX,
		 list_display_delimited(
			  primary_key_list,
			  MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER));

	element = element_appaserver_new(
			prompt,
			strdup( format_initial_capital( 
					buffer, 
					element_name ) ) );

	list_append( 	return_list, 
			element, 
			sizeof( ELEMENT_APPASERVER ) );

	/* Create the drop down element */
	/* ---------------------------- */
	element = element_appaserver_new(
			drop_down,
			strdup( element_name ) );

	element->drop_down->option_data_list =
		folder_primary_data_list(
			application_name,
			session,
			isa_related_folder_name,
			login_name,
			(DICTIONARY *)0 /* parameter_dictionary */,
			(DICTIONARY *)0 /* where_clause_dictionary */,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			populate_drop_down_process,
			attribute_list,
			(LIST *)0 /* common_non_primary_attribute_... */,
			0 /* filter_only_login_name */,
			(LIST *)0 /*  exclude_attribute_name_list */,
			role_name,
			(char *)0 /* state */,
			(char *)0 /* one2m_folder_name_for_processes */,
			(char *)0 /* appaserver_user_foreign_login_name */,
			0 /* not include_root_folder */ );

	list_append( 	return_list, 
			element, 
			sizeof( ELEMENT_APPASERVER ) );

	/* Create a hidden folder_name */
	/* --------------------------- */
	element =
		element_appaserver_new(
			hidden,
			"folder_name" );

	element->hidden->data = strdup( folder_name );

	list_append(
		return_list, 
		element, 
		sizeof( ELEMENT_APPASERVER ) );

	/* Create the lookup push button */
	/* ----------------------------- */
	element = element_appaserver_new( linebreak, "" );

	list_append( 	return_list, 
			element, 
			sizeof( ELEMENT_APPASERVER ) );

	element = element_appaserver_new( 	toggle_button, 
				LOOKUP_PUSH_BUTTON_NAME );

	element_toggle_button_set_heading(
		element->toggle_button, "lookup" );

	list_append(	return_list, 
			element, 
			sizeof( ELEMENT_APPASERVER ) );

	/* Create a hidden query relational operator equals */
	/* ------------------------------------------------ */
	sprintf( element_name, 
		 "%s",
		 list_display_delimited_prefixed(
			  primary_key_list,
			  MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			  QUERY_RELATION_OPERATOR_STARTING_LABEL ) );

	element =
		element_appaserver_new(
			hidden,
			strdup( element_name ) );

	element->hidden->data = EQUAL_OPERATOR;

	list_append(
		return_list, 
		element, 
		sizeof( ELEMENT_APPASERVER ) );

	return return_list;
}

