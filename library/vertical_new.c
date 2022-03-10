/* APPASERVER_HOME/library/vertical_new.c				*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new buttons on the prompt insert form.	*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include "timlib.h"
#include "String.h"
#include "element.h"
#include "application.h"
#include "dictionary.h"
#include "frameset.h"
#include "folder_menu.h"
#include "document.h"
#include "element.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "dictionary_separate.h"
#include "vertical_new.h"

char *vertical_new_post_prompt_insert_one_folder_name(
			char *vertical_new_one_prefix,
			DICTIONARY *non_prefixed_dictionary )
{
	LIST *key_list;
	char *key;
	char *data;
	char return_key[ 128 ];

	if ( !non_prefixed_dictionary ) return (char *)0;

	key_list =
		dictionary_extract_prefixed_key_list(
			non_prefixed_dictionary,
			vertical_new_one_prefix );

	if ( !list_rewind( key_list ) ) return (char *)0;

	do {
		key = list_get( key_list );

		data =
			dictionary_get(
				key,
				non_prefixed_dictionary );

		if ( *data == 'y' )
		{
			return strdup(
				trim_index(
				return_key,
				key +
				strlen( vertical_new_one_prefix ) ) );
		}
	} while( list_next( key_list ) );

	return (char *)0;
}

char *vertical_new_output_prompt_insert_element_name(
			char *vertical_new_one_prefix,
			char *one_folder_name )
{
	char element_name[ 128 ];

	if ( !one_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( element_name,
		 "%s%s",
		 vertical_new_one_prefix,
		 one_folder_name );

	return strdup( element_name );
}

APPASERVER_ELEMENT *vertical_new_output_prompt_insert_element(
			char *one_folder_name,
			LIST *role_folder_insert_list,
			char *form_name,
			char *vertical_new_one_prefix,
			char *vertical_new_display,
			char *submit_javascript )
{
	APPASERVER_ELEMENT *element;

	if ( !one_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !form_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: form_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !submit_javascript )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: submit_javascript is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_exists_string(
		one_folder_name,
		role_folder_insert_list ) )
	{
		return (APPASERVER_ELEMENT *)0;
	}

	element =
		appaserver_element_new(
			checkbox, 
			(char *)0 /* element_name */ );

	free( element->checkbox );

	element->checkbox =
		element_checkbox_new(
			(char *)0 /* attribute_name */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			vertical_new_output_prompt_insert_element_name(
				vertical_new_one_prefix,
				one_folder_name ),
			vertical_new_display /* prompt_string */,
			submit_javascript /* on_click */,
			0 /* tab_order */,
			(char *)0 /* image_source */,
			0 /* not recall */ );

	return element;
}

char *vertical_new_dictionary_folder_name(
			char *hidden_label,
			DICTIONARY *non_prefixed_dictionary )
{
	return dictionary_get(
			hidden_label,
			non_prefixed_dictionary );
}

void vertical_new_dictionary_set(
			DICTIONARY *non_prefixed_dictionary,
			char *hidden_label,
			char *folder_name )
{
	dictionary_set_pointer(
		non_prefixed_dictionary,
		hidden_label,
		folder_name );
}

char *vertical_new_output_insert_table_onload_control_string(
			char *prompt_filename,
			char *frameset_prompt_frame )
{
	char onload_control_string[ 128 ];

	sprintf(onload_control_string,
		"window.open('%s','%s');",
		prompt_filename,
		frameset_prompt_frame );

	return strdup( onload_control_string );

}

VERTICAL_NEW_POST_PROMPT_INSERT *
	vertical_new_post_prompt_insert_new(
			DICTIONARY *non_prefixed_dictionary /* in/out */,
			char *many_folder_name,
			DICTIONARY *drillthru_dictionary,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *vertical_new_one_prefix,
			char *vertical_new_one_hidden_label,
			char *vertical_new_many_hidden_label,
			char *frameset_prompt_frame )
{
	VERTICAL_NEW_POST_PROMPT_INSERT *vertical_new_post_prompt_insert;
	char *one_folder_name;

	if ( ! ( one_folder_name =
			vertical_new_post_prompt_insert_one_folder_name(
				vertical_new_one_prefix,
				non_prefixed_dictionary ) ) )
	{
		return (VERTICAL_NEW_POST_PROMPT_INSERT *)0;
	}

	vertical_new_post_prompt_insert =
		vertical_new_post_prompt_insert_calloc();

	vertical_new_post_prompt_insert->one_folder_name = one_folder_name;

	vertical_new_dictionary_set(
		non_prefixed_dictionary,
		vertical_new_one_hidden_label,
		vertical_new_post_prompt_insert->one_folder_name );

	vertical_new_dictionary_set(
		non_prefixed_dictionary,
		vertical_new_many_hidden_label,
		many_folder_name );

	vertical_new_post_prompt_insert->system_string =
		vertical_new_post_prompt_insert_system_string(
			drillthru_dictionary,
			non_prefixed_dictionary,
			application_name,
			login_name,
			session_key,
			vertical_new_post_prompt_insert->one_folder_name,
			role_name,
			frameset_prompt_frame );

	return vertical_new_post_prompt_insert;
}

VERTICAL_NEW_OUTPUT_INSERT_TABLE *
	vertical_new_button_output_insert_table_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *document_root_directory,
			DICTIONARY *non_prefixed_dictionary,
			char *vertical_new_one_hidden_label,
			char *frameset_prompt_frame,
			char *data_directory )
{
	VERTICAL_NEW_OUTPUT_INSERT_TABLE *vertical_new_output_insert_table;
	char *one_folder_name;

	one_folder_name =
		vertical_new_dictionary_folder_name(
			vertical_new_one_hidden_label,
			non_prefixed_dictionary );

	if ( !one_folder_name )
	{
		return (VERTICAL_NEW_OUTPUT_INSERT_TABLE *)0;
	}

	if ( strcmp( folder_name, one_folder_name ) != 0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: strcmp(%s,%s) returned error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name,
			one_folder_name );
		exit( 1 );
	}

	vertical_new_output_insert_table =
		vertical_new_output_insert_table_calloc();

	vertical_new_output_insert_table->one_folder_name = one_folder_name;

	vertical_new_output_insert_table->vertical_new_blank_filename =
		/* ------------------- */
		/* Always returns true */
		/* ------------------- */
		vertical_new_blank_filename_new(
			application_name,
			session_key,
			document_root_directory );

	vertical_new_output_insert_table_blank_prompt_frame(
		vertical_new_output_insert_table->
			vertical_new_blank_filename->
			output_filename,
		application_name,
		session_key,
		login_name,
		role_name,
		frameset_prompt_frame,
		data_directory );

	vertical_new_output_insert_table->onload_control_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		vertical_new_output_insert_table_onload_control_string(
			vertical_new_output_insert_table->
				vertical_new_blank_filename->
				prompt_filename,
			frameset_prompt_frame );

	return vertical_new_output_insert_table;
}

VERTICAL_NEW_BLANK_FILENAME *vertical_new_blank_filename_calloc( void )
{
	VERTICAL_NEW_BLANK_FILENAME *vertical_new_blank_filename;

	if ( ! ( vertical_new_blank_filename =
			calloc(	1, sizeof( VERTICAL_NEW_BLANK_FILENAME ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return vertical_new_blank_filename;
}

VERTICAL_NEW_BLANK_FILENAME *vertical_new_blank_filename_new(
			char *application_name,
			char *session_key,
			char *document_root_directory )
{
	VERTICAL_NEW_BLANK_FILENAME *vertical_new_blank_filename =
		vertical_new_blank_filename_calloc();

	vertical_new_blank_filename->appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			"blank_screen" /* filename_stem */,
			application_name,
			0 /* process_id */,
			session_key,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"html" );

	if ( !vertical_new_blank_filename->appaserver_link )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: appaserver_link_new(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			document_root_directory );
		exit( 1 );
	}

	vertical_new_blank_filename->output_filename =
		vertical_new_blank_filename->appaserver_link->output->filename;

	vertical_new_blank_filename->prompt_filename =
		vertical_new_blank_filename->appaserver_link->prompt->filename;

	return vertical_new_blank_filename;
}

void vertical_new_output_insert_table_blank_prompt_frame(
			char *output_filename,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *frameset_prompt_frame,
			char *data_directory )
{
	DOCUMENT *document;
	FILE *output_file;
	boolean menu_b;
	MENU *menu = {0};

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			output_filename );
		exit( 1 );
	}

	if ( ( menu_b =
		menu_boolean(
			frameset_prompt_frame /* current_frame */,
			frameset_menu_horizontal(
				application_name,
				login_name ) ) ) )
	{
		FOLDER_MENU *folder_menu;

		if ( ! ( folder_menu =
				folder_menu_fetch(
					application_name,
					session_key,
					data_directory,
					role_name ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_menu_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		menu =
			menu_fetch(
				application_name,
				login_name,
				session_key,
				role_name,
				frameset_prompt_frame /* target_frame */,
				folder_menu->lookup_count_list );
	}

	document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_new(
			application_name,
			application_title_string( application_name ),
			(char *)0 /* title_html */,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			menu_b /* menu_boolean */,
			menu,
			document_head_menu_setup_string( menu_b ),
			(char *)0 /* calendar_setup_string */,
			(char *)0 /* javascript_include_string */,
			(char *)0 /* input_onload_string */ );

	fprintf(output_file,
		"%s\n%s\n%s\n%s\n%s\n%s\n",
		document->html,
		document->document_head->html,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_close_html(),
		document->document_body->html,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_body_close_html(),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_close_html() );

	fclose( output_file );
}

char *vertical_new_post_prompt_insert_system_string(
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			char *application_name,
			char *login_name,
			char *session_key,
			char *one_folder_name,
			char *role_name,
			char *frameset_prompt_frame )
{
	char system_string[ 65536 ];
	char *send_string;

	send_string =
		dictionary_separate_send_string(
			dictionary_separate_send_dictionary(
				(DICTIONARY *)0 /* sort_dictionary */,
				(DICTIONARY *)0 /* query_dictionary */,
				drillthru_dictionary,
				(DICTIONARY *)0 /* ignore_dictionary */,
				(DICTIONARY *)0 /* pair_one2m_dictionary */,
				non_prefixed_dictionary ) );

	sprintf(system_string,
		"echo \"%s\" |"
		"output_insert_table %s %s %s %s %s %s 2>>%s",
		send_string,
		application_name,
		login_name,
		session_key,
		one_folder_name,
		role_name,
		frameset_prompt_frame,
		appaserver_error_filename( application_name ) );

	return strdup( system_string );
}

VERTICAL_NEW_POST_INSERT_TABLE *vertical_new_post_insert_table_calloc( void )
{
	VERTICAL_NEW_POST_INSERT_TABLE *vertical_new_post_insert_table;

	if ( ! ( vertical_new_post_insert_table =
		    calloc( 1, sizeof( VERTICAL_NEW_POST_INSERT_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return vertical_new_post_insert_table;
}

VERTICAL_NEW_POST_INSERT_TABLE *vertical_new_post_insert_table_new(
			char *vertical_new_button_many_hidden_label,
			DICTIONARY *non_prefixed_dictionary )
{
	VERTICAL_NEW_POST_INSERT_TABLE *vertical_new_post_insert_table;

	vertical_new_post_insert_table =
		vertical_new_post_insert_table_calloc();

	return vertical_new_post_insert_table;
}

VERTICAL_NEW_OUTPUT_INSERT_TABLE *vertical_new_output_insert_table_calloc(
			void )
{
	VERTICAL_NEW_OUTPUT_INSERT_TABLE *vertical_new_output_insert_table;

	vertical_new_output_insert_table =
		calloc( 1, sizeof( VERTICAL_NEW_OUTPUT_INSERT_TABLE ) );

	if ( !vertical_new_output_insert_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return vertical_new_output_insert_table;
}

VERTICAL_NEW_POST_PROMPT_INSERT *vertical_new_post_prompt_insert_calloc(
			void )
{
	VERTICAL_NEW_POST_PROMPT_INSERT *vertical_new_post_prompt_insert;

	vertical_new_post_prompt_insert =
		calloc( 1, sizeof( VERTICAL_NEW_POST_PROMPT_INSERT ) );

	if ( !vertical_new_post_prompt_insert )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return vertical_new_post_prompt_insert;
}

