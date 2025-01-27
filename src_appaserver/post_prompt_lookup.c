/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/post_prompt_lookup.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "post_dictionary.h"
#include "appaserver_error.h"
#include "environ.h"
#include "piece.h"
#include "document.h"
#include "post_prompt_lookup.h"

void post_prompt_lookup_query_string_parse(
		char **application_name,
		char **session_key,
		char **login_name,
		char **role_name,
		char **folder_name,
		char **parameters,
		char *query_string );

int main( int argc, char **argv )
{
	char *application_name = {0};
	char *session_key = {0};
	char *login_name = {0};
	char *role_name = {0};
	char *folder_name = {0};
	char *parameters = {0};
	POST_DICTIONARY *post_dictionary;
	POST_PROMPT_LOOKUP *post_prompt_lookup;
	char *query_string;

	document_content_type_output();

	if ( argc == 1 )
	{
		query_string = getenv( "QUERY_STRING" );

		(void)string_search_replace(
			query_string /* source_destination */,
			"%20" /* search_string */,
			" " /* replace_string */ );

		post_prompt_lookup_query_string_parse(
			&application_name,
			&session_key,
			&login_name,
			&role_name,
			&folder_name,
			&parameters,
			query_string );
	}
	else
	{
		application_name = argv[ 1 ];
		session_key = argv[ 2 ];
		login_name = argv[ 3 ];
		role_name = argv[ 4 ];
		folder_name = argv[ 5 ];

		if ( argc == 7 ) parameters = argv[ 6 ];
	}

	if ( !application_name )
	{
		fprintf(stderr, 
	"Usage: %s application session login_name role folder [parameters]\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	if ( parameters )
	{
		post_dictionary =
			post_dictionary_string_new(
				DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
				DICTIONARY_ELEMENT_DELIMITER,
				parameters );
	}
	else
	{
		post_dictionary =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			post_dictionary_stdin_new(
				/* ---------------------------------- */
				/* Used when expecting a spooled file */
				/* ---------------------------------- */
				(char *)0 /* application_name */,
				(char *)0 /* upload_directory */,
				(LIST *)0 /* upload_filename_list */ );
	}

	post_prompt_lookup =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_prompt_lookup_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			post_dictionary->original_post_dictionary );

	if ( post_prompt_lookup->missing_html )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			(char *)0 /* process_name */ );

		printf(	"%s\n",
			post_prompt_lookup->missing_html );

		document_close();
	}
	else
	{
		if ( !post_prompt_lookup->execute_system_string )
		{
			char message[ 128 ];

			sprintf(message, "execute_system_string is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( system(
			post_prompt_lookup->
				execute_system_string ) ){}
	}

	return 0;
}

void post_prompt_lookup_query_string_parse(
		char **application_name,
		char **session_key,
		char **login_name,
		char **role_name,
		char **folder_name,
		char **parameters,
		char *query_string )
{
	char piece_buffer[ 1024 ];

	piece( piece_buffer, '+', query_string, 0 );
	if ( *piece_buffer ) *application_name = strdup( piece_buffer );

	piece( piece_buffer, '+', query_string, 1 );
	if ( *piece_buffer ) *session_key = strdup( piece_buffer );

	piece( piece_buffer, '+', query_string, 2 );
	if ( *piece_buffer ) *login_name = strdup( piece_buffer );

	piece( piece_buffer, '+', query_string, 3 );
	if ( *piece_buffer ) *role_name = strdup( piece_buffer );

	piece( piece_buffer, '+', query_string, 4 );
	if ( *piece_buffer ) *folder_name = strdup( piece_buffer );

	piece( piece_buffer, '+', query_string, 5 );
	if ( *piece_buffer ) *parameters = strdup( piece_buffer );
}

