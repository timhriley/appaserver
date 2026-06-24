/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/insert_feeder.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "String.h"
#include "appaserver.h"
#include "entity.h"
#include "insert_feeder.h"

INSERT_FEEDER *insert_feeder_new(
		char *feeder_account,
		char *new_full_name,
		char *feeder_phrase )
{
	INSERT_FEEDER *insert_feeder;

	insert_feeder = insert_feeder_calloc();

	insert_feeder->account_empty_boolean =
		insert_feeder_account_empty_boolean(
			feeder_account );

	insert_feeder->name_empty_boolean =
		insert_feeder_name_empty_boolean(
			new_full_name );

	insert_feeder->phrase_empty_boolean =
		insert_feeder_phrase_empty_boolean(
			feeder_phrase );

	insert_feeder->error_message =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		insert_feeder_error_message(
			insert_feeder->account_empty_boolean,
			insert_feeder->name_empty_boolean,
			insert_feeder->phrase_empty_boolean );

	return insert_feeder;
}

boolean insert_feeder_account_empty_boolean(
		char *feeder_account )
{
	boolean empty_boolean = 0;

	if ( !feeder_account
	||   !*feeder_account
	||   strcmp( feeder_account, "feeder_account" ) == 0 )
	{
		empty_boolean = 1;
	}

	return empty_boolean;
}

boolean insert_feeder_name_empty_boolean(
		char *new_full_name )
{
	boolean empty_boolean = 0;

	if ( !new_full_name
	||   !*new_full_name
	||   strcmp( new_full_name, "new_full_name" ) == 0 )
	{
		empty_boolean = 1;
	}

	return empty_boolean;
}

boolean insert_feeder_phrase_empty_boolean(
		char *feeder_phrase )
{
	boolean empty_boolean = 0;

	if ( !feeder_phrase
	||   !*feeder_phrase
	||   strcmp( feeder_phrase, "feeder_phrase" ) == 0 )
	{
		empty_boolean = 1;
	}

	return empty_boolean;
}

INSERT_FEEDER *insert_feeder_calloc( void )
{
	INSERT_FEEDER *insert_feeder;

	if ( ! ( insert_feeder =
			calloc( 1,
				sizeof ( INSERT_FEEDER ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return insert_feeder;
}

char *insert_feeder_error_message(
		boolean account_empty_boolean,
		boolean name_empty_boolean,
		boolean phrase_empty_boolean )
{
	char *error_message = {0};

	if ( account_empty_boolean )
		error_message =
			"Please choose a feeder account.";
	else
	if ( name_empty_boolean )
		error_message =
			"Please enter in a new full name.";
	else
	if ( phrase_empty_boolean )
		error_message =
			"Please enter in a feeder phrase.";

	return error_message;
}

