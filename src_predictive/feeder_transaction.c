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
		double amount,
		char *transaction_date_time,
		char *memo )
{
	FEEDER_TRANSACTION *feeder_transaction;

	if ( !feeder_account_name
	||   !feeder_phrase_seek
	||   !amount
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

	feeder_transaction = feeder_transaction_calloc();

	if ( amount < 0.0 )
	{
		feeder_transaction->debit_account =
			feeder_phrase_seek->
				nominal_account;

		feeder_transaction->credit_account = feeder_account_name;
		amount = -amount;
	}
	else
	{
		feeder_transaction->debit_account = feeder_account_name;

		feeder_transaction->credit_account =
			feeder_phrase_seek->
				nominal_account;
	}

	if ( !feeder_transaction->debit_account )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_transaction->debit_account is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !feeder_transaction->credit_account )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_transaction->credit_account is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_transaction->transaction =
		transaction_binary(
			feeder_phrase_seek->full_name,
			feeder_phrase_seek->street_address,
			transaction_date_time,
			amount /* transaction_amount */,
			memo,
			feeder_transaction->debit_account
				/* debit_account_name */,
			feeder_transaction->credit_account
				/* credit_account_name */ );

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

	feeder_transaction->transaction->fund_name = fund_name;

	return feeder_transaction;
}

