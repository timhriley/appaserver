/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/subsidiary_transaction.c 		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "float.h"
#include "sql.h"
#include "update.h"
#include "journal.h"
#include "preupdate_change.h"
#include "subsidiary_transaction.h"

SUBSIDIARY_TRANSACTION *
	subsidiary_transaction_new(
		const char *foreign_table_name,
		const char *foreign_full_name_column,
		const char *foreign_street_address_column,
		const char *foreign_date_time_column,
		char *prior_transaction_date_time,
		LIST *insert_journal_list,
		double foreign_amount,
		char *transaction_memo,
		SUBSIDIARY_TRANSACTION_INSERT *
			subsidiary_transaction_insert,
		SUBSIDIARY_TRANSACTION_DELETE *
			subsidiary_transaction_delete )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;

	if ( !foreign_amount || foreign_amount < 0.0 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"invalid foreign_amount=%.2lf",
			foreign_amount );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	subsidiary_transaction = subsidiary_transaction_calloc();

	if ( subsidiary_transaction_delete )
	{
		subsidiary_transaction->delete_transaction =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_new(
				subsidiary_transaction_delete->
					full_name,
				subsidiary_transaction_delete->
					street_address,
				subsidiary_transaction_delete->
					transaction_date_time );
	}

	if ( subsidiary_transaction_insert )
	{
		subsidiary_transaction->insert_transaction =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_new(
				subsidiary_transaction_insert->
					full_name,
				subsidiary_transaction_insert->
					street_address,
				subsidiary_transaction_insert->
					transaction_date_time );

		subsidiary_transaction->
			insert_transaction->
			journal_list =
				insert_journal_list;

		subsidiary_transaction->
			insert_transaction->
			transaction_amount =
				foreign_amount;

		subsidiary_transaction->
			insert_transaction->
			memo =
				transaction_memo;

		subsidiary_transaction->update_template =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			subsidiary_transaction_update_template(
				foreign_table_name,
				foreign_full_name_column,
				foreign_street_address_column,
				foreign_date_time_column,
				subsidiary_transaction_insert->
					full_name,
				subsidiary_transaction_insert->
					street_address,
				subsidiary_transaction_insert->
					transaction_date_time
					/* foreign_date_time */,
				prior_transaction_date_time );
	}

	return subsidiary_transaction;
}

SUBSIDIARY_TRANSACTION *subsidiary_transaction_calloc( void )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;

	if ( ! ( subsidiary_transaction =
		  calloc(
			1,
			sizeof ( SUBSIDIARY_TRANSACTION ) ) ) )
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

	return subsidiary_transaction;
}

char *subsidiary_transaction_update_template(
		const char *foreign_table_name,
		const char *foreign_full_name_column,
		const char *foreign_street_address_column,
		const char *foreign_date_time_column,
		char *full_name,
		char *street_address,
		char *foreign_date_time,
		char *transaction_date_time )
{
	char update_template[ 1024 ];

	if ( !full_name
	||   !street_address )
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

	if ( !foreign_date_time
	||   !transaction_date_time
	||   strcmp(
		foreign_date_time,
		transaction_date_time ) == 0 )
	{
		return NULL;
	}

	snprintf(
		update_template,
		sizeof ( update_template ),
		"update %s "
		"set %s = '%cs' "
		"where %s = '%s' and "
		"%s = '%s' and "
		"%s = '%s';",
		foreign_table_name,
		foreign_date_time_column,
		'%',
		foreign_full_name_column,
		full_name,
		foreign_street_address_column,
		street_address,
		foreign_date_time_column,
		foreign_date_time );

	return strdup( update_template );
}

void subsidiary_transaction_execute(
		char *application_name,
		TRANSACTION *delete_transaction,
		TRANSACTION *insert_transaction,
		char *update_template )
{
	char *transaction_date_time;

	if ( !application_name )
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

	if ( delete_transaction )
	{
		transaction_delete(
			delete_transaction->full_name,
			delete_transaction->street_address,
			delete_transaction->transaction_date_time );
	}

	if ( insert_transaction )
	{
		transaction_date_time =
			/* -------------------------------------- */
			/* Returns inserted transaction_date_time */
			/* -------------------------------------- */
			transaction_insert(
				insert_transaction->full_name,
				insert_transaction->street_address,
				insert_transaction->transaction_date_time,
				insert_transaction->transaction_amount,
				0 /* check_number */,
				insert_transaction->memo,
				'n' /* lock_transaction_yn not present */,
				insert_transaction->journal_list,
				1 /* insert_journal_list_boolean */ );

		if ( update_template )
		{
			int cmp;

			cmp = strcmp(
				insert_transaction->transaction_date_time,
				transaction_date_time );

			if ( cmp != 0 )
			{
				char update_statement[ 1024 ];

				snprintf(
					update_statement,
					sizeof ( update_statement ),
					update_template,
					transaction_date_time );
	
				update_statement_execute(
					SQL_EXECUTABLE,
					application_name,
					update_statement );
			}
		}
	}
}

SUBSIDIARY_TRANSACTION_DELETE *
	subsidiary_transaction_delete_new(
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_foreign_date_time,
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_street_address,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time,
		boolean journal_list_match_boolean )
{
	SUBSIDIARY_TRANSACTION_DELETE *subsidiary_transaction_delete;

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

	if (	preupdate_change_foreign_date_time->no_change_boolean
	&&	journal_list_match_boolean )
	{
		return NULL;
	}

	subsidiary_transaction_delete = subsidiary_transaction_delete_calloc();

	subsidiary_transaction_delete->full_name =
		/* ----------------------------------------- */
		/* Returns preupdate_attribute_datum or null */
		/* ----------------------------------------- */
		preupdate_change_prior_datum(
			preupdate_full_name
				/* preupdate_attribute_datum */,
			preupdate_change_full_name->
				state_evaluate );

	if ( !subsidiary_transaction_delete->full_name )
	{
		free( subsidiary_transaction_delete );
		return NULL;
	}

	subsidiary_transaction_delete->street_address =
		/* ----------------------------------------- */
		/* Returns preupdate_attribute_datum or null */
		/* ----------------------------------------- */
		preupdate_change_prior_datum(
			preupdate_street_address
				/* preupdate_attribute_datum */,
			preupdate_change_street_address->
				state_evaluate );

	if ( !subsidiary_transaction_delete->street_address )
	{
		free( subsidiary_transaction_delete );
		return NULL;
	}

	subsidiary_transaction_delete->transaction_date_time =
		/* ----------------------------------------- */
		/* Returns preupdate_attribute_datum or null */
		/* ----------------------------------------- */
		preupdate_change_prior_datum(
			preupdate_foreign_date_time
				/* preupdate_attribute_datum */,
			preupdate_change_foreign_date_time->
				state_evaluate );

	if ( !subsidiary_transaction_delete->transaction_date_time )
	{
		free( subsidiary_transaction_delete );
		return NULL;
	}

	return subsidiary_transaction_delete;
}

SUBSIDIARY_TRANSACTION_DELETE *
	subsidiary_transaction_delete_calloc(
		void )
{
	SUBSIDIARY_TRANSACTION_DELETE *subsidiary_transaction_delete;

	if ( ! (  subsidiary_transaction_delete =
			calloc( 1,
				sizeof ( SUBSIDIARY_TRANSACTION_DELETE ) ) ) )
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

	return subsidiary_transaction_delete;
}

SUBSIDIARY_TRANSACTION_INSERT *
	subsidiary_transaction_insert_new(
		char *full_name,
		char *street_address,
		char *foreign_date_time,
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_street_address,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time,
		boolean journal_list_match_boolean )
{
	SUBSIDIARY_TRANSACTION_INSERT *subsidiary_transaction_insert;

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

	if (	preupdate_change_foreign_date_time->no_change_boolean
	&&	journal_list_match_boolean )
	{
		return NULL;
	}

	subsidiary_transaction_insert = subsidiary_transaction_insert_calloc();

	subsidiary_transaction_insert->full_name =
		/* ------------------------------- */
		/* Returns attribute_datum or null */
		/* ------------------------------- */
		preupdate_change_prior_datum(
			full_name /* attribute_datum */,
			preupdate_change_full_name->
				state_evaluate );

	if ( !subsidiary_transaction_insert->full_name )
	{
		free( subsidiary_transaction_insert );
		return NULL;
	}

	subsidiary_transaction_insert->street_address =
		preupdate_change_prior_datum(
			street_address /* attribute_datum */,
			preupdate_change_street_address->
				state_evaluate );

	if ( !subsidiary_transaction_insert->street_address )
	{
		free( subsidiary_transaction_insert );
		return NULL;
	}

	subsidiary_transaction_insert->transaction_date_time =
		preupdate_change_prior_datum(
			foreign_date_time /* attribute_datum */,
			preupdate_change_foreign_date_time->
				state_evaluate );

	if ( !subsidiary_transaction_insert->transaction_date_time )
	{
		free( subsidiary_transaction_insert );
		return NULL;
	}

	return subsidiary_transaction_insert;
}

SUBSIDIARY_TRANSACTION_INSERT *
	subsidiary_transaction_insert_calloc(
		void )
{
	SUBSIDIARY_TRANSACTION_INSERT *subsidiary_transaction_insert;

	if ( ! (  subsidiary_transaction_insert =
			calloc( 1,
				sizeof ( SUBSIDIARY_TRANSACTION_INSERT ) ) ) )
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

	return subsidiary_transaction_insert;
}

