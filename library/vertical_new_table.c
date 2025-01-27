/* -------------------------------------------------------------------- */
/* APPASERVER_HOME/library/vertical_new_table.c				*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new checkboxes on the prompt insert form.	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "application.h"
#include "appaserver.h"
#include "dictionary.h"
#include "appaserver_error.h"
#include "vertical_new_table.h"

VERTICAL_NEW_TABLE *vertical_new_table_new(
		const char *vertical_new_prompt_one_hidden_label,
		const char *vertical_new_prompt_many_hidden_label,
		const char *frameset_prompt_frame,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *data_directory,
		DICTIONARY *non_prefixed_dictionary,
		boolean menu_horizontal_boolean )
{
	VERTICAL_NEW_TABLE *vertical_new_table;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	vertical_new_table = vertical_new_table_calloc();

	if ( ! ( vertical_new_table->one_folder_name =
		   /* ---------------------------------------------------- */
		   /* Returns component of non_prefixed_dictionary or null */
		   /* ---------------------------------------------------- */
		   vertical_new_table_one_folder_name(
			vertical_new_prompt_one_hidden_label,
			non_prefixed_dictionary ) ) )
	{
		free( vertical_new_table );
		return NULL;
	}

	if ( ! ( vertical_new_table->many_folder_name =
		   /* ---------------------------------------------------- */
		   /* Returns component of non_prefixed_dictionary or null */
		   /* ---------------------------------------------------- */
		   vertical_new_table_many_folder_name(
			vertical_new_prompt_many_hidden_label,
			non_prefixed_dictionary ) ) )
	{
		char message[ 128 ];

		sprintf(message,
		"vertical_new_table_many_folder_name() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	vertical_new_table->
		vertical_new_filename =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			vertical_new_filename_new(
				application_name,
				session_key,
				data_directory,
				"vertical_table"
					/* filename_stem */ );

	vertical_new_table->onload_javascript_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		vertical_new_table_onload_javascript_string(
			frameset_prompt_frame,
			vertical_new_table->
				vertical_new_filename->
				prompt_filename );

	if ( menu_horizontal_boolean )
	{
		if ( ! ( vertical_new_table->folder_menu =
				folder_menu_fetch(
					application_name,
					session_key,
					role_name,
					data_directory,
					0 /* not folder_menu_remove */ ) ) )
		{
			char message[ 128 ];

			sprintf(message, "folder_menu_fetch() empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		vertical_new_table->menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				vertical_new_table->
					folder_menu->
					count_list );
	}

	vertical_new_table->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			application_title_string( application_name ),
			(char *)0 /* title_string */,
			(char *)0 /* sub_title_string */,
			(char *)0 /* sub_subtitle_string */,
			(char *)0 /* notepad */,
			(char *)0 /* onload_javascript_string */,
			(vertical_new_table->menu)
				? vertical_new_table->menu->html
				: (char *)0,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_menu_setup_string(
				(vertical_new_table->menu)
					? 1
					: 0 /* menu_boolean */ ),
			(char *)0 /* calendar_setup_string */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			javascript_include_string(
				application_name,
				(LIST *)0 /* javascript_filename_list */ ) );

	return vertical_new_table;
}

void vertical_new_table_blank_prompt_frame(
		char *output_filename,
		DOCUMENT *document )
{
	FILE *output_file;

	if ( !output_filename
	||   !document
	||   !document->html
	||   !document->document_head
	||   !document->document_body )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Safely returns */
	/* -------------- */
	output_file = appaserver_output_file( output_filename );

	fprintf(output_file,
		"%s\n%s\n%s\n%s\n%s\n%s\n",
		document->html,
		document->document_head->html,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_close_tag(),
		document->document_body->html,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_body_close_tag(),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_close_tag() );

	fclose( output_file );
}

char *vertical_new_table_one_folder_name(
		const char *vertical_new_prompt_one_hidden_label,
		DICTIONARY *non_prefixed_dictionary )
{
	return
	dictionary_get(
		(char *)vertical_new_prompt_one_hidden_label,
		non_prefixed_dictionary );
}

char *vertical_new_table_many_folder_name(
		const char *vertical_new_prompt_many_hidden_label,
		DICTIONARY *non_prefixed_dictionary )
{
	return
	dictionary_get(
		(char *)vertical_new_prompt_many_hidden_label,
		non_prefixed_dictionary );
}

VERTICAL_NEW_TABLE *vertical_new_table_calloc( void )
{
	VERTICAL_NEW_TABLE *vertical_new_table;

	if ( ! ( vertical_new_table =
			calloc( 1,
				sizeof ( VERTICAL_NEW_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return vertical_new_table;
}

char *vertical_new_table_onload_javascript_string(
		const char *frameset_prompt_frame,
		char *prompt_filename )
{
	static char javascript_string[ 128 ];

	if ( !prompt_filename )
	{
		char message[ 128 ];

		sprintf(message, "prompt_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(javascript_string,
		"window.open( '%s', '%s' )",
		prompt_filename,
		frameset_prompt_frame );

	return javascript_string;
}

boolean vertical_new_table_participating_boolean(
		const char *vertical_new_prompt_one_hidden_label,
		DICTIONARY *non_prefixed_dictionary )
{
	return
	dictionary_key_exists(
		non_prefixed_dictionary,
		(char *)vertical_new_prompt_one_hidden_label
			/* key */ );
}
