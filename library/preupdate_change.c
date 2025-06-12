/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/preupdate_change.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "preupdate_change.h"

PREUPDATE_CHANGE *preupdate_change_new(
		const char *appaserver_insert_state,
		const char *appaserver_predelete_state,
		char *state,
		char *preupdate_datum,
		char *postupdate_datum,
		const char *preupdate_placeholder_name )
{
	PREUPDATE_CHANGE *preupdate_change;

	if ( !state )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"state is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	preupdate_change = preupdate_change_calloc();

	preupdate_change->state_evaluate =
		preupdate_change_state_evaluate(
			appaserver_insert_state,
			appaserver_predelete_state,
			state,
			preupdate_datum,
			postupdate_datum,
			preupdate_placeholder_name );

	preupdate_change->no_change_boolean =
		preupdate_change_no_change_boolean(
			preupdate_change->state_evaluate );

	if ( preupdate_change->no_change_boolean )
	{
		preupdate_datum = postupdate_datum;
	}

	preupdate_change->prior_datum =
		/* ----------------------------------------- */
		/* Returns preupdate_attribute_datum or null */
		/* ----------------------------------------- */
		preupdate_change_prior_datum(
			preupdate_datum
				/* preupdate_attribute_datum */,
			preupdate_change->state_evaluate );

	preupdate_change->new_datum =
		/* ------------------------------- */
		/* Returns attribute_datum or null */
		/* ------------------------------- */
		preupdate_change_new_datum(
			postupdate_datum
				/* attribute_datum */,
			preupdate_change->state_evaluate );

	return preupdate_change;
}

PREUPDATE_CHANGE *preupdate_change_calloc( void )
{
	PREUPDATE_CHANGE *preupdate_change;

	if ( ! ( preupdate_change = calloc( 1, sizeof ( PREUPDATE_CHANGE ) ) ) )
	{
		char message[ 128 ];

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

	return preupdate_change;
}

char *preupdate_change_state_display(
		enum preupdate_change_state state_evaluate )
{
	if ( state_evaluate == from_null_to_something )
		return "from_null_to_something";
	else
	if ( state_evaluate == from_something_to_null )
		return "from_something_to_null";
	else
	if ( state_evaluate == from_something_to_something_else )
		return "from_something_to_something_else";
	else
	if ( state_evaluate == no_change_null )
		return "no_change_null";
	else
	if ( state_evaluate == no_change_something )
		return "no_change_something";
	else
		return "unknown";
}

enum preupdate_change_state preupdate_change_state_evaluate(
		const char *appaserver_insert_state,
		const char *appaserver_predelete_state,
		char *state,
		char *preupdate_datum,
		char *postupdate_datum,
		const char *preupdate_placeholder_name )
{
	enum preupdate_change_state state_evaluate;

	if ( !preupdate_datum
	||   strcmp( preupdate_datum, "select" ) == 0 )
		preupdate_datum = "";

	if ( !postupdate_datum
	||   strcmp( postupdate_datum, "/" ) == 0 )
	{
		postupdate_datum = "";
	}

	if ( strcmp( preupdate_datum, preupdate_placeholder_name ) == 0 )
	{
		if ( *postupdate_datum )
			state_evaluate = no_change_something;
		else
			state_evaluate = no_change_null;
	}
	else
	if ( !*preupdate_datum && !*postupdate_datum )
	{
		state_evaluate = no_change_null;
	}
	else
	if ( strcmp( state, appaserver_insert_state ) == 0 )
	{
		if ( *postupdate_datum )
			state_evaluate = from_null_to_something;
		else
			state_evaluate = no_change_null;
	}
	else
	if ( strcmp( state, appaserver_predelete_state ) == 0 )
	{
		if ( *postupdate_datum )
			state_evaluate = from_something_to_null;
		else
			state_evaluate = no_change_null;
	}
	else
	if ( !*postupdate_datum )
		state_evaluate = from_something_to_null;
	else
	if ( !*preupdate_datum )
		state_evaluate = from_null_to_something;
	else
		state_evaluate = from_something_to_something_else;

	return state_evaluate;
}

boolean preupdate_change_no_change_boolean(
		enum preupdate_change_state state_evaluate )
{
	return
	(	state_evaluate == no_change_null
	||	state_evaluate == no_change_something );
}

char *preupdate_change_prior_datum(
		char *preupdate_attribute_datum,
		enum preupdate_change_state
			preupdate_change_state )
{
	char *prior_datum = {0};

	if (	preupdate_change_state ==
			from_null_to_something
	||	preupdate_change_state ==
			no_change_null )
	{
		prior_datum = NULL;
	}
	else
	if (	preupdate_change_state ==
			from_something_to_null
	||	preupdate_change_state ==
			from_something_to_something_else
	||	preupdate_change_state ==
			no_change_something )
	{
		prior_datum = preupdate_attribute_datum;
	}

	return prior_datum;
}

char *preupdate_change_new_datum(
		char *attribute_datum,
		enum preupdate_change_state
			preupdate_change_state )
{
	char *datum = {0};

	if (	preupdate_change_state ==
			from_something_to_null
	||	preupdate_change_state ==
			no_change_null )
	{
		datum = NULL;
	}
	else
	if (	preupdate_change_state ==
			from_null_to_something
	||	preupdate_change_state ==
			from_something_to_something_else
	||	preupdate_change_state ==
			no_change_something )
	{
		datum = attribute_datum;
	}

	return datum;
}

