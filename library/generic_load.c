/* $APPASERVER_HOME/library/generic_load.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "sql.h"
#include "String.h"
#include "piece.h"
#include "document.h"
#include "dictionary.h"
#include "timlib.h"
#include "folder.h"
#include "relation.h"
#include "appaserver_error.h"
#include "role.h"
#include "appaserver_parameter.h"
#include "role_folder.h"
#include "form.h"
#include "frameset.h"
#include "element.h"
#include "security.h"
#include "row_security.h"
#include "application.h"
#include "generic_load.h"

GENERIC_LOAD_CHOOSE *generic_load_choose_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			boolean menu_boolean,
			boolean frameset_menu_horizontal )
{
	GENERIC_LOAD_CHOOSE *generic_load_choose;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	generic_load_choose = generic_load_choose_calloc();

	generic_load_choose->role =
		role_fetch(
			role_name,
			0 /* not fetch_attribute_exclude_list */ );

	if ( menu_boolean )
	{
		generic_load_choose->folder_menu =
			folder_menu_new(
				application_name,
				session_key,
				role_name,
				appaserver_parameter_data_directory() );

		if ( !generic_load_choose->folder_menu )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_menu_new() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		generic_load_choose->menu =
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				frameset_menu_horizontal,
				generic_load_choose->
					folder_menu->
					count_list );

		if ( !generic_load_choose->menu )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: menu_new() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	} /* if menu_boolean */

	generic_load_choose->post_action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_choose_post_action_string(
			GENERIC_LOAD_CHOOSE_POST_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	generic_load_choose->prompt_html =
		/* -------------------- */
		/* Return static memory */
		/* -------------------- */
		generic_load_choose_prompt_html(
			process_name );

	generic_load_choose->title_string =
		/* -------------------- */
		/* Return static memory */
		/* -------------------- */
		generic_load_choose_title_string(
			process_name );

	generic_load_choose->document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_new(
			application_name,
			generic_load_choose->title_string,
			(char *)0 /* title_html */,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			menu_boolean,
			generic_load_choose->menu,
			document_head_menu_setup_string( 
				menu_boolean ),
			(char *)0 /* calendar_setup_string */,
			(char *)0 /* javascript_include_string */,
			(char *)0 /* input_onload_string */ );

	generic_load_choose->generic_load_choose_form =
		generic_load_choose_form_new(
			generic_load_choose->prompt_html,
			GENERIC_LOAD_DROP_DOWN_NAME,
			role_folder_name_list(
				role_folder_insert_list(
					role_name ) ),
			generic_load_choose->post_action_string );

	if ( !generic_load_choose->generic_load_choose_form )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: generic_load_choose_form_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	generic_load_choose->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_choose_html(
			generic_load_choose->document->html,
			generic_load_choose->document->document_head->html,
			document_head_close_html(),
			generic_load_choose->document->document_body->html,
			generic_load_choose->generic_load_choose_form->html,
			document_body_close_html(),
			document_close_html() );

	return generic_load_choose;
}

char *generic_load_choose_post_action_string(
			char *choose_post_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name )
{
	char post_action_string[ 1024 ];

	if ( !choose_post_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(post_action_string,
		"action=\"%s/%s?%s+%s+%s+%s+%s\"",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		choose_post_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		process_name );

	return strdup( post_action_string );
}

char *generic_load_choose_prompt_html(
			char *process_name )
{
	static char prompt_html[ 128 ];
	char buffer[ 64 ];

	sprintf(prompt_html,
		"<h3>%s choose a table:</h3>",
		string_initial_capital(
			buffer,
			process_name ) );

	return prompt_html;
}

char *generic_load_choose_title_string(
			char *process_name )
{
	static char title_string[ 128 ];

	string_initial_capital(
		title_string,
		process_name );

	return title_string;
}

char *generic_load_choose_html(
			char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *document_body_html,
			char *form_generic_load_html,
			char *document_body_close_html,
			char *document_close_html )
{
	char html[ STRING_64K ];

	if ( !document_html
	||   !document_head_html
	||   !document_head_close_html
	||   !document_body_html
	||   !form_generic_load_html
	||   !document_body_close_html
	||   !document_close_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s\n%s\n%s\n%s",
		document_html,
		document_head_html,
		document_head_close_html,
		document_body_html,
		form_generic_load_html,
		document_body_close_html,
		document_close_html );

	return strdup( html );
}

GENERIC_LOAD_CHOOSE *generic_load_choose_calloc( void )
{
	GENERIC_LOAD_CHOOSE *generic_load_choose;

	if ( ! ( generic_load_choose =
			calloc( 1, sizeof( GENERIC_LOAD_CHOOSE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_choose;
}

GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form_calloc( void )
{
	GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form;

	if ( ! ( generic_load_choose_form =
			calloc( 1, sizeof( GENERIC_LOAD_CHOOSE_FORM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_choose_form;
}

GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form_new(
			char *prompt_html,
			char *drop_down_name,
			LIST *role_folder_insert_name_list,
			char *post_action_string )
{
	GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form =
		generic_load_choose_form_calloc();

	generic_load_choose_form->tag_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag_html(
			"generic_load_choose" /* form_name */,
			post_action_string,
			FRAMESET_PROMPT_FRAME /* target_frame */ );

	generic_load_choose_form->element_list =
		generic_load_choose_form_element_list(
			prompt_html,
			role_folder_insert_name_list,
			drop_down_name );

	generic_load_choose_form->element_list_html =
		generic_load_choose_form_element_list_html(
			generic_load_choose_form->element_list );

	if ( !generic_load_choose_form->element_list_html )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: generic_load_choose_form_element_list_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	generic_load_choose_form->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_choose_form_html(
			generic_load_choose_form->tag_html,
			generic_load_choose_form->element_list_html,
			form_close_html() );

	free( generic_load_choose_form->element_list_html );

	return generic_load_choose_form;
}

LIST *generic_load_choose_form_element_list(
			char *prompt_html,
			LIST *role_folder_insert_name_list,
			char *drop_down_name )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	/* Create a table */
	/* -------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_open,
				(char *)0 /* element_name */ ) ) );

	/* Create a table row */
	/* ------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	/* Create the prompt */
	/* ----------------- */
	list_set(
		element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				non_edit_text,
				(char *)0 /* element_name */ ) ) );

	element->non_edit_text->message = prompt_html;

	/* Create a drop-down */
	/* ------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				drop_down,
				(char *)0 /* element_name */ ) ) );

	free( element->drop_down );

	element->drop_down =
		element_drop_down_new(
			drop_down_name,
			(LIST *)0 /* attribute_name_list */,
			role_folder_insert_name_list /* delimited_list */,
			0 /* not no_initial_capital */,
			0 /* not output_null_option */,
			0 /* not output_not_null_option */,
			0 /* not output_select_option */,
			element_drop_down_display_size(
				list_length(
					role_folder_insert_name_list ) ),
			-1 /* tab order */,
			0 /* not multi_select */,
			(char *)0 /* post_change_javascript */,
			0 /* not readonly */,
			0 /* not recall */ );

	/* Close the table */
	/* --------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_close,
				(char *)0 /* element_name */ ) ) );

	return element_list;
}

char *generic_load_choose_form_element_list_html(
			LIST *element_list )
{
	if ( !list_length( element_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list_length() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	appaserver_element_list_html(
			element_list /* in/out */ );
}

char *generic_load_choose_form_html(
			char *tag_html,
			char *element_list_html,
			char *form_close_html )
{
	char html[ STRING_64K ];

	if ( !tag_html
	||   !element_list_html
	||   !form_close_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s\n%s",
		tag_html,
		element_list_html,
		form_close_html );

	return strdup( html );
}

GENERIC_LOAD_CHOOSE_POST *generic_load_choose_post_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			DICTIONARY *working_post_dictionary )
{
	GENERIC_LOAD_CHOOSE_POST *generic_load_choose_post =
		generic_load_choose_post_calloc();

	generic_load_choose_post->folder_name =
	/* --------------------------------------------------------------- */
	/* Returns working_post_dictionary->hash_table->other_data or null */
	/* --------------------------------------------------------------- */
		generic_load_choose_post_folder_name(
			GENERIC_LOAD_DROP_DOWN_NAME,
			working_post_dictionary );

	if ( !generic_load_choose_post->folder_name )
	{
		return (GENERIC_LOAD_CHOOSE_POST *)0;
	}

	generic_load_choose_post->system_string =
		generic_load_choose_post_system_string(
			application_name,
			session_key,
			login_name,
			role_name,
			GENERIC_LOAD_FOLDER_EXECUTABLE,
			generic_load_choose_post->folder_name );

	return generic_load_choose_post;
}

char *generic_load_choose_post_folder_name(
			char *drop_down_name,
			DICTIONARY *working_post_dictionary )
{
	return
	dictionary_get(
		drop_down_name,
		working_post_dictionary );
}

char *generic_load_choose_post_system_string(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *generic_load_folder_executable,
			char *post_folder_name )
{
	char system_string[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !post_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %s %s %s",
		generic_load_folder_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		post_folder_name );

	return strdup( system_string );
}

GENERIC_LOAD_CHOOSE_POST *generic_load_choose_post_calloc( void )
{
	GENERIC_LOAD_CHOOSE_POST *generic_load_choose_post;

	if ( ! ( generic_load_choose_post =
			calloc( 1, sizeof( GENERIC_LOAD_CHOOSE_POST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_choose_post;
}

GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form_new(
			char *prompt_html,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *post_action_string,
			char *login_name )
{
	GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form =
		generic_load_folder_form_calloc();

	generic_load_folder_form->tag_html =
		form_tag_html(
			GENERIC_LOAD_FOLDER_FORM_NAME,
			post_action_string,
			FRAMESET_EDIT_FRAME /* target_frame */ );

	generic_load_folder_form->generic_load_folder_element_list =
		generic_load_folder_element_list_new(
			folder_attribute_list,
			relation_mto1_non_isa_list,
			GENERIC_LOAD_UPLOAD_LABEL,
			GENERIC_LOAD_SKIP_HEADER_ROWS,
			login_name );

	if ( !generic_load_folder_form->generic_load_folder_element_list )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: generic_load_folder_element_list_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	generic_load_folder_form->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_folder_form_html(
			generic_load_folder_form->tag_html,
			prompt_html,
			generic_load_folder_form->
				generic_load_folder_element_list->
				html,
			form_close_html() );

	free( generic_load_folder_form->
		generic_load_folder_element_list->
		html );

	return generic_load_folder_form;
}

char *generic_load_folder_form_html(
			char *tag_html,
			char *prompt_html,
			char *folder_element_list_html,
			char *form_close_html )
{
	char html[ STRING_64K ];

	if (	!tag_html
	||	!prompt_html
	||	!folder_element_list_html
	||	!form_close_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s",
		tag_html,
		prompt_html,
		folder_element_list_html,
		form_close_html );

	return strdup( html );
}

GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form_calloc(
			void )
{
	GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form;

	if ( ! ( generic_load_folder_form =
			calloc( 1, sizeof( GENERIC_LOAD_FOLDER_FORM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_folder_form;
}

GENERIC_LOAD_FOLDER_ELEMENT_LIST *
	generic_load_folder_element_list_new(
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *generic_load_upload_label,
			char *generic_load_skip_header_rows,
			char *login_name )
{
	FOLDER_ATTRIBUTE *folder_attribute;

	GENERIC_LOAD_FOLDER_ELEMENT_LIST *generic_load_folder_element_list =
		generic_load_folder_element_list_calloc();

	generic_load_folder_element_list->element_list = list_new();

	if ( !list_length( folder_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_set_list(
		generic_load_folder_element_list->element_list,
		generic_load_buttons_element_list() );

	list_set_list(
		generic_load_folder_element_list->element_list,
		generic_load_upload_element_list(
			generic_load_upload_label ) );

	list_set_list(
		generic_load_folder_element_list->element_list,
		generic_load_skip_header_rows_element_list(
			generic_load_skip_header_rows ) );

	list_set_list(
		generic_load_folder_element_list->element_list,
		generic_load_execute_checkbox_element_list() );

	list_set_list(
		generic_load_folder_element_list->element_list,
		generic_load_dialog_box_element_list() );

	list_rewind( folder_attribute_list );

	do {
		folder_attribute = list_get( folder_attribute_list );

		if ( !folder_attribute->attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		generic_load_folder_element_list->
			generic_load_attribute_element_list =
				generic_load_attribute_element_list_new(
					relation_mto1_non_isa_list,
					folder_attribute,
					login_name,
					++generic_load_folder_element_list->
						position );

		if ( !generic_load_folder_element_list->
			generic_load_attribute_element_list )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: generic_load_attribute_element_list_new() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set_list(
			generic_load_folder_element_list->element_list,
			generic_load_folder_element_list->
				generic_load_attribute_element_list->
					element_list );

	} while( list_next( folder_attribute_list ) );

	generic_load_folder_element_list->html =
		generic_load_folder_element_list_html(
			generic_load_folder_element_list->element_list );

	return generic_load_folder_element_list;
}

LIST *generic_load_buttons_element_list( void )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_open,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	/* Create <Submit> */
	/* --------------- */
	list_set(
		element_list,
		appaserver_element_new(
			table_data,
			(char *)0 /* element_name */ ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				button,
				(char *)0 /* element_name */ ) ) );

	element->button->button =
		button_submit(
			(char *)0 /* multi_select_all_javascript */,
			(char *)0 /* keystrokes_save_javascript */,
			(char *)0 /* keystrokes_multi_save_javascript */,
			(char *)0 /* verify_attribute_widths_javascript */,
			0 /* form_number */ );

	/* Create <Reset> */
	/* -------------- */
	list_set(
		element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				button,
				(char *)0 /* element_name */ ) ) );

	element->button->button =
		button_reset(
			(char *)0 /* javascript_replace */,
			0 /* form_number */ );

	list_set(
		element_list,
		( element = appaserver_element_new(
				table_close,
				(char *)0 /* element_name */ ) ) );

	return element_list;
}

LIST *generic_load_upload_element_list(
			char *generic_load_upload_label )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_open,
				(char *)0 /* element_name */ ) ) );

	/* Create the filename prompt */
	/* -------------------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				non_edit_text,
				(char *)0 /* element_name */ ) ) );

	element->non_edit_text->message = "Filename";

	list_set(
		element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				upload,
				(char *)0 /* element_name */ ) ) );

	free( element->upload );

	element->upload =
		element_upload_new(
			generic_load_upload_label /* attribute_name */,
			-1 /* tab_order */,
			1 /* recall */ );

	element->upload->html =
		element_upload_insert_html(
			generic_load_upload_label /* element_name */,
			-1 /* tab_order */ );

	return element_list;
}

LIST *generic_load_skip_header_rows_element_list(
			char *generic_load_skip_header_rows )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	/* Create skip header rows */
	/* ----------------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				non_edit_text,
				(char *)0 /* element_name */ ) ) );

	element->non_edit_text->message = "Skip Header Rows";

	list_set(
		element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				text,
				(char *)0 /* element_name */ ) ) );

	element->text->attribute_name = generic_load_skip_header_rows;
	element->text->attribute_width_max_length = 3;

	list_set(
		element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				non_edit_text,
				(char *)0 /* element_name */ ) ) );

	element->non_edit_text->message = "How many header rows to skip?";

	return element_list;
}

LIST *generic_load_execute_checkbox_element_list( void )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				checkbox,
				(char *)0 /* element_name */ ) ) );

	element->checkbox->prompt_string = "Execute?";
	element->checkbox->element_name = "execute_yn";

	list_set(
		element_list,
		( element = appaserver_element_new(
				table_close,
				(char *)0 /* element_name */ ) ) );

	return element_list;
}

LIST *generic_load_dialog_box_element_list( void )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	list_set(
		element_list,
		( element = appaserver_element_new(
				line_break,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_open,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_heading,
				(char *)0 /* element_name */ ) ) );

	element->table_heading->heading_string = "Attribute";

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_heading,
				(char *)0 /* element_name */ ) ) );

	element->table_heading->heading_string = "Position";

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_heading,
				(char *)0 /* element_name */ ) ) );

	element->table_heading->heading_string = "Ignore";

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_heading,
				(char *)0 /* element_name */ ) ) );

	element->table_heading->heading_string = "Constant";

	return element_list;
}

GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST *
	generic_load_attribute_element_list_new(
			LIST *relation_mto1_non_isa_list,
			FOLDER_ATTRIBUTE *folder_attribute,
			char *login_name,
			int position )
{
	GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST *
		generic_load_attribute_element_list =
			generic_load_attribute_element_list_calloc();

	generic_load_attribute_element_list->element_list = list_new();

	list_set(
		generic_load_attribute_element_list->element_list,
		appaserver_element_new(
			table_row,
			(char *)0 ) );

	list_set(
		generic_load_attribute_element_list->element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	list_set(
		generic_load_attribute_element_list->element_list,
		generic_load_prompt_element(
			folder_attribute->attribute_name,
			folder_attribute->primary_key_index,
			folder_attribute->attribute->hint_message ) );

	list_set(
		generic_load_attribute_element_list->element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	list_set(
		generic_load_attribute_element_list->element_list,
		generic_load_position_element(
			folder_attribute->attribute_name,
			position,
			GENERIC_LOAD_POSITION_PREFIX ) );

	list_set(
		generic_load_attribute_element_list->element_list,
		appaserver_element_new(
			table_data,
			(char *)0 ) );

	if ( ( generic_load_attribute_element_list->
		row_security_relation =
			row_security_relation_new(
				folder_attribute->attribute_name,
				relation_mto1_non_isa_list,
				(char *)0 /* post_change_javascript */,
				(DICTIONARY *)0 /* drillthru_dictionary */,
				login_name,
				(char *)0 /* security_entity_where */,
				0 /* not viewonly */,
				(LIST *)0 /* row_security_relation_list */ ) )
	&& ( list_length(
		generic_load_attribute_element_list->
			row_security_relation->
			relation->
			foreign_key_list ) == 1 ) )
	{
		list_set_list(
			generic_load_attribute_element_list->element_list,
			generic_load_attribute_element_list->
				row_security_relation->
				element_list );
	}
	else
	{
		list_set(
			generic_load_attribute_element_list->element_list,
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) );

		list_set(
			generic_load_attribute_element_list->element_list,
			generic_load_constant_element(
				folder_attribute->attribute_name,
				folder_attribute->
					attribute->
					datatype_name,
				folder_attribute->
					attribute->
					width ) );
	}

	if ( !folder_attribute->primary_key_index )
	{
		list_set(
			generic_load_attribute_element_list->element_list,
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) );

		list_set(
			generic_load_attribute_element_list->element_list,
			generic_load_ignore_element(
				folder_attribute->attribute_name,
				GENERIC_LOAD_POSITION_PREFIX,
				GENERIC_LOAD_IGNORE_PREFIX ) );
	}

	return generic_load_attribute_element_list;
}

GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST *
	generic_load_attribute_element_list_calloc(
			void )
{
	GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST *
		generic_load_attribute_element_list;

	if ( ! ( generic_load_attribute_element_list =
		   calloc( 1,
			   sizeof( GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_attribute_element_list;
}

APPASERVER_ELEMENT *generic_load_prompt_element(
			char *attribute_name,
			int primary_key_index,
			char *hint_message )
{
	char prompt[ 256 ];
	char buffer[ 128 ];
	char *ptr = prompt;
	APPASERVER_ELEMENT *element;

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Set the attribute label */
	/* ----------------------- */
	if ( primary_key_index )
	{
		ptr += sprintf( ptr, "*" );
	}

	ptr += sprintf(
		ptr,
		"%s",
		format_initial_capital(
			buffer,
			attribute_name ) );

	if ( hint_message && *hint_message )
	{
		ptr += sprintf(
			ptr,
			"(%s)",
			hint_message );
	}

	element = appaserver_element_new( non_edit_text, (char *)0 );
	element->non_edit_text->message = strdup( prompt );

	return element;
}

APPASERVER_ELEMENT *generic_load_position_element(
			char *attribute_name,
			int position,
			char *position_prefix )
{
	char element_name[ 256 ];
	char buffer[ 16 ];
	APPASERVER_ELEMENT *element;

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element =
		appaserver_element_new(
			text,
			(char *)0 /* element_name */ );

	free( element->text );

	sprintf(element_name,
		"%s%s",
		position_prefix,
		attribute_name );

	element->text =
		element_text_new(
			strdup( element_name ) /* attribute_name */,
			(char *)0 /* datatype_name */,
			3 /* attribute_width_max_length */,
			0 /* not null_to_slash */,
			0 /* not prevent_carrot */,
			(char *)0 /* on_change */,
			(char *)0 /* on_focus */,
			(char *)0 /* on_keyup */,
			-1 /* tab_order */,
			1 /* recall */ );

	sprintf( buffer, "%d", position );
	element->text->value = strdup( buffer );

	return element;
}

APPASERVER_ELEMENT *generic_load_constant_element(
			char *attribute_name,
			char *datatype_name,
			int width )
{
	char post_change_javascript[ 128 ];
	APPASERVER_ELEMENT *element;

	if ( attribute_is_time( datatype_name ) )
	{
		strcpy(	post_change_javascript,
			"validate_time_insert(this)" );
	}
	else
	{
		*post_change_javascript = '\0';
	}

	element = appaserver_element_new( text, (char *)0 );
	free( element->text );

	element->text =
		element_text_new(
			attribute_name,
			datatype_name,
			width,
			0 /* not null_to_slash */,
			0 /* not prevent_carrot */,
			strdup( post_change_javascript ) /* on_change */,
			(char *)0 /* on_focus */,
			(char *)0 /* on_keyup */,
			-1 /* tab_order */,
			1 /* recall */ );

	return element;
}

APPASERVER_ELEMENT *generic_load_ignore_element(
			char *attribute_name,
			char *position_prefix,
			char *ignore_prefix )
{
	APPASERVER_ELEMENT *element;
	char element_name[ 128 ];
	char post_change_javascript[ 128 ];

	element =
		appaserver_element_new(
			checkbox,
			(char *)0 );

	free( element->checkbox );

	sprintf(post_change_javascript,
		"after_ignore(this, '%s%s','%s')",
		position_prefix,
		attribute_name,
		attribute_name );

	sprintf(element_name,
		"%s%s",
		ignore_prefix,
		attribute_name );

	element->checkbox =
		element_checkbox_new(
			(char *)0 /* attribute_name */,
			strdup( element_name ),
			"Ignore" /* prompt_string */,
			strdup( post_change_javascript ) /* on_click */,
			-1 /* tab_order */,
			(char *)0 /* image_source */,
			1 /* recall */ );

	return element;
}

GENERIC_LOAD_FOLDER *generic_load_folder_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			boolean menu_boolean,
			boolean frameset_menu_horizontal )
{
	GENERIC_LOAD_FOLDER *generic_load_folder =
		generic_load_folder_calloc();

	generic_load_folder->folder =
		folder_fetch(
			folder_name,
			role_name,
			(LIST *)0 /* exclude_lookup_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_relation_mto1_non_isa_list */,
			0 /* not fetch_relation_mto1_isa_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_one2m_recursive_list */,
			0 /* not fetch_process */,
			1 /* fetch_role_folder_list */,
			0 /* not fetch_row_level_restriction */,
			0 /* not fetch_role_operation_list */ );

	if ( generic_load_folder_forbid(
		role_folder_insert(
			generic_load_folder->folder->role_folder_list ) ) )
	{
		return (GENERIC_LOAD_FOLDER *)0;
	}

	if ( menu_boolean )
	{
		generic_load_folder->folder_menu =
			folder_menu_new(
				application_name,
				session_key,
				role_name,
				appaserver_parameter_data_directory() );

		generic_load_folder->menu =
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				frameset_menu_horizontal,
				generic_load_folder->
					folder_menu->
					count_list );
	}

	generic_load_folder->post_action_string =
		generic_load_folder_post_action_string(
			GENERIC_LOAD_FOLDER_POST_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	generic_load_folder->prompt_html =
		generic_load_folder_prompt_html(
			folder_name );

	generic_load_folder->title_string =
		generic_load_folder_title_string(
			folder_name );

	generic_load_folder->document =
		document_new(
			application_name,
			generic_load_folder->title_string,
			(char *)0 /* title_html */,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			menu_boolean,
			generic_load_folder->menu,
			document_head_menu_setup_string( 
				menu_boolean ),
			(char *)0 /* calendar_setup_string */,
			(char *)0 /* javascript_include_string */,
			(char *)0 /* input_onload_string */ );

	generic_load_folder->generic_load_folder_form =
		generic_load_folder_form_new(
			generic_load_folder->prompt_html,
			generic_load_folder->
				folder->
				folder_attribute_list,
			generic_load_folder->
				folder->
				relation_mto1_non_isa_list,
			generic_load_folder->post_action_string,
			login_name );

	generic_load_folder->html =
		generic_load_folder_html(
			generic_load_folder->document->html,
			generic_load_folder->document->document_head->html,
			document_head_close_html(),
			generic_load_folder->document->document_body->html,
			generic_load_folder->generic_load_folder_form->html,
			document_body_close_html(),
			document_close_html() );

	free( generic_load_folder->generic_load_folder_form->html );

	return generic_load_folder;
}

char *generic_load_folder_post_action_string(
			char *folder_post_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name )
{
	char post_action_string[ 1024 ];

	if ( !folder_post_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(post_action_string,
		"action=\"%s/%s?%s+%s+%s+%s+%s\"",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		folder_post_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name );

	return strdup( post_action_string );
}

char *generic_load_folder_prompt_html( char *folder_name )
{
	static char html[ 128 ];
	char buffer[ 64 ];

	sprintf(html,
		"<h3>Load %s</h3>",
		string_initial_capital(
			buffer,
			folder_name ) );
	return html;
}

char *generic_load_folder_title_string( char *folder_name )
{
	static char title_string[ 128 ];
	char buffer[ 64 ];

	sprintf(title_string,
		"Load %s",
		string_initial_capital(
			buffer,
			folder_name ) );
	return title_string;
}

char *generic_load_folder_html(
			char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *document_body_html,
			char *generic_load_folder_form_html,
			char *document_body_close_html,
			char *document_close_html )
{
	char html[ STRING_64K ];

	if ( !document_html
	||   !document_head_html
	||   !document_head_close_html
	||   !document_body_html
	||   !generic_load_folder_form_html
	||   !document_body_close_html
	||   !document_close_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s\n%s\n%s\n%s",
		document_html,
		document_head_html,
		document_head_close_html,
		document_body_html,
		generic_load_folder_form_html,
		document_body_close_html,
		document_close_html );

	return strdup( html );
}

GENERIC_LOAD_FOLDER *generic_load_folder_calloc( void )
{
	GENERIC_LOAD_FOLDER *generic_load_folder;

	if ( ! ( generic_load_folder =
			calloc( 1, sizeof( GENERIC_LOAD_FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_folder;
}

LIST *generic_load_attribute_list(
			DICTIONARY *working_post_dictionary,
			LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *list;

	if ( !list_rewind( folder_attribute_list ) )
		return (LIST *)0;

	list = list_new();

	do {
		folder_attribute = list_get( folder_attribute_list );

		list_set(
			list,
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			generic_load_attribute_new(
				working_post_dictionary,
				folder_attribute ) );

	} while ( list_next( folder_attribute_list ) );

	return list;
}

GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_new(
			DICTIONARY *working_post_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute )
{
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute =
		generic_load_attribute_calloc();

	generic_load_attribute->folder_attribute = folder_attribute;

	generic_load_attribute->constant_data =
		generic_load_attribute_constant_data(
			working_post_dictionary,
			folder_attribute->attribute_name );

	generic_load_attribute->position =
		generic_load_attribute_position(
			working_post_dictionary,
			folder_attribute->attribute_name,
			GENERIC_LOAD_POSITION_PREFIX );

	generic_load_attribute->ignore =
		generic_load_attribute_ignore(
			working_post_dictionary,
			folder_attribute->attribute_name,
			GENERIC_LOAD_IGNORE_PREFIX );

	return generic_load_attribute;
}

char *generic_load_attribute_constant_data(
			DICTIONARY *working_post_dictionary,
			char *attribute_name )
{
	return
	dictionary_get(
		attribute_name,
		working_post_dictionary );
}

int generic_load_attribute_position(
			DICTIONARY *working_post_dictionary,
			char *attribute_name,
			char *generic_load_position_prefix )
{
	char key[ 128 ];
	char *results;

	sprintf(key,
		"%s%s",
		generic_load_position_prefix,
		attribute_name );

	results = dictionary_get( key, working_post_dictionary );

	if ( results )
		return atoi( results );
	else
		return 0;
}

boolean generic_load_attribute_ignore(
			DICTIONARY *working_post_dictionary,
			char *attribute_name,
			char *generic_load_ignore_prefix )
{
	char key[ 128 ];
	char *results;

	sprintf(key,
		"%s%s",
		generic_load_ignore_prefix,
		attribute_name );

	results = dictionary_get( key, working_post_dictionary );

	if ( results && *results == 'y' )
		return 1;
	else
		return 0;
}

RELATION *generic_load_attribute_consumes_relation(
			char *attribute_name,
			LIST *relation_mto1_non_isa_list )
{
	return relation_consumes(
			attribute_name,
			relation_mto1_non_isa_list );
}

GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_calloc( void )
{
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;

	if ( ! ( generic_load_attribute =
			calloc( 1, sizeof( GENERIC_LOAD_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_attribute;
}

LIST *generic_load_attribute_data_list(
			LIST *generic_load_attribute_list,
			char *input_line )
{
	LIST *list;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;
	GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data;

	if ( !list_rewind( generic_load_attribute_list ) ) return (LIST *)0;

	list = list_new();

	do {
		generic_load_attribute =
			list_get(
				generic_load_attribute_list );

		if ( !generic_load_attribute->folder_attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		generic_load_attribute_data =
			generic_load_attribute_data_new(
				generic_load_attribute->
					folder_attribute->
					attribute_name,
				generic_load_attribute->constant_data,
				generic_load_attribute->position,
				generic_load_attribute->ignore,
				input_line );

		if ( generic_load_attribute_data )
		{
			list_set( list, generic_load_attribute_data );
		}

	} while ( list_next( generic_load_attribute_list ) );

	return list;
}

GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data_new(
			char *attribute_name,
			char *constant_data,
			int position,
			boolean ignore,
			char *input_line )
{
	GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data =
		generic_load_attribute_data_calloc();

	generic_load_attribute_data->attribute_name = attribute_name;

	if ( ! ( generic_load_attribute_data->string =
			generic_load_attribute_data_string(
				constant_data,
				position,
				ignore,
				input_line ) ) )
	{
		free( generic_load_attribute_data );
		return (GENERIC_LOAD_ATTRIBUTE_DATA *)0;
	}

	generic_load_attribute_data->
		security_sql_injection_escape_quote_delimit =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			security_sql_injection_escape_quote_delimit(
				generic_load_attribute_data->string );

	return generic_load_attribute_data;
}

char *generic_load_attribute_data_string(
			char *constant_data,
			int position,
			boolean ignored,
			char *input_line )
{
	char *data = {0};

	if ( ignored )
	{
		return (char *)0;
	}
	else
	if ( constant_data && *constant_data )
	{
		data = constant_data;
	}
	else
	{
		char buffer[ 1024 ];

		if ( piece( buffer, SQL_DELIMITER, input_line, position - 1 ) )
		{
			data = strdup( buffer );
		}
	}

	return data;
}

LIST *generic_load_attribute_data_name_list(
			LIST *generic_load_attribute_data_list )
{
	GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data;
	LIST *list;

	if ( !list_rewind( generic_load_attribute_data_list ) )
	{
		return (LIST *)0;
	}

	list = list_new();

	do {
		generic_load_attribute_data =
			list_get(
				generic_load_attribute_data_list );

		list_set(
			list,
			generic_load_attribute_data->attribute_name );

	} while ( list_next( generic_load_attribute_data_list ) );

	return list;
}

LIST *generic_load_attribute_data_escape_list(
			LIST *generic_load_attribute_data_list )
{
	GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data;
	LIST *list;

	if ( !list_rewind( generic_load_attribute_data_list ) )
	{
		return (LIST *)0;
	}

	list = list_new();

	do {
		generic_load_attribute_data =
			list_get(
				generic_load_attribute_data_list );

		list_set(
			list,
			generic_load_attribute_data->
				security_sql_injection_escape_quote_delimit );

	} while ( list_next( generic_load_attribute_data_list ) );

	return list;
}

GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data_seek(
			char *attribute_name,
			LIST *generic_load_attribute_data_list )
{
	GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data;

	if ( ! list_rewind( generic_load_attribute_data_list ) )
	{
		return (GENERIC_LOAD_ATTRIBUTE_DATA *)0;
	}

	do {
		generic_load_attribute_data =
			list_get(
				generic_load_attribute_data_list );

		if ( string_strcmp(
			attribute_name,
			generic_load_attribute_data->attribute_name ) == 0 )
		{
			return generic_load_attribute_data;
		}

	} while ( list_next( generic_load_attribute_data_list ) );

	return (GENERIC_LOAD_ATTRIBUTE_DATA *)0;
}

GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data_calloc( void )
{
	GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data;

	if ( ! ( generic_load_attribute_data =
			calloc( 1, sizeof( GENERIC_LOAD_ATTRIBUTE_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_attribute_data;
}

LIST *generic_load_sql_statement_list(
			char *folder_table_name,
			LIST *generic_load_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *post_filename,
			boolean replace_existing,
			int skip_header_rows )
{
	LIST *list = {0};
	FILE *input_file;
	char input_line[ 2048 ];
	int line_count = 0;
	GENERIC_LOAD_SQL_STATEMENT *generic_load_sql_statement;

	if ( ! ( input_file = fopen( post_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			post_filename );
		exit( 1 );
	}

	while ( string_input( input_line, input_file, 2048 ) )
	{
		if ( skip_header_rows > 0 )
		{
			if ( ++line_count < skip_header_rows ) continue;
		}

		generic_load_sql_statement =
			generic_load_sql_statement_new(
				folder_table_name,
				generic_load_attribute_list,
				relation_mto1_non_isa_list,
				replace_existing,
				input_line );

		if ( generic_load_sql_statement )
		{
			if ( !list ) list = list_new();

			list_set( list, generic_load_sql_statement );
		}
	}

	fclose( input_file );

	return list;
}

GENERIC_LOAD_SQL_STATEMENT *generic_load_sql_statement_new(
			char *folder_table_name,
			LIST *generic_load_attribute_list,
			LIST *relation_mto1_non_isa_list,
			boolean replace_existing,
			char *input_line )
{
	GENERIC_LOAD_SQL_STATEMENT *generic_load_sql_statement =
		generic_load_sql_statement_calloc();

	generic_load_sql_statement->generic_load_attribute_data_list =
		generic_load_attribute_data_list(
			generic_load_attribute_list,
			input_line );

	if ( !list_length( 
		generic_load_sql_statement->
			generic_load_attribute_data_list ) )
	{
		free( generic_load_sql_statement );
		return (GENERIC_LOAD_SQL_STATEMENT *)0;
	}

	if ( list_length( relation_mto1_non_isa_list ) )
	{
		generic_load_sql_statement->generic_load_relation =
			generic_load_relation_new(
				generic_load_sql_statement->
					generic_load_attribute_data_list,
				relation_mto1_non_isa_list );
	}

	if ( !generic_load_sql_statement->generic_load_relation
	||   !generic_load_sql_statement->generic_load_relation->orphan )
	{
		generic_load_sql_statement->string =
			generic_load_sql_statement_string(
				folder_table_name,
				generic_load_sql_statement->
					generic_load_attribute_data_list,
				replace_existing );
	}
			
	return generic_load_sql_statement;
}

char *generic_load_sql_statement_string(
			char *folder_table_name,
			LIST *generic_load_attribute_data_list,
			boolean replace_existing )
{
	char string[ 65536 ];
	char *ptr = string;

	if ( !folder_table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_table_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( generic_load_attribute_data_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list_rewind() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( replace_existing )
		ptr += sprintf( ptr, "replace into %s", folder_table_name );
	else
		ptr += sprintf( ptr, "insert into %s", folder_table_name );

	ptr += sprintf(
		ptr,
		" (%s) values",
		list_display_delimited(
			generic_load_attribute_data_name_list(
				generic_load_attribute_data_list ),
			',' ) );

	ptr += sprintf(
		ptr,
		" (%s);",
		list_display_delimited(
			generic_load_attribute_data_escape_list(
				generic_load_attribute_data_list ),
			',' ) );

	return strdup( string );
}

GENERIC_LOAD_SQL_STATEMENT *generic_load_sql_statement_calloc( void )
{
	GENERIC_LOAD_SQL_STATEMENT *generic_load_sql_statement;

	if ( ! ( generic_load_sql_statement =
			calloc( 1, sizeof( GENERIC_LOAD_SQL_STATEMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_sql_statement;
}

GENERIC_LOAD_RELATION *generic_load_relation_new(
			LIST *attribute_data_list,
			LIST *relation_mto1_non_isa_list )
{
	RELATION *relation;
	GENERIC_LOAD_RELATION *generic_load_relation;

	if ( !list_rewind( relation_mto1_non_isa_list ) )
		return (GENERIC_LOAD_RELATION *)0;

	generic_load_relation = generic_load_relation_calloc();

	do {
		relation = list_get( relation_mto1_non_isa_list );

		if ( !relation->one_folder )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: relation->one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !list_length( relation->foreign_key_list ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		generic_load_relation->foreign_data_delimited =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			generic_load_relation_foreign_data_delimited(
				attribute_data_list,
				relation->foreign_key_list );

		if ( generic_load_relation->foreign_data_delimited )
		{
			if ( ( generic_load_relation->orphan =
				generic_load_relation_orphan(
					generic_load_relation->
						foreign_data_delimited,
					relation->
						one_folder->
						delimited_list ) ) )
			{
				return generic_load_relation;
			}

			free( generic_load_relation->foreign_data_delimited );
		}

	} while ( list_next( relation_mto1_non_isa_list ) );

	return generic_load_relation;
}

char *generic_load_relation_foreign_data_delimited(
			LIST *attribute_data_list,
			LIST *foreign_key_list )
{
	GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data;
	char *foreign_key;
	char foreign_data_delimited[ 1024 ];
	char *ptr = foreign_data_delimited;

	if ( !list_rewind( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list_rewind() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		foreign_key = list_get( foreign_key_list );

		if ( ! ( generic_load_attribute_data =
				generic_load_attribute_data_seek(
					foreign_key /* attribute_name */,
					attribute_data_list ) ) )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: generic_load_attribute_data_seek(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				foreign_key );
			exit( 1 );
		}

		if ( generic_load_attribute_data->string )
		{
			if ( ptr != foreign_data_delimited )
			{
				ptr += sprintf( ptr, "%c", SQL_DELIMITER );
			}

			ptr += sprintf(
				ptr,
				"%s",
				generic_load_attribute_data->string );
		}

	} while ( list_next( foreign_key_list ) );

	if ( ptr == foreign_data_delimited )
		return (char *)0;
	else
		return strdup( foreign_data_delimited );
}

boolean generic_load_relation_orphan(
			char *foreign_data_delimited,
			LIST *delimited_list )
{
	return
	list_string_exists(
		foreign_data_delimited,
		delimited_list );
}

GENERIC_LOAD_RELATION *generic_load_relation_calloc( void )
{
	GENERIC_LOAD_RELATION *generic_load_relation;

	if ( ! ( generic_load_relation =
			calloc( 1, sizeof( GENERIC_LOAD_RELATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_relation;
}

char *generic_load_folder_element_list_html(
			LIST *element_list )
{
	if ( !list_length( element_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	appaserver_element_list_html( element_list /* in/out */ );
}

boolean generic_load_folder_forbid(
			boolean role_folder_insert )
{
	return 1 - role_folder_insert;
}

GENERIC_LOAD_FOLDER_POST *generic_load_folder_post_new(
			char *application_name,
			char *folder_name,
			char *role_name,
			DICTIONARY *working_post_dictionary )
{
	GENERIC_LOAD_FOLDER_POST *generic_load_folder_post =
		generic_load_folder_post_calloc();

	generic_load_folder_post->folder =
		folder_fetch(
			folder_name,
			role_name,
			(LIST *)0 /* exclude_lookup_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_relation_mto1_non_isa_list */,
			0 /* not fetch_relation_mto1_isa_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_one2m_recursive_list */,
			0 /* not fetch_process */,
			1 /* fetch_role_folder_list */,
			0 /* not fetch_row_level_restriction */,
			0 /* not fetch_role_operation_list */ );

	generic_load_folder_post->generic_load_folder_forbid =
		generic_load_folder_forbid(
			role_folder_insert(
				generic_load_folder_post->
					folder->
					role_folder_list ) );

	if ( generic_load_folder_post->generic_load_folder_forbid )
	{
		free( generic_load_folder_post );
		return (GENERIC_LOAD_FOLDER_POST *)0;
	}

	relation_set_one_folder_primary_delimited_list(
		generic_load_folder_post->
			folder->
			relation_mto1_non_isa_list );

	generic_load_folder_post->generic_load_attribute_list =
		generic_load_attribute_list(
			working_post_dictionary,
			generic_load_folder_post->
				folder->
				folder_attribute_list );

	generic_load_folder_post->filename =
		generic_load_folder_post_filename(
		GENERIC_LOAD_UPLOAD_LABEL,
		working_post_dictionary );

	generic_load_folder_post->replace_existing =
		generic_load_folder_post_replace_existing(
		GENERIC_LOAD_REPLACE_EXISTING_YN,
		working_post_dictionary );

	generic_load_folder_post->execute =
		generic_load_folder_post_execute(
		GENERIC_LOAD_EXECUTE_YN,
		working_post_dictionary );

	generic_load_folder_post->skip_header_rows =
		generic_load_folder_post_skip_header_rows(
		GENERIC_LOAD_SKIP_HEADER_ROWS,
		working_post_dictionary );

	generic_load_folder_post->generic_load_sql_statement_list =
		generic_load_sql_statement_list(
			folder_table_name(
				application_name,
				folder_name ),
			generic_load_folder_post->
				generic_load_attribute_list,
			generic_load_folder_post->
				folder->
				relation_mto1_non_isa_list,
			generic_load_folder_post->filename,
			generic_load_folder_post->replace_existing,
			generic_load_folder_post->skip_header_rows );

	return generic_load_folder_post;
}

char *generic_load_folder_post_filename(
			char *generic_load_upload_label,
			DICTIONARY *working_post_dictionary )
{
	return
	dictionary_get(
		generic_load_upload_label,
		working_post_dictionary );
}

boolean generic_load_folder_post_replace_existing(
			char *generic_load_replace_existing_yn,
			DICTIONARY *working_post_dictionary )
{
	char *results;

	results =
		dictionary_get(
			generic_load_replace_existing_yn,
			working_post_dictionary );

	if ( results && *results == 'y' )
		return 1;
	else
		return 0;
}

boolean generic_load_folder_post_execute(
			char *generic_load_execute_yn,
			DICTIONARY *working_post_dictionary )
{
	char *results;

	results =
		dictionary_get(
			generic_load_execute_yn,
			working_post_dictionary );

	if ( results && *results == 'y' )
		return 1;
	else
		return 0;
}

int generic_load_folder_post_skip_header_rows(
			char *generic_load_skip_header_rows,
			DICTIONARY *working_post_dictionary )
{
	char *results;

	results =
		dictionary_get(
			generic_load_skip_header_rows,
			working_post_dictionary );

	if ( results )
		return atoi( results );
	else
		return 0;
}

GENERIC_LOAD_FOLDER_POST *generic_load_folder_post_calloc( void )
{
	GENERIC_LOAD_FOLDER_POST *generic_load_folder_post;

	if ( ! ( generic_load_folder_post =
			calloc( 1, sizeof( GENERIC_LOAD_FOLDER_POST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_folder_post;
}

