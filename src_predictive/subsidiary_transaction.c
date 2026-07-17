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
#include "predictive.h"
#include "journal.h"
#include "preupdate_change.h"
#include "subsidiary_transaction.h"

SUBSIDIARY_TRANSACTION *
	subsidiary_transaction_new(
		const char *foreign_table_name,
		const char *foreign_fund_name_column,
		const char *foreign_full_name_column,
		const char *foreign_contact_key_column,
		const char *foreign_date_time_column,
		const char *update_date_time_column,
		LIST *insert_journal_list,
		double foreign_amount,
		char *transaction_memo,
		SUBSIDIARY_TRANSACTION_INSERT *
			subsidiary_transaction_insert,
		SUBSIDIARY_TRANSACTION_DELETE *
			subsidiary_transaction_delete,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;

	subsidiary_transaction = subsidiary_transaction_calloc();

	subsidiary_transaction->predictive_fund_boolean =
		fund_boolean;

	subsidiary_transaction->entity_contact_key_boolean =
		contact_key_boolean;

	if ( subsidiary_transaction_delete )
	{
		subsidiary_transaction->delete_transaction =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_new(
				subsidiary_transaction_delete->
					fund_name,
				subsidiary_transaction_delete->
					full_name,
				subsidiary_transaction_delete->
					contact_key,
				subsidiary_transaction_delete->
					transaction_date_time );
	}

	if ( foreign_amount > 0.0
	&&   subsidiary_transaction_insert )
	{
		subsidiary_transaction->insert_transaction =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_new(
				subsidiary_transaction_insert->
					fund_name,
				subsidiary_transaction_insert->
					full_name,
				subsidiary_transaction_insert->
					contact_key,
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
				foreign_fund_name_column,
				foreign_full_name_column,
				foreign_contact_key_column,
				foreign_date_time_column,
				update_date_time_column,
				subsidiary_transaction_insert->
					fund_name,
				subsidiary_transaction_insert->
					full_name,
				subsidiary_transaction_insert->
					contact_key,
				subsidiary_transaction_insert->
					transaction_date_time
					/* foreign_date_time */,
				fund_boolean,
				contact_key_boolean );
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
		const char *foreign_fund_name_column,
		const char *foreign_full_name_column,
		const char *foreign_contact_key_column,
		const char *foreign_date_time_column,
		const char *update_date_time_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *foreign_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *fund_where;
	char *primary_where;
	char update_template[ 1024 ];

	if ( !full_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fund_where =
		/* ------------------------------ */
		/* Change it to the foreign where */
		/* Returns static memory.	  */
		/* ------------------------------ */
		predictive_fund_where(
			foreign_fund_name_column
				/* PREDICTIVE_FUND_COLUMN */,
			fund_name,
			fund_boolean );

	primary_where =
		/* ------------------------------ */
		/* Change it to the foreign where */
		/* Returns static memory.	  */
		/* ------------------------------ */
		entity_primary_where(
			foreign_full_name_column
				/* ENTITY_FULL_NAME_COLUMN */,
			foreign_contact_key_column
				/* ENTITY_CONTACT_KEY_COLUMN */,
			(char *)0 /* table_name */,
			full_name,
			contact_key,
			contact_key_boolean );

	if ( !foreign_date_time ) return NULL;

	snprintf(
		update_template,
		sizeof ( update_template ),
		"update %s "
		"set %s = '%cs' "
		"where %s and %s and %s = '%s';",
		foreign_table_name,
		update_date_time_column,
		'%',
		fund_where,
		primary_where,
		foreign_date_time_column,
		foreign_date_time );

	return strdup( update_template );
}

char *subsidiary_transaction_execute(
		char *application_name,
		TRANSACTION *delete_transaction,
		TRANSACTION *insert_transaction,
		char *update_template,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *transaction_date_time = {0};

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
			delete_transaction->fund_name,
			delete_transaction->full_name,
			delete_transaction->contact_key,
			delete_transaction->transaction_date_time,
			fund_boolean,
			contact_key_boolean );
	}

	if ( insert_transaction )
	{
		transaction_date_time =
			/* -------------------------------------- */
			/* Returns inserted transaction_date_time */
			/* -------------------------------------- */
			transaction_insert(
				insert_transaction->fund_name,
				insert_transaction->full_name,
				insert_transaction->contact_key,
				insert_transaction->transaction_date_time,
				insert_transaction->transaction_amount,
				0 /* check_number */,
				insert_transaction->memo,
				insert_transaction->journal_list,
				fund_boolean,
				contact_key_boolean,
				1 /* insert_journal_list_boolean */ );
	}

	if ( update_template && transaction_date_time )
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

	return transaction_date_time;
}

SUBSIDIARY_TRANSACTION_DELETE *
	subsidiary_transaction_delete_new(
		PREUPDATE_CHANGE *preupdate_change_fund_name,
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_contact_key,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time )
{
	SUBSIDIARY_TRANSACTION_DELETE *subsidiary_transaction_delete;

	if ( !preupdate_change_fund_name
	||   !preupdate_change_full_name
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

	if (	preupdate_change_fund_name->no_change_boolean
	&&	preupdate_change_full_name->no_change_boolean
	&&  	preupdate_change_contact_key->no_change_boolean
	&&  	preupdate_change_foreign_date_time->no_change_boolean )
	{
		return NULL;
	}

	subsidiary_transaction_delete = subsidiary_transaction_delete_calloc();

	subsidiary_transaction_delete->fund_name =
		preupdate_change_fund_name->
			prior_datum;

	subsidiary_transaction_delete->full_name =
		preupdate_change_full_name->
			prior_datum;

	if ( !subsidiary_transaction_delete->full_name )
	{
		free( subsidiary_transaction_delete );
		return NULL;
	}

	subsidiary_transaction_delete->contact_key =
		preupdate_change_contact_key->
			prior_datum;

	subsidiary_transaction_delete->transaction_date_time =
		preupdate_change_foreign_date_time->
			prior_datum;

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
		PREUPDATE_CHANGE *preupdate_change_fund_name,
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_contact_key,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time )
{
	SUBSIDIARY_TRANSACTION_INSERT *subsidiary_transaction_insert;

	if ( !preupdate_change_fund_name
	||   !preupdate_change_full_name
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

	if (	preupdate_change_fund_name->no_change_boolean
	&&	preupdate_change_full_name->no_change_boolean
	&&  	preupdate_change_contact_key->no_change_boolean
	&&  	preupdate_change_foreign_date_time->no_change_boolean )
	{
		return NULL;
	}

	subsidiary_transaction_insert = subsidiary_transaction_insert_calloc();

	subsidiary_transaction_insert->fund_name =
		preupdate_change_fund_name->
			new_datum;

	subsidiary_transaction_insert->full_name =
		preupdate_change_full_name->
			new_datum;

	if ( !subsidiary_transaction_insert->full_name )
	{
		free( subsidiary_transaction_insert );
		return NULL;
	}

	subsidiary_transaction_insert->contact_key =
		preupdate_change_contact_key->
			new_datum;

	subsidiary_transaction_insert->transaction_date_time =
		preupdate_change_foreign_date_time->
			new_datum;

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

