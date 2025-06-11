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
#include "subsidiary_transaction.h"

SUBSIDIARY_TRANSACTION *
	subsidiary_transaction_new(
		const char *foreign_table_name,
		const char *foreign_full_name_column,
		const char *foreign_street_address_column,
		const char *foreign_date_time_column,
		char *transaction_date_time,
		LIST *insert_journal_list,
		char *insert_full_name,
		char *insert_street_address,
		char *foreign_date_time,
		double foreign_amount,
		char *transaction_memo,
		boolean insert_boolean,
		boolean delete_boolean,
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

	if ( delete_boolean )
	{
		subsidiary_transaction->delete_transaction =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			subsidiary_transaction_delete_transaction(
				subsidiary_transaction_delete->
					full_name,
				subsidiary_transaction_delete->
					street_address,
				subsidiary_transaction_delete->
					transaction_date_time );
	}

	if ( insert_boolean )
	{
		subsidiary_transaction->insert_transaction =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_new(
				insert_full_name,
				insert_street_address,
				foreign_date_time
					/* transaction_date_time */ );

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
				insert_full_name,
				insert_street_address,
				foreign_date_time,
				transaction_date_time );
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

TRANSACTION *subsidiary_transaction_delete_transaction(
		char *delete_full_name,
		char *delete_street_address,
		char *delete_transaction_date_time )
{
	TRANSACTION *delete_transaction = {0};

	if ( delete_full_name
	&&   delete_street_address
	&&   delete_transaction_date_time )
	{
		delete_transaction = transaction_calloc();

		delete_transaction->full_name =
			delete_full_name;

		delete_transaction->street_address =
			delete_street_address;

		delete_transaction->transaction_date_time =
			delete_transaction_date_time;
	}

	return delete_transaction;
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

