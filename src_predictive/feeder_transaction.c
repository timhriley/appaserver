/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_transaction.c			*/
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
#include "sql.h"
#include "sed.h"
#include "environ.h"
#include "process.h"
#include "application_constant.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "feeder_transaction.h"

FEEDER_TRANSACTION *feeder_transaction_calloc( void )
{
	FEEDER_TRANSACTION *feeder_transaction;

	if ( ! ( feeder_transaction =
			calloc( 1, sizeof ( FEEDER_TRANSACTION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_transaction;
}

FEEDER_TRANSACTION *feeder_transaction_new(
		char *fund_name,
		char *feeder_account_name,
		FEEDER_PHRASE *feeder_phrase_seek,
		LIABILITY_ENTITY *liability_entity,
		double amount,
		char *transaction_date_time,
		char *memo )
{
	FEEDER_TRANSACTION *feeder_transaction;
	char *debit_account_name;
	char *credit_account_name;

	if ( !feeder_account_name
	||   float_money_virtually_zero( amount )
	||   !transaction_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !feeder_phrase_seek
	&&   !liability_entity )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
		"both feeder_phrase_seek and liability_entity are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( liability_entity && !liability_entity->entity )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"liability_entity->entity is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_transaction = feeder_transaction_calloc();

	feeder_transaction->nominal_account_name =
		/* --------------------------------------- */
		/* Returns a component of either parameter */
		/* --------------------------------------- */
		feeder_transaction_nominal_account_name(
			feeder_phrase_seek,
			liability_entity );

	if ( amount < 0.0 )
	{
		debit_account_name =
			feeder_transaction->nominal_account_name;

		credit_account_name = feeder_account_name;

		amount = -amount;
	}
	else
	{
		debit_account_name = feeder_account_name;

		credit_account_name =
			feeder_transaction->nominal_account_name;
	}

	if ( !debit_account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"debit_account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !credit_account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"credit_account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_transaction->full_name =
		feeder_transaction_full_name(
			feeder_phrase_seek,
			liability_entity );

	feeder_transaction->contact_key =
		feeder_transaction_contact_key(
			feeder_phrase_seek,
			liability_entity );

	feeder_transaction->transaction =
		transaction_binary(
			fund_name,
			feeder_transaction->full_name,
			feeder_transaction->contact_key,
			transaction_date_time,
			amount /* transaction_amount */,
			memo,
			debit_account_name,
			credit_account_name );

	if ( !feeder_transaction->transaction )
	{
		char message[ 128 ];

		sprintf(message,
			"transaction_binary(%s/%.2lf) returned empty.",
			transaction_date_time,
			amount );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_transaction;
}

char *feeder_transaction_nominal_account_name(
		FEEDER_PHRASE *feeder_phrase_seek,
		LIABILITY_ENTITY *feeder_row_liability_entity )
{
	char *account_name;

	if ( feeder_phrase_seek )
		account_name =
			feeder_phrase_seek->
				nominal_account;
	else
		account_name =
			feeder_row_liability_entity->
				single_account_name;

	return account_name;
}

char *feeder_transaction_full_name(
		FEEDER_PHRASE *feeder_phrase_seek,
		LIABILITY_ENTITY *feeder_row_liability_entity )
{
	char *full_name;

	if ( feeder_phrase_seek )
		full_name =
			feeder_phrase_seek->
				full_name;
	else
		full_name =
			feeder_row_liability_entity->
				entity->
				full_name;

	return full_name;
}

char *feeder_transaction_contact_key(
		FEEDER_PHRASE *feeder_phrase_seek,
		LIABILITY_ENTITY *feeder_row_liability_entity )
{
	char *contact_key;

	if ( feeder_phrase_seek )
		contact_key =
			feeder_phrase_seek->
				contact_key;
	else
		contact_key =
			feeder_row_liability_entity->
				entity->
				contact_key;

	return contact_key;
}

