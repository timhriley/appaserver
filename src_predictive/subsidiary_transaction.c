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
#include "subsidiary_transaction.h"

SUBSIDIARY_TRANSACTION *
	subsidiary_transaction_new(
		const char *foreign_table_name,
		const char *foreign_full_name_column,
		const char *foreign_street_address_column,
		const char *foreign_date_time_column,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_foreign_date_time,
		char *preupdate_foreign_amount,
		char *preupdate_account_name,
		char *full_name,
		char *street_address,
		char *foreign_date_time,
		double foreign_amount,
		char *account_name,
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_street_address,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time,
		PREUPDATE_CHANGE *preupdate_change_foreign_amount,
		PREUPDATE_CHANGE *preupdate_change_account_name,
		char *debit_account_name,
		char *credit_account_name,
		char *transaction_memo )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
	boolean delete_boolean;
	boolean insert_boolean;

	if ( !preupdate_full_name
	||   !preupdate_street_address
	||   !preupdate_foreign_date_time
	||   !preupdate_foreign_amount
	||   !preupdate_account_name
	||   !preupdate_change_full_name
	||   !preupdate_change_street_address
	||   !preupdate_change_foreign_date_time
	||   !preupdate_change_foreign_amount
	||   !preupdate_change_account_name )
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

	subsidiary_transaction = subsidiary_transaction_calloc();

	delete_boolean =
		subsidiary_transaction_delete_boolean(
			preupdate_change_full_name,
			preupdate_change_street_address,
			preupdate_change_foreign_date_time,
			preupdate_change_foreign_amount,
			preupdate_change_account_name );

	if ( delete_boolean )
	{
		char *delete_full_name;
		char *delete_street_address;
		char *delete_transaction_date_time;

		delete_full_name =
			subsidiary_transaction_delete_datum(
				full_name,
				preupdate_full_name,
				preupdate_change_full_name );

		delete_street_address =
			subsidiary_transaction_delete_datum(
				street_address,	
				preupdate_street_address,
		 		preupdate_change_street_address );

		delete_transaction_date_time =
			subsidiary_transaction_delete_datum(
				foreign_date_time,
				preupdate_foreign_date_time,
				preupdate_change_foreign_date_time );

		subsidiary_transaction->delete_transaction =
			/* Safely returns */
			/* -------------- */
			subsidiary_transaction_delete_transaction(
				delete_full_name,
				delete_street_address,
				delete_transaction_date_time );
	}

	if ( full_name
	&&   street_address
	&&   foreign_date_time
	&&   !float_virtually_same( foreign_amount, 0.0 )
	&&   account_name )
	{
		insert_boolean =
			subsidiary_transaction_insert_boolean(
				preupdate_change_full_name,
				preupdate_change_street_address,
				preupdate_change_foreign_date_time,
				preupdate_change_foreign_amount,
				preupdate_change_account_name );

	    	if ( insert_boolean )
	    	{
			char *debit_account;
			char *credit_account;

			debit_account =
				/* ---------------------------- */
				/* Returns attribute_datum,	*/
				/* preupdate_attribute_datum,	*/
				/* or null			*/
				/* ---------------------------- */
				subsidiary_transaction_debit_account_name(
					debit_account_name,
					account_name );

			credit_account =
				subsidiary_transaction_credit_account_name(
					credit_account_name,
					account_name );

			subsidiary_transaction->insert_transaction =
				transaction_binary(
					full_name,
					street_address,
					foreign_date_time
					/* transaction_date_time */,
					foreign_amount
					/* transaction_amount */,
					transaction_memo,
					debit_account,
					credit_account );

		subsidiary_transaction->update_template =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			subsidiary_transaction_update_template(
				foreign_table_name,
				foreign_full_name_column,
				foreign_street_address_column,
				foreign_date_time_column,
				full_name,
				street_address,
				foreign_date_time );
		}
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

char *subsidiary_transaction_delete_datum(
		char *attribute_datum,
		char *preupdate_attribute_datum,
		PREUPDATE_CHANGE *preupdate_change_datum )
{
	char *delete_datum = {0};

	if ( !preupdate_attribute_datum
	||   !preupdate_change_datum )
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

	if (	preupdate_change_datum->state_evaluate ==
		no_change_null )
	{
		delete_datum = NULL;
	}
	else
	if (	preupdate_change_datum->state_evaluate ==
		no_change_something )
	{
		delete_datum = attribute_datum;
	}
	else
	if (	preupdate_change_datum->state_evaluate ==
		from_something_to_something_else )
	{
		delete_datum = preupdate_attribute_datum;
	}
	else
	if (	preupdate_change_datum->state_evaluate ==
		from_something_to_null )
	{
		if ( string_strncmp(
			preupdate_attribute_datum,
			"preupdate_" ) != 0 )
		{
			delete_datum = preupdate_attribute_datum;
		}
		else
		{

			/* state must be predelete */
			/* ----------------------- */
			delete_datum = attribute_datum;
		}
	}
	else
	if (	preupdate_change_datum->state_evaluate ==
		from_null_to_something )
	{
		delete_datum = NULL;
	}
	else
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"invalid state_evaluate=%d.",
			preupdate_change_datum->state_evaluate );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_datum;
}

boolean subsidiary_transaction_insert_boolean(
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_street_address,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time,
		PREUPDATE_CHANGE *preupdate_change_foreign_amount,
		PREUPDATE_CHANGE *preupdate_change_account_name )
{
	boolean insert_boolean = 0;

	if ( !preupdate_change_full_name
	||   !preupdate_change_street_address
	||   !preupdate_change_foreign_date_time
	||   !preupdate_change_foreign_amount
	||   !preupdate_change_account_name )
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
		from_something_to_something_else
	||	preupdate_change_street_address->state_evaluate ==
		from_null_to_something
	||	preupdate_change_street_address->state_evaluate ==
		from_something_to_something_else
	||	preupdate_change_foreign_date_time->state_evaluate ==
		from_null_to_something
	||	preupdate_change_foreign_date_time->state_evaluate ==
		from_something_to_something_else
	||	preupdate_change_foreign_amount->state_evaluate ==
		from_null_to_something
	||	preupdate_change_foreign_amount->state_evaluate ==
		from_something_to_something_else
	||	preupdate_change_account_name->state_evaluate ==
		from_null_to_something
	||	preupdate_change_account_name->state_evaluate ==
		from_something_to_something_else )
	{
		insert_boolean = 1;
	}

	return insert_boolean;
}

boolean subsidiary_transaction_delete_boolean(
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_street_address,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time,
		PREUPDATE_CHANGE *preupdate_change_foreign_amount,
		PREUPDATE_CHANGE *preupdate_change_account_name )
{
	boolean delete_boolean = 0;

	if ( !preupdate_change_full_name
	||   !preupdate_change_street_address
	||   !preupdate_change_foreign_date_time
	||   !preupdate_change_foreign_amount
	||   !preupdate_change_account_name )
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
		from_something_to_null
	||	preupdate_change_full_name->state_evaluate ==
		from_something_to_something_else
	||	preupdate_change_street_address->state_evaluate ==
		from_something_to_null
	||	preupdate_change_street_address->state_evaluate ==
		from_something_to_something_else
	||	preupdate_change_foreign_date_time->state_evaluate ==
		from_something_to_null
	||	preupdate_change_foreign_date_time->state_evaluate ==
		from_something_to_something_else
	||	preupdate_change_foreign_amount->state_evaluate ==
		from_something_to_null
	||	preupdate_change_foreign_amount->state_evaluate ==
		from_something_to_something_else
	||	preupdate_change_account_name->state_evaluate ==
		from_something_to_null
	||	preupdate_change_account_name->state_evaluate ==
		from_something_to_something_else )
	{
		delete_boolean = 1;
	}

	return delete_boolean;
}

char *subsidiary_transaction_update_template(
		const char *foreign_table_name,
		const char *foreign_full_name_column,
		const char *foreign_street_address_column,
		const char *foreign_date_time_column,
		char *full_name,
		char *street_address,
		char *foreign_date_time )
{
	char update_template[ 1024 ];

	if ( !full_name
	||   !street_address
	||   !foreign_date_time )
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

char *subsidiary_transaction_debit_account_name(
		char *debit_account_name,
		char *account_name )
{
	if ( debit_account_name )
		return debit_account_name;
	else
		return account_name;
}

char *subsidiary_transaction_credit_account_name(
		char *credit_account_name,
		char *account_name )
{
	if ( credit_account_name )
		return credit_account_name;
	else
		return account_name;
}

void subsidiary_transaction_execute(
		char *application_name,
		TRANSACTION *delete_transaction,
		TRANSACTION *insert_transaction,
		char *update_template )
{
	char *transaction_date_time;
	int cmp;
	char update_statement[ 1024 ];

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
		if ( !update_template )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"update_template is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

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

		cmp = strcmp(
			insert_transaction->transaction_date_time,
			transaction_date_time );

		if ( cmp != 0 )
		{
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

