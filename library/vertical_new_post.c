/* -------------------------------------------------------------------- */
/* APPASERVER_HOME/library/vertical_new_post.c				*/
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
#include "appaserver_error.h"
#include "prompt_insert.h"
#include "vertical_new_post.h"

VERTICAL_NEW_POST *vertical_new_post_new(
		const char *vertical_new_prompt_many_hidden_label,
		const char *frameset_prompt_frame,
		const char *dictionary_separate_drillthru_prefix,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *non_prefixed_dictionary,
		char *data_directory )
{
	VERTICAL_NEW_POST *vertical_new_post;

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

	vertical_new_post = vertical_new_post_calloc();

	vertical_new_post->data_directory = data_directory;

	vertical_new_post->many_folder_name =
		vertical_new_post_many_folder_name(
			vertical_new_prompt_many_hidden_label,
			non_prefixed_dictionary );

	if ( !vertical_new_post->many_folder_name )
	{
		free( vertical_new_post );
		return NULL;
	}

	vertical_new_post->application_menu_horizontal_boolean =
		application_menu_horizontal_boolean(
			application_name );

	vertical_new_post->original_post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		vertical_new_post_original_dictionary(
			dictionary_separate_drillthru_prefix,
			drillthru_dictionary,
			non_prefixed_dictionary );

	vertical_new_post->vertical_new_filename =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		vertical_new_filename_new(
			application_name,
			session_key,
			data_directory,
			"vertical_post"
				/* filename_stem */ );

	vertical_new_post->onload_javascript_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		vertical_new_post_onload_javascript_string(
			frameset_prompt_frame,
			vertical_new_post->
				vertical_new_filename->
				prompt_filename );

	return vertical_new_post;
}

VERTICAL_NEW_POST *vertical_new_post_calloc( void )
{
	VERTICAL_NEW_POST *vertical_new_post;

	if ( ! ( vertical_new_post =
			calloc( 1,
				sizeof ( VERTICAL_NEW_POST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return vertical_new_post;
}

char *vertical_new_post_many_folder_name(
		const char *vertical_new_prompt_many_hidden_label,
		DICTIONARY *non_prefixed_dictionary )
{
	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		(char *)vertical_new_prompt_many_hidden_label,
		non_prefixed_dictionary );
}

DICTIONARY *vertical_new_post_original_dictionary(
		const char *dictionary_separate_drillthru_prefix,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *non_prefixed_dictionary )
{
	DICTIONARY *original_post_dictionary;

	original_post_dictionary = dictionary_medium();

	(void)dictionary_append(
		original_post_dictionary,
		non_prefixed_dictionary );

	if ( dictionary_length( drillthru_dictionary ) )
	{
		DICTIONARY *prefix =
			/* ----------------------------------- */
			/* Returns dictionary_medium() or null */
			/* ----------------------------------- */
			dictionary_prefix(
				drillthru_dictionary,
				(char *)dictionary_separate_drillthru_prefix );

		(void)dictionary_set_dictionary(
			original_post_dictionary /* source_destination */,
			prefix /* append_dictionary */ );
	}

	return original_post_dictionary;
}

char *vertical_new_post_onload_javascript_string(
		const char *frameset_prompt_frame,
		char *prompt_filename )
{
	static char onload_javascript_string[ 256 ];

	sprintf(onload_javascript_string,
		"window.open( '%s', '%s' )",
		prompt_filename,
		frameset_prompt_frame );

	return onload_javascript_string;
}

void vertical_new_post_prompt_insert_create(
		VERTICAL_NEW_FILENAME *vertical_new_filename,
		PROMPT_INSERT *prompt_insert )
{
	FILE *output_file;

	if ( !vertical_new_filename
	||   !vertical_new_filename->output_filename
	||   !prompt_insert
	||   !prompt_insert->document_form_html )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			vertical_new_filename->
				output_filename );

	fprintf(output_file,
		"%s\n",
		prompt_insert->document_form_html );

	fclose( output_file );
}
