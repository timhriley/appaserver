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
#include "predictive.h"
#include "journal.h"
#include "subsidiary_transaction.h"
#include "subsidiary_transaction_state.h"

SUBSIDIARY_TRANSACTION_STATE *
	subsidiary_transaction_state_new(
		const char *preupdate_fund_name_placeholder,
		const char *preupdate_full_name_placeholder,
		const char *preupdate_contact_key_placeholder,
		const char *preupdate_foreign_date_time_placeholder,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_foreign_date_time,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *foreign_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
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

	subsidiary_transaction_state->predictive_fund_name =
		/* ------------------------- */
		/* Returns parameter or null */
		/* ------------------------- */
		predictive_fund_name(
			fund_name,
			fund_boolean );

	subsidiary_transaction_state->preupdate_change_fund_name =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_fund_name
				/* preupdate_datum */,
			subsidiary_transaction_state->predictive_fund_name
				/* postupdate_datum */,
			preupdate_fund_name_placeholder
				/* preupdate_placeholder_name */ );

	subsidiary_transaction_state->preupdate_change_full_name =
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

	subsidiary_transaction_state->entity_contact_key =
		/* --------------------------------------- */
		/* Returns parameter, heap memory, or null */
		/* --------------------------------------- */
		entity_contact_key(
			ENTITY_TABLE,
			ENTITY_CONTACT_KEY_COLUMN,
			ENTITY_CONTACT_KEY_DEFAULT,
			full_name,
			contact_key,
			contact_key_boolean );

	subsidiary_transaction_state->preupdate_change_contact_key =
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_contact_key,
			subsidiary_transaction_state->entity_contact_key,
			preupdate_contact_key_placeholder );

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
			subsidiary_transaction_state->
				preupdate_change_fund_name->
				prior_datum,
			subsidiary_transaction_state->
				preupdate_change_full_name->
				prior_datum,
			subsidiary_transaction_state->
				preupdate_change_contact_key->
				prior_datum,
			subsidiary_transaction_state->
				preupdate_change_foreign_date_time->
				prior_datum
					/* transaction_date_time */,
			fund_boolean,
			contact_key_boolean );

	subsidiary_transaction_state->exist_boolean =
		/* ----------------------- */
		/* Any journal rows exist? */
		/* ----------------------- */
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
				preupdate_change_fund_name,
			subsidiary_transaction_state->
				preupdate_change_full_name,
			subsidiary_transaction_state->
				preupdate_change_contact_key,
			subsidiary_transaction_state->
				preupdate_change_foreign_date_time,
			subsidiary_transaction_state->
				journal_list_match_boolean );

	subsidiary_transaction_state->subsidiary_transaction_delete =
		subsidiary_transaction_delete_new(
			subsidiary_transaction_state->
				preupdate_change_fund_name,
			subsidiary_transaction_state->
				preupdate_change_full_name,
			subsidiary_transaction_state->
				preupdate_change_contact_key,
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
			preupdate_change_contact_key,
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

	if (	preupdate_change_contact_key->
			state_evaluate ==
		from_null_to_something
	||	preupdate_change_contact_key->
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
			preupdate_change_contact_key,
		PREUPDATE_CHANGE *
			preupdate_change_foreign_date_time,
		boolean subsidiary_transaction_state_exist_boolean,
		boolean journal_list_match_boolean )
{
	if ( !preupdate_change_full_name
	||   !preupdate_change_contact_key
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
	||	!preupdate_change_contact_key->
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

