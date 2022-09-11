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
#include "html_table.h"
#include "entity.h"
#include "accrual.h"
#include "transaction.h"
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
	char where[ 1024 ];

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

REOCCURRING *reoccurring_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_description[ 128 ];
	char buffer[ 128 ];
	REOCCURRING *reoccurring;

	/* See REOCCURRING_SELECT */
	/* ---------------------- */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( transaction_description, SQL_DELIMITER, input, 2 );

	reoccurring =
		reoccurring_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_description ) );

	piece( buffer, SQL_DELIMITER, input, 3 );
	reoccurring_transaction->debit_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input, 4 );
	reoccurring_transaction->credit_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input, 5 );
	reoccurring_transaction->accrued_daily_amount = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input, 6 );
	reoccurring_transaction->accrued_monthly_amount = atof( buffer );

	return reoccurring_transaction;
}

LIST *reoccurring_transaction_system_list( char *sys_string )
{
	LIST *reoccurring_transaction_list = list_new();
	FILE *input_pipe;
	char input[ 1024 ];

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			reoccurring_transaction_list,
			reoccurring_transaction_parse( input ) );
	}
	pclose( input_pipe );
	return reoccurring_transaction_list;
}

LIST *reoccurring_transaction_list( void )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '%s' %s \"%s\" \"%s\"",
		reoccurring_transaction_select(),
		"reoccurring_transaction",
		"bank_upload_feeder_phrase is not null",
		"bank_upload_feeder_phrase desc" );

	return reoccurring_transaction_system_list( sys_string );
}

REOCCURRING_TRANSACTION *reoccurring_bank_upload_feeder_phrase(
				LIST *reoccurring_transaction_list,
				char *bank_description )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	char feeder_phrase_piece[ 1024 ];
	int p;

	if ( !list_rewind( reoccurring_transaction_list ) )
		return (REOCCURRING_TRANSACTION *)0;

	do {
		reoccurring_transaction =
			list_get(
				reoccurring_transaction_list );

		for(	p = 0;
			piece(	feeder_phrase_piece,
				FEEDER_PHRASE_DELIMITER,
				reoccurring_transaction->
					bank_upload_feeder_phrase,
				p );
			p++ )
		{
			if ( timlib_exists_string(
				bank_description /* string */,
				feeder_phrase_piece /* substring */ ) )
			{
				return reoccurring_transaction;
			}
		}

	} while( list_next( reoccurring_transaction_list ) );

	return (REOCCURRING_TRANSACTION *)0;
}

/* --------------------- */
/* Returns static memory */
/* --------------------- */
char *reoccurring_memo(
				char *transaction_description,
				char *credit_account )
{
	static char memo[ 256 ];

	sprintf( memo,
		 "%s/%s",
		 transaction_description,
		 credit_account );

	return memo;
}

int reoccurring_days_between_last_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account )
{
	char system_string[ 2048 ];
	int current_year;
	char where[ 1024 ];
	char *subquery;
	char *select;
	char *folder;
	char *max_transaction_date;
	char end_date_string[ 16 ];
	int days_between;

	if ( !transaction_date_time
	||   ! ( current_year = atoi( transaction_date_time ) ) )
	{
		return 0;
	}

	column( end_date_string, 0, transaction_date_time );

	select = "max( transaction_date_time )";
	folder = "transaction";

	subquery =
		reoccurring_transaction_subquery(
			debit_account,
			credit_account );

	sprintf( where,
		 "full_name = '%s' and				"
		 "street_address = '%s' and			"
		 "%s						",
		 entity_escape_full_name( full_name ),
		 street_address,
		 subquery );

	sprintf(sys_string,
		"select.sh '%s' %s \"%s\" %s		|"
		"column.e 0				 ",
		select,
		folder,
		where,
		"select" );

	max_transaction_date = pipe2string( sys_string );

	if ( !timlib_strlen( max_transaction_date ) )
	{
		days_between = 1;
	}
	else
	{
		days_between =
			date_days_between(
				max_transaction_date /* from_date */,
				end_date_string /* to_date */ );
	}

	return days_between;
}

char *reoccurring_transaction_subquery(
			char *debit_account,
			char *credit_account )
{
	char subquery[ 1024 ];

	sprintf(subquery,
		"exists ( select 1 from journal_ledger			"
		"	   where transaction.full_name =		"
		"		journal_ledger.full_name 		"
		"	     and transaction.street_address =		"
		"		journal_ledger.street_address 		"
		"	     and transaction.transaction_date_time =	"
		"		journal_ledger.transaction_date_time	"
		"	     and account = '%s' ) and			"
		"exists ( select 1 from journal_ledger			"
		"	   where transaction.full_name =		"
		"		journal_ledger.full_name 		"
		"	     and transaction.street_address =		"
		"		journal_ledger.street_address 		"
		"	     and transaction.transaction_date_time =	"
		"		journal_ledger.transaction_date_time	"
		"	     and account = '%s' ) 			",
		debit_account,
		credit_account );

	return strdup( subquery );
}

TRANSACTION *reoccurring_accrued_daily_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_increment_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_daily_amount )
{
	TRANSACTION *transaction;
	JOURNAL *journal;
	int days_between;
	double accrued_amount;
	char *memo;

	if ( ! ( days_between =
			reoccurring_days_between_last_transaction(
				full_name,
				street_address,
				transaction_date_time,
				debit_account,
				credit_account ) ) )
	{
		return (TRANSACTION *)0;
	}

	if ( days_between < 0.0 ) return (TRANSACTION *)0;

	accrued_amount = (double)days_between * accrued_daily_amount;

	if ( timlib_dollar_virtually_same( accrued_amount, 0.0 ) )
	{
		return (TRANSACTION *)0;
	}

	memo =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		reoccurring_memo(
			transaction_description,
			credit_account );

	transaction =
		transaction_new(
			full_name,
			street_address,
			transaction_date_time );

	transaction->memo = strdup( memo );

	transaction->transaction_amount = accrued_amount;

	transaction->journal_list = list_new();

	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			debit_account );

	journal->debit_amount = transaction->transaction_amount;

	list_set( transaction->journal_list, journal );

	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			credit_account );

	journal->credit_amount = transaction->transaction_amount;

	list_set( transaction->journal_list, journal );

	return transaction;
}

TRANSACTION *post_reoccurring_get_accrued_monthly_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_monthly_amount )
{
	TRANSACTION *transaction;
	JOURNAL *journal;
	double accrued_amount;
	char *begin_date_string;
	char end_date_string[ 16 ];
	char *memo;

	begin_date_string =
			reoccurring_last_transaction_date(
				full_name,
				street_address,
				transaction_date_time,
				debit_account,
				credit_account );

	column( end_date_string, 0, transaction_date_time );

	accrued_amount =
		accrual_monthly_accrue(
			begin_date_string,
			end_date_string,
			accrued_monthly_amount
				/* monthly_accrual */ );

	if ( timlib_dollar_virtually_same( accrued_amount, 0.0 ) )
	{
		return (TRANSACTION *)0;
	}

	memo =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		reoccurring_memo(
			transaction_description,
			credit_account );

	transaction =
		transaction_new(
			full_name,
			street_address,
			transaction_date_time );

	transaction->memo = strdup( memo );

	transaction->transaction_amount = accrued_amount;

	transaction->journal_list = list_new();

	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			debit_account );

	journal->debit_amount = transaction->transaction_amount;

	list_set( transaction->journal_list, journal );

	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			credit_account );

	journal->credit_amount = transaction->transaction_amount;

	list_set( transaction->journal_list, journal );

	return transaction;
}

