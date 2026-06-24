/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_phrase.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "column.h"
#include "float.h"
#include "spool.h"
#include "optional_column.h"
#include "sql.h"
#include "sed.h"
#include "environ.h"
#include "process.h"
#include "application_constant.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "feeder_phrase.h"

LIST *feeder_phrase_list(
		const char *feeder_phrase_select,
		const char *feeder_phrase_table,
		boolean entity_contact_key_boolean )
{
	char *select_string;
	LIST *list;
	FILE *pipe_open;
	char input[ 1024 ];
	FEEDER_PHRASE *feeder_phrase;

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_select_string(
			feeder_phrase_select /* ENTITY_SELECT */,
			ENTITY_CONTACT_KEY_COLUMN,
			entity_contact_key_boolean );

	pipe_open =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				select_string,
				(char *)feeder_phrase_table,
				(char *)0 /* where */ ) );

	list = list_new();

	while ( string_input( input, pipe_open, sizeof ( input ) ) )
	{
		feeder_phrase =
			feeder_phrase_parse(
				entity_contact_key_boolean,
				input );

		if ( !feeder_phrase->nominal_account )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"feeder_phrase=[%s] has an empty nominal account.",
				feeder_phrase->phrase );

			pclose( pipe_open );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, feeder_phrase );
	}

	pclose( pipe_open );

	return list;
}

FEEDER_PHRASE *feeder_phrase_parse(
		boolean contact_key_boolean,
		char *input )
{
	char phrase[ 128 ];
	char buffer[ 128 ];
	FEEDER_PHRASE *feeder_phrase;

	if ( !input || !*input ) return NULL;

	/* See entity_select_string() */
	/* -------------------------- */
	piece( phrase, SQL_DELIMITER, input, 0 );
	feeder_phrase = feeder_phrase_new( strdup( phrase ) );

	/* Returns null if not enough delimiters */
	/* ------------------------------------- */
	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) feeder_phrase->nominal_account = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) feeder_phrase->full_name = strdup( buffer );

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 3 );
		if ( *buffer ) feeder_phrase->contact_key = strdup( buffer );
	}

	return feeder_phrase;
}

FEEDER_PHRASE *feeder_phrase_new( char *phrase )
{
	FEEDER_PHRASE *feeder_phrase;

	if ( !phrase || !*phrase )
	{
		char message[ 128 ];

		sprintf(message, "phrase is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_phrase = feeder_phrase_calloc();
	feeder_phrase->phrase = phrase;

	return feeder_phrase;
}

FEEDER_PHRASE *feeder_phrase_calloc( void )
{
	FEEDER_PHRASE *feeder_phrase;

	if ( ! ( feeder_phrase = calloc( 1, sizeof ( FEEDER_PHRASE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_phrase;
}

FEEDER_PHRASE *feeder_phrase_seek(
		char *financial_institution_full_name,
		char *financial_institution_contact_key,
		char *description_space_trim,
		LIST *feeder_phrase_list )
{
	if ( !financial_institution_full_name
	||   !description_space_trim )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_phrase_zap_match_length(
		feeder_phrase_list );

	feeder_phrase_set_match_length(
		FEEDER_PHRASE_DELIMITER,
		description_space_trim,
		feeder_phrase_list );

	return
	feeder_phrase_extract(
		financial_institution_full_name,
		financial_institution_contact_key,
		feeder_phrase_list );
}

void feeder_phrase_set_match_length(
		const char feeder_phrase_delimiter,
		char *description_space_trim,
		LIST *feeder_phrase_list )
{
	FEEDER_PHRASE *feeder_phrase;
	char feeder_component[ 128 ];
	int piece_number;

	if ( list_rewind( feeder_phrase_list ) )
	do {
		feeder_phrase = list_get( feeder_phrase_list );

		for(	piece_number = 0;
			piece(	feeder_component,
				feeder_phrase_delimiter,
				feeder_phrase->phrase,
				piece_number );
			piece_number++ )
		{
			if ( feeder_phrase_match_boolean(
				description_space_trim /* string */,
				feeder_component /* regular_expression */ ) )
			{
				feeder_phrase->match_length =
					strlen( feeder_component );
			}
		}

	} while ( list_next( feeder_phrase_list ) );
}

char *feeder_phrase_primary_where( char *feeder_phrase )
{
	static char primary_where[ 256 ];

	if ( !feeder_phrase )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_phrase is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		primary_where,
		sizeof ( primary_where ),
		"feeder_phrase = '%s'",
		feeder_phrase );

	return primary_where;
}

FEEDER_PHRASE *feeder_phrase_entity_set(
		char *financial_institution_full_name,
		char *financial_institution_contact_key,
		FEEDER_PHRASE *feeder_phrase )
{
	if ( !financial_institution_full_name
	||   !feeder_phrase )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_phrase->full_name =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		feeder_phrase_full_name(
			financial_institution_full_name,
			feeder_phrase->full_name );

	feeder_phrase->contact_key =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		feeder_phrase_contact_key(
			financial_institution_contact_key,
			feeder_phrase->contact_key );

	return feeder_phrase;
}

char *feeder_phrase_full_name(
		char *financial_institution_full_name,
		char *feeder_phrase_full_name )
{
	if ( feeder_phrase_full_name )
		return feeder_phrase_full_name;
	else
		return financial_institution_full_name;
}

char *feeder_phrase_contact_key(
		char *financial_institution_contact_key,
		char *feeder_phrase_contact_key )
{
	if ( feeder_phrase_contact_key )
		return feeder_phrase_contact_key;
	else
		return financial_institution_contact_key;
}

void feeder_phrase_zap_match_length( LIST *feeder_phrase_list )
{
	FEEDER_PHRASE *feeder_phrase;

	if ( list_rewind( feeder_phrase_list ) )
	do {
		feeder_phrase = list_get( feeder_phrase_list );
		feeder_phrase->match_length = 0;

	} while ( list_next( feeder_phrase_list ) );
}

FEEDER_PHRASE *feeder_phrase_extract(
		char *financial_institution_full_name,
		char *financial_institution_contact_key,
		LIST *feeder_phrase_list )
{
	FEEDER_PHRASE *return_feeder_phrase = {0};
	int highest_length = 0;
	FEEDER_PHRASE *feeder_phrase;

	if ( list_rewind( feeder_phrase_list ) )
	do {
		feeder_phrase = list_get( feeder_phrase_list );

		if ( feeder_phrase->match_length > highest_length )
		{
			return_feeder_phrase =
				feeder_phrase_entity_set(
					financial_institution_full_name,
					financial_institution_contact_key,
					feeder_phrase /* in/out */ );

			highest_length = feeder_phrase->match_length;
		}

	} while ( list_next( feeder_phrase_list ) );

	return return_feeder_phrase;
}

boolean feeder_phrase_match_boolean(
		char *description_space_trim,
		char *feeder_component )
{
	if ( string_exists(
		description_space_trim /* string */,
		feeder_component /* substr */ ) )
	{
		return 1;
	}

	return
	sed_match_boolean(
		description_space_trim /* string */,
		feeder_component /* regular_expression */ );
}

char *feeder_phrase_insert(
		const char *feeder_phrase_table,
		const char *feeder_phrase_select,
		const char *entity_contact_key_column,
		boolean contact_key_boolean,
		char *feeder_phrase,
		char *feeder_account,
		char *new_full_name,
		char *entity_contact_key )
{
	char *system_string;
	char *data_string;
	char *spool_string;

	if ( !feeder_phrase
	||   !feeder_account
	||   !new_full_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_phrase_insert_system_string(
			feeder_phrase_table,
			feeder_phrase_select,
			entity_contact_key_column,
			SQL_DELIMITER,
			contact_key_boolean );

	data_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_phrase_insert_data_string(
			SQL_DELIMITER,
			contact_key_boolean,
			feeder_phrase,
			feeder_account,
			new_full_name,
			entity_contact_key );

	spool_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		spool_data_string(
			system_string,
			data_string );

	free( system_string );
	free( data_string );

	return spool_string;
}

char *feeder_phrase_insert_data_string(
		const char sql_delimiter,
		boolean contact_key_boolean,
		char *feeder_phrase,
		char *feeder_account,
		char *new_full_name,
		char *entity_contact_key )
{
	char data_string[ 1024 ];
	OPTIONAL_COLUMN *optional_column;

	if ( !feeder_phrase
	||   !feeder_account
	||   !new_full_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		data_string,
		sizeof ( data_string ),
		"%s^%s^%s",
		feeder_phrase,
		feeder_account,
		new_full_name );

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			sql_delimiter,
			data_string /* base_string */,
			entity_contact_key /* component */,
			1 /* escape_boolean */,
			contact_key_boolean /* set_boolean */ );

	return optional_column->return_string;
}

char *feeder_phrase_insert_system_string(
		const char *feeder_phrase_table,
		const char *feeder_phrase_select,
		const char *entity_contact_key_column,
		const char sql_delimiter,
		boolean contact_key_boolean )
{
	char *insert_column_string;
	char system_string[ 1024 ];

	insert_column_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_insert_column_string(
			feeder_phrase_select /* entity_insert */,
			entity_contact_key_column,
			contact_key_boolean );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"insert_statement table=%s field=%s delimiter='%c' | "
		"sql 2>&1",
		feeder_phrase_table,
		insert_column_string,
		sql_delimiter );

	free( insert_column_string );

	return strdup( system_string );
}

