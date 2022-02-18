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
#include "reoccurring.h"

REOCCURRING_TRANSACTION *reoccurring_transaction_new(
			char *full_name,
			char *street_address,
			char *transaction_description )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	reoccurring_transaction = reoccurring_transaction_calloc();

	reoccurring_transaction->full_name = full_name;
	reoccurring_transaction->street_address = street_address;

	reoccurring_transaction->transaction_description =
		transaction_description;

	return reoccurring_transaction;
}

REOCCURRING_STRUCTURE *reoccurring_structure_calloc( void )
{
	REOCCURRING_STRUCTURE *p =
		(REOCCURRING_STRUCTURE *)
			calloc( 1, sizeof( REOCCURRING_STRUCTURE ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;
}

REOCCURRING_TRANSACTION *reoccurring_transaction_calloc( void )
{
	REOCCURRING_TRANSACTION *p;

	if ( ! ( p = calloc( 1, sizeof( REOCCURRING_TRANSACTION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;
}

REOCCURRING_TRANSACTION *reoccurring_transaction_fetch(
			char *full_name,
			char *street_address,
			char *transaction_description )
{
	char sys_string[ 1024 ];

	if ( !full_name ) return (REOCCURRING_TRANSACTION *)0;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 reoccurring_transaction_select(),
		 "reoccurring_transaction",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 reoccurring_transaction_primary_where(
			full_name,
			street_address,
			transaction_description ) );

	return reoccurring_transaction_parse( pipe2string( sys_string ) );
}

char *reoccurring_transaction_select( void )
{
	return	"full_name,"
		"street_address,"
		"transaction_description,"
		"debit_account,"
		"credit_account,"
		"bank_upload_feeder_phrase,"
		"feeder_phrase_ignore_yn,"
		"accrued_daily_amount,"
		"accrued_monthly_amount";
}

char *reoccurring_escape_transaction_description(
			char *transaction_description )
{
	static char escape_transaction_description[ 256 ];

	return string_escape_quote(
		escape_transaction_description,
		transaction_description );
}

char *reoccurring_transaction_primary_where(
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

REOCCURRING_TRANSACTION *reoccurring_transaction_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_description[ 128 ];
	char buffer[ 128 ];
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( transaction_description, SQL_DELIMITER, input, 2 );

	reoccurring_transaction =
		reoccurring_transaction_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_description ) );

	piece( buffer, SQL_DELIMITER, input, 3 );
	reoccurring_transaction->debit_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input, 4 );
	reoccurring_transaction->credit_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input, 5 );
	reoccurring_transaction->bank_upload_feeder_phrase = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input, 6 );
	reoccurring_transaction->feeder_phrase_ignore = ( *buffer == 'y' );

	piece( buffer, FOLDER_DATA_DELIMITER, input, 7 );
	reoccurring_transaction->accrued_daily_amount = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input, 8 );
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

} /* reoccurring_memo() */

int reoccurring_days_between_last_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account )
{
	char sys_string[ 2048 ];
	int current_year;
	char where[ 1024 ];
	char sub_query[ 1024 ];
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

	reoccurring_transaction_subquery(
		sub_query,
		debit_account,
		credit_account );

	sprintf( where,
		 "full_name = '%s' and				"
		 "street_address = '%s' and			"
		 "%s						",
		 entity_escape_full_name( full_name ),
		 street_address,
		 sub_query );

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

void reoccurring_transaction_subquery(
			char *sub_query,
			char *debit_account,
			char *credit_account )
{
	sprintf( sub_query,
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

}

