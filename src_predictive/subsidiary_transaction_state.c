/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/subsidiary_transaction_state.c	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "journal.h"
#include "subsidiary_transaction.h"
#include "subsidiary_transaction_state.h"

SUBSIDIARY_TRANSACTION_STATE *
	subsidiary_transaction_state_new(
		const char *preupdate_full_name_placeholder,
		const char *preupdate_street_address_placeholder,
		const char *preupdate_foreign_date_time_placeholder,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_foreign_date_time,
		char *full_name,
		char *street_address,
		char *foreign_date_time,
		LIST *insert_journal_list )
{
	SUBSIDIARY_TRANSACTION_STATE *subsidiary_transaction_state;

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

	subsidiary_transaction_state =
		subsidiary_transaction_state_calloc();

	subsidiary_transaction_state->preupdate_change_full_name =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_full_name
				/* preupdate_datum */,
			full_name
				/* postupdate_datum */,
			preupdate_full_name_placeholder
				/* preupdate_placeholder_name */ );

	subsidiary_transaction_state->preupdate_change_street_address =
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_street_address,
			street_address,
			preupdate_street_address_placeholder );

	subsidiary_transaction_state->preupdate_change_foreign_date_time =
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_foreign_date_time,
			foreign_date_time,
			preupdate_foreign_date_time_placeholder );

	subsidiary_transaction_state->old_journal_list =
		journal_transaction_list(
			JOURNAL_SELECT,
			JOURNAL_TABLE,
			full_name,
			street_address,
			foreign_date_time
				/* transaction_date_time */ );

	subsidiary_transaction_state->exist_boolean =
		subsidiary_transaction_state_exist_boolean(
			subsidiary_transaction_state->old_journal_list );

	if ( subsidiary_transaction_state->exist_boolean )
	{
		subsidiary_transaction_state->journal_list_match_boolean =
			journal_list_match_boolean(
				insert_journal_list /* journal1_list */,
				subsidiary_transaction_state->
					old_journal_list /* journal2_list */ );
	}

	subsidiary_transaction_state->subsidiary_transaction_insert =
		subsidiary_transaction_insert_new(
			subsidiary_transaction_state->
				preupdate_change_full_name,
			subsidiary_transaction_state->
				preupdate_change_street_address,
			subsidiary_transaction_state->
				preupdate_change_foreign_date_time,
			subsidiary_transaction_state->
				journal_list_match_boolean );

	subsidiary_transaction_state->subsidiary_transaction_delete =
		subsidiary_transaction_delete_new(
			subsidiary_transaction_state->
				preupdate_change_full_name,
			subsidiary_transaction_state->
				preupdate_change_street_address,
			subsidiary_transaction_state->
				preupdate_change_foreign_date_time,
			subsidiary_transaction_state->
				journal_list_match_boolean );

	return subsidiary_transaction_state;
}

SUBSIDIARY_TRANSACTION_STATE *
	subsidiary_transaction_state_calloc(
		void )
{
	SUBSIDIARY_TRANSACTION_STATE *
		subsidiary_transaction_state;

	if ( ! ( subsidiary_transaction_state =
		  calloc(
			1,
			sizeof ( SUBSIDIARY_TRANSACTION_STATE ) ) ) )
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

	return subsidiary_transaction_state;
}

boolean subsidiary_transaction_state_insert_boolean(
		PREUPDATE_CHANGE *
			preupdate_change_full_name,
		PREUPDATE_CHANGE *
			preupdate_change_street_address,
		PREUPDATE_CHANGE *
			preupdate_change_foreign_date_time,
		boolean journal_list_match_boolean )
{
	if (	preupdate_change_full_name->
			state_evaluate ==
		from_null_to_something
	||	preupdate_change_full_name->
			state_evaluate ==
		from_something_to_something_else )
	{
		return 1;
	}

	if (	preupdate_change_street_address->
			state_evaluate ==
		from_null_to_something
	||	preupdate_change_street_address->
			state_evaluate ==
		from_something_to_something_else )
	{
		return 1;
	}

	if (	preupdate_change_foreign_date_time->
			state_evaluate ==
		from_null_to_something
	||	preupdate_change_foreign_date_time->
			state_evaluate ==
		from_something_to_something_else )
	{
		return 1;
	}

	if ( !journal_list_match_boolean )
	{
		return 1;
	}
	
	return 0;
}

boolean subsidiary_transaction_state_delete_boolean(
		PREUPDATE_CHANGE *
			preupdate_change_full_name,
		PREUPDATE_CHANGE *
			preupdate_change_street_address,
		PREUPDATE_CHANGE *
			preupdate_change_foreign_date_time,
		boolean subsidiary_transaction_state_exist_boolean,
		boolean journal_list_match_boolean )
{
	if ( !preupdate_change_full_name
	||   !preupdate_change_street_address
	||   !preupdate_change_foreign_date_time )
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

	if (	preupdate_change_full_name->state_evaluate ==
		from_null_to_something
	||	preupdate_change_full_name->state_evaluate ==
		no_change_null )
	{
		return 0;
	}

	if (	!preupdate_change_full_name->
			no_change_boolean
	||	!preupdate_change_street_address->
			no_change_boolean
	||	!preupdate_change_foreign_date_time->
			no_change_boolean )
	{
		return 1;
	}

	if (	subsidiary_transaction_state_exist_boolean
	&&	!journal_list_match_boolean )
	{
		return 1;
	}
	
	return 0;
}

boolean subsidiary_transaction_state_exist_boolean( LIST *old_journal_list )
{
	return
	(boolean)list_length( old_journal_list );
}

