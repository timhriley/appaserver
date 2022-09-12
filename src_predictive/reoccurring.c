/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/reoccurring.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "date_convert.h"
#include "piece.h"
#include "date.h"
#include "column.h"
#include "appaserver_library.h"
#include "folder.h"
#include "html_table.h"
#include "entity.h"
#include "accrual.h"
#include "transaction.h"
#include "journal.h"
#include "reoccurring.h"

char *reoccurring_escape_transaction_description(
			char *transaction_description )
{
	static char escape_transaction_description[ 256 ];

	return string_escape_quote(
		escape_transaction_description,
		transaction_description );
}

char *reoccurring_primary_where(
			char *full_name,
			char *street_address,
			char *transaction_description )
{
	char where[ 256 ];

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "transaction_description = '%s'	",
		 entity_escape_full_name( full_name ),
		 street_address,
		 reoccurring_escape_transaction_description(
			transaction_description ) );

	return strdup( where );
}

REOCCURRING *reoccurring_parse(
			boolean property_attribute_exists,
			char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_description[ 128 ];
	char buffer[ 128 ];
	REOCCURRING *reoccurring;

	if ( !input || !*input ) return (REOCCURRING *)0;

	/* See reoccurring_select() */
	/* ------------------------ */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( transaction_description, SQL_DELIMITER, input, 2 );

	reoccurring =
		reoccurring_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_description ) );

	if ( piece( buffer, SQL_DELIMITER, input, 3 ) )
	{
		reoccurring->debit_account = strdup( buffer );
	}

	if ( piece( buffer, FOLDER_DATA_DELIMITER, input, 4 ) )
	{
		reoccurring->credit_account = strdup( buffer );
	}

	piece( buffer, FOLDER_DATA_DELIMITER, input, 5 );
	reoccurring->accrued_daily_amount = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input, 6 );
	reoccurring->accrued_monthly_amount = atof( buffer );

	if ( property_attribute_exists )
	{
		if ( piece( buffer, SQL_DELIMITER, input, 7 ) )
		{
			reoccurring->rental_property_street_address =
				strdup( buffer );
		}
	}

	reoccurring->transaction_increment_date_time =
		/* ------------------------------------ */
		/* Returns heap memory.			*/
		/* Increments second each invocation.   */
		/* ------------------------------------ */
		transaction_increment_date_time(
			(char *)0 /* transaction_date */ );

	return reoccurring;
}

FILE *reoccurring_input_pipe( char *system_string )
{
	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return popen( system_string, "r" );
}

LIST *reoccurring_system_list(
			char *system_string,
			boolean property_attribute_exists )
{
	LIST *list;
	FILE *input_pipe;
	char input[ 1024 ];

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	input_pipe = reoccurring_input_pipe( system_string );
	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			reoccurring_parse(
				property_attribute_exists,
				input ) );
	}

	pclose( input_pipe );

	return list;
}

char *reoccurring_memo( char *transaction_description,
			char *credit_account )
{
	char memo[ 256 ];

	sprintf( memo,
		 "%s/%s",
		 transaction_description,
		 credit_account );

	return strdup( memo );
}

int reoccurring_last_transaction_days_between(
			char *transaction_table,
			char *full_name,
			char *street_address,
			char *transaction_increment_date_time,
			char *debit_account,
			char *credit_account )
{
	if ( !transaction_increment_date_time
	||   !atoi( transaction_increment_date_time ) )
	{
		return 0;
	}

	return
	date_days_between(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		reoccurring_max_transaction_date_time(
			transaction_table,
			full_name,
			street_address,
			debit_account,
			credit_account )
				/* from_date_string */,
		transaction_increment_date_time
				/* to_date_string */ );
}

char *reoccurring_journal_transaction_subquery(
			char *journal_table,
			char *transaction_table,
			char *debit_account,
			char *credit_account )
{
	char subquery[ 1024 ];

	sprintf(subquery,
		"exists ( select 1 from %s				"
		"	   where %s.full_name =				"
		"		%s.full_name 				"
		"	     and %s.street_address =			"
		"		%s.street_address 			"
		"	     and %s.transaction_date_time =		"
		"		%s.transaction_date_time		"
		"	     and account = '%s' ) and			"
		"exists ( select 1 from %s				"
		"	   where %s.full_name =				"
		"		%s.full_name 				"
		"	     and %s.street_address =			"
		"		%s.street_address 			"
		"	     and %s.transaction_date_time =		"
		"		%s.transaction_date_time		"
		"	     and account = '%s' ) 			",
		journal_table,
		transaction_table,
		journal_table,
		transaction_table,
		journal_table,
		transaction_table,
		journal_table,
		debit_account,
		journal_table,
		transaction_table,
		journal_table,
		transaction_table,
		journal_table,
		transaction_table,
		journal_table,
		credit_account );

	return strdup( subquery );
}

TRANSACTION *reoccurring_daily_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_increment_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_daily_amount )
{
	int days_between;

	if ( !full_name
	||   !street_address
	||   !transaction_description
	||   !transaction_increment_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	if ( !accrued_daily_amount
	||   !debit_account
	||   !credit_account )
	{
		return (TRANSACTION *)0;
	}

	if ( ! ( days_between =
			reoccurring_last_transaction_days_between(
				TRANSACTION_TABLE,
				full_name,
				street_address,
				transaction_increment_date_time,
				debit_account,
				credit_account ) ) )
	{
		return (TRANSACTION *)0;
	}

	return
	transaction_binary(
		full_name,
		street_address,
		transaction_increment_date_time,
		reoccurring_daily_accrued_amount(
			accrued_daily_amount,
			days_between ),
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		reoccurring_memo(
			transaction_description,
			credit_account ),
		debit_account,
		credit_account );
}

TRANSACTION *reoccurring_monthly_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_increment_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_monthly_amount,
			char *rental_property_street_address )
{
	double monthly_accrue;
	char *max_transaction_date_time;
	TRANSACTION *transaction;

	if ( !full_name
	||   !street_address
	||   !transaction_description
	||   !transaction_increment_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !accrued_monthly_amount
	||   !debit_account
	||   !credit_account )
	{
		return (TRANSACTION *)0;
	}

	max_transaction_date_time =
		reoccurring_max_transaction_date_time(
			TRANSACTION_TABLE,
			full_name,
			street_address,
			debit_account,
			credit_account );

	monthly_accrue =
		accrual_monthly_accrue(
			max_transaction_date_time,
			transaction_increment_date_time,
			accrued_monthly_amount
				/* monthly_accrual */ );

	if ( money_virtually_same( monthly_accrue, 0.0 ) )
	{
		return (TRANSACTION *)0;
	}

	transaction =
		transaction_binary(
			full_name,
			street_address,
			transaction_increment_date_time,
			monthly_accrue,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			reoccurring_memo(
				transaction_description,
				credit_account ),
			debit_account,
			credit_account );

	transaction->rental_property_street_address =
		rental_property_street_address;

	return transaction;
}

char *reoccurring_system_string(
			char *select,
			char *table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !select
	||   !table
	||   !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" none",
		select,
		table,
		where );

	return strdup( system_string );
}

char *reoccurring_select(
			char *select_attributes,
			char *property_attribute,
			boolean property_attribute_exists )
{
	char select[ 1024 ];

	if ( !select_attributes
	||   !property_attribute )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	strcpy( select, select_attributes );

	if ( property_attribute_exists )
	{
		sprintf(select + strlen( select ),
			",%s",
			property_attribute );
	}

	return strdup( select );
}

boolean reoccurring_property_attribute_exists(
			char *application_name,
			char *reoccurring_table,
			char *reoccurring_property_attribute )
{
	return
	folder_attribute_exists(
			application_name,
			reoccurring_table /* folder_name */,
			reoccurring_property_attribute /* attribute_name */ );
}

double reoccurring_daily_accrued_amount(
			double accrued_daily_amount,
			int days_between )
{
	return (double)days_between * accrued_daily_amount;

}

char *reoccurring_max_transaction_date_time(
			char *transaction_table,
			char *full_name,
			char *street_address,
			char *debit_account,
			char *credit_account )
{
	char system_string[ 2048 ];
	char where[ 1024 ];
	char *tmp;

	sprintf(where,
		"full_name = '%s' and			"
		"street_address = '%s' and		"
		"%s					",
		entity_escape_full_name( full_name ),
		street_address,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		( tmp = reoccurring_journal_transaction_subquery(
			JOURNAL_TABLE,
			transaction_table,
			debit_account,
			credit_account ) ) );

	free( tmp );

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" none",
		"max( transaction_date_time )",
		transaction_table,
		where );

	/* Returns heap memory or null */
	/* --------------------------- */
	return string_pipe_fetch( system_string );
}

