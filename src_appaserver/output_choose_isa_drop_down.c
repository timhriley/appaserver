/* $APPASERVER_HOME/src_appaserver/output_choose_isa_drop_down.c	*/
/* --------------------------------------------------------------------	*/
/*									*/
/* This is the drop down that displays to select a related folder that	*/
/* another folder inherents the attributes of.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtbl.h"
#include "timlib.h"
#include "list.h"
#include "attribute.h"
#include "document.h"
#include "application.h"
#include "folder.h"
#include "related_folder.h"
#include "dictionary.h"
#include "form.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "appaserver.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "role.h"

/* Constants */
/* --------- */
#define INSERT_UPDATE_KEY			"prompt"

/* Prototypes */
/* ---------- */
LIST *get_element_list(	
					char *login_name,
					char *application_name,
					char *session,
					char *folder_name,
					char *isa_related_folder_name,
					char *role_name,
					LIST *attribute_list,
					LIST *primary_attribute_name_list,
					PROCESS *populate_drop_down_process );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *role_name, *state;
	char *target_frame = PROMPT_FRAME;
	FORM *form;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	/* char *spool_filename; */
	LIST *attribute_list;
	LIST *primary_attribute_name_list;
	FOLDER *folder;
	char *isa_related_folder_name;
	ROLE *role;
	boolean with_dynarch_menu;
	char subtitle_string[ 128 ];
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s login_name ignored session folder related_isa_folder_name role state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	isa_related_folder_name = argv[ 5 ];
	role_name = argv[ 6 ];
	state = argv[ 7 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	folder = folder_new_folder(	application_name,
					session,
					isa_related_folder_name );

	role = role_new_role(	application_name,
				role_name );

	attribute_list =
		attribute_get_attribute_list(
			application_name,
			isa_related_folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	folder_load(	&folder->insert_rows_number,
			&folder->lookup_email_output,
			&folder->row_level_non_owner_forbid,
			&folder->row_level_non_owner_view_only,
			&folder->populate_drop_down_process,
			&folder->post_change_process,
			&folder->folder_form,
			&folder->notepad,
			&folder->html_help_file_anchor,
			&folder->post_change_javascript,
			&folder->lookup_before_drop_down,
			&folder->data_directory,
			&folder->index_directory,
			&folder->no_initial_capital,
			&folder->subschema_name,
			&folder->create_view_statement,
			application_name,
			session,
			folder->folder_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	form = form_new( INSERT_UPDATE_KEY,
			 application_get_title_string( application_name ) );

	form_set_folder_parameters(	form,
					state,
					login_name,
					application_name,
					session,
					folder_name,
					role_name );

	form->regular_element_list =
		get_element_list(
			login_name,
			application_name,
			session,
			folder_name,
			isa_related_folder_name,
			role_name,
			attribute_list,
			primary_attribute_name_list,
			folder->populate_drop_down_process );

	form_set_post_process( form, "post_choose_isa_drop_down" );
	form_set_target_frame( form, target_frame );
	form_set_output_row_zero_only( form );

	form->html_help_file_anchor = folder->html_help_file_anchor;

	document = document_new(
			application_get_title_string( application_name ),
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
			application_get_relative_source_directory(
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

	form_output_title(	form->application_title,
				form->state,
				form->form_title,
				form->folder_name,
				form->target_frame,
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
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(LIST *)0 /* form_button_list */,
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
			(char *)0 /* attribute_not_null */,
			(char *)0 /* appaserver_user_foreign_login_name */ );

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

	document_close();

	exit( 0 );
} /* main() */

LIST *get_element_list(		char *login_name,
				char *application_name,
				char *session,
				char *folder_name,
				char *isa_related_folder_name,
				char *role_name,
				LIST *attribute_list,
				LIST *primary_attribute_name_list,
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
			  primary_attribute_name_list,
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
	element = element_appaserver_new(
			hidden,
			"folder_name" );

	element_hidden_set_data(	element->hidden,
					folder_name );

	list_append( 	return_list, 
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
			  primary_attribute_name_list,
			  MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			  QUERY_RELATION_OPERATOR_STARTING_LABEL ) );

	element = element_appaserver_new(
			hidden,
			strdup( element_name ) );

	element_hidden_set_data(	element->hidden,
					EQUAL_OPERATOR );

	list_append( 	return_list, 
			element, 
			sizeof( ELEMENT_APPASERVER ) );

	return return_list;
}

