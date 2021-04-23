/* -------------------------------------------------------------------- */
/* src_communityband/donation.c						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver donation ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "date.h"
#include "appaserver_library.h"
#include "ledger.h"
#include "donation.h"

DONATION_FUND *donation_fund_new( char *fund_name )
{
	DONATION_FUND *p =
		(DONATION_FUND *)
			calloc( 1, sizeof( DONATION_FUND ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	p->fund_name = fund_name;
	p->donation_account_list = list_new();
	return p;

}

DONATION_ACCOUNT *donation_account_new( void )
{
	DONATION_ACCOUNT *p =
		(DONATION_ACCOUNT *)
			calloc( 1, sizeof( DONATION_ACCOUNT ) );

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

DONATION *donation_new( void )
{
	DONATION *p =
		(DONATION *)
			calloc( 1, sizeof( DONATION ) );

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

char *donation_get_where(	char *full_name,
				char *street_address,
				char *donation_date )
{
	static char where[ 512 ];
	char buffer[ 256 ];

	sprintf( where,
		"full_name = '%s' and			"
	  	"street_address = '%s' and		"
	  	"donation_date = '%s'	 		",
		escape_character(	buffer,
					full_name,
					'\'' ),
		street_address,
		donation_date );

	return where;
}

DONATION *donation_fetch(
			char *application_name,
			char *full_name,
			char *street_address,
			char *donation_date )
{
	char *select;
	char sys_string[ 1024 ];
	char *where;
	char *results;
	DONATION *donation;

	select = donation_get_select();

	where = donation_get_where(
			full_name,
			street_address,
			donation_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=donation		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return (DONATION *)0;
	}

	donation = donation_parse( results );

	donation->donation_account_list =
		donation_fetch_donation_account_list(
			application_name,
			donation->full_name,
			donation->street_address,
			donation->donation_date );

	donation->donation_fund_list =
		donation_get_fund_list(
			application_name,
			donation->donation_account_list );

	if ( donation->transaction_date_time )
	{
		donation->transaction =
			ledger_transaction_fetch(
				application_name,
				donation->full_name,
				donation->street_address,
				donation->transaction_date_time );
	}

	return donation;

}

LIST *donation_fetch_donation_account_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *donation_date )
{
	DONATION_ACCOUNT *donation_account;
	char *select;
	LIST *donation_account_list;
	char sys_string[ 1024 ];
	char *where;
	FILE *input_pipe;
	char input_buffer[ 1024 ];

	select = donation_account_get_select();

	where = donation_get_where(
			full_name,
			street_address,
			donation_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=donation_account		"
		 "			where=\"%s\"			"
		 "			order=account			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	donation_account_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		donation_account =
			donation_account_parse(
				input_buffer );

		list_append_pointer( donation_account_list, donation_account );
	}

	pclose( input_pipe );
	return donation_account_list;

}

void donation_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *donation_date,
			double total_donation_amount,
			double database_total_donation_amount,
			char *transaction_date_time,
			char *database_transaction_date_time )
{
	char *sys_string;
	FILE *output_pipe;

	sys_string = donation_get_update_sys_string( application_name );
	output_pipe = popen( sys_string, "w" );

	if ( !dollar_virtually_same(
			total_donation_amount,
			database_total_donation_amount ) )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^total_donation_amount^%.2lf\n",
			full_name,
			street_address,
			donation_date,
			total_donation_amount );
	}

	if ( timlib_strcmp(	transaction_date_time,
				database_transaction_date_time ) != 0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^transaction_date_time^%s\n",
			full_name,
			street_address,
			donation_date,
			(transaction_date_time)
				? transaction_date_time
				: "" );
	}

	pclose( output_pipe );

}

DONATION_ACCOUNT *donation_account_parse(
			char *input_buffer )
{
	DONATION_ACCOUNT *donation_account;
	char piece_buffer[ 256 ];

	donation_account = donation_account_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	donation_account->account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	donation_account->donation_amount = atof( piece_buffer );

	return donation_account;

}

DONATION *donation_parse(
			char *input_buffer )
{
	DONATION *donation;
	char piece_buffer[ 256 ];

	donation = donation_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	donation->full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	donation->street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	donation->donation_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	donation->check_number = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	donation->total_donation_amount =
	donation->database_total_donation_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		donation->transaction_date_time =
		donation->database_transaction_date_time =
			strdup( piece_buffer );

	return donation;

}

char *donation_get_select( void )
{
	char *select =
"full_name,street_address,donation_date,check_number,total_donation_amount,transaction_date_time";
	return select;
}

char *donation_account_get_select( void )
{
	char *select = "account,donation_amount";
	return select;
}

char *donation_get_update_sys_string(
			char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name = get_table_name( application_name, "donation" );
	key = "full_name,street_address,donation_date";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

}

DONATION_ACCOUNT *donation_account_seek(
			char *account_name,
			LIST *donation_account_list )
{
	return donation_seek_donation_account(
			account_name,
			donation_account_list );
}

DONATION_ACCOUNT *donation_seek_donation_account(
			char *account_name,
			LIST *donation_account_list )
{
	DONATION_ACCOUNT *donation_account;

	if ( !list_rewind( donation_account_list ) )
		return (DONATION_ACCOUNT *)0;

	do {
		donation_account = list_get( donation_account_list );

		if ( strcmp(	donation_account->account_name,
				account_name ) == 0 )
		{
			return donation_account;
		}

	} while( list_next( donation_account_list ) );

	return (DONATION_ACCOUNT *)0;

}

double donation_get_total_donation_amount(
			LIST *donation_account_list )
{
	double total_donation_amount;
	DONATION_ACCOUNT *donation_account;

	if ( !list_rewind( donation_account_list ) )
		return 0.0;

	total_donation_amount = 0.0;

	do {
		donation_account = list_get( donation_account_list );
		total_donation_amount += donation_account->donation_amount;

	} while( list_next( donation_account_list ) );

	return total_donation_amount;

}

void donation_journal_ledger_refresh_and_propagate(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *donation_fund_list,
			boolean propagate_only )
{
	char *debit_account_name;
	DONATION_FUND *donation_fund;
	DONATION_ACCOUNT *donation_account;
	double total_transaction_donation_amount = 0.0;

	if ( !list_rewind( donation_fund_list ) ) return;

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	do {
		donation_fund = list_get_pointer( donation_fund_list );

		if ( !list_rewind( donation_fund->donation_account_list ) )
			continue;

		debit_account_name =
			ledger_get_hard_coded_account_name(
				application_name,
				donation_fund->fund_name,
				LEDGER_CASH_KEY,
				0 /* not warning_only */,
				__FUNCTION__ );

		if ( !propagate_only )
		{
			ledger_journal_ledger_insert(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				debit_account_name,
				donation_fund->total_fund_donation_amount,
				1 /* is_debit */ );

			total_transaction_donation_amount +=
				donation_fund->total_fund_donation_amount;
		}

		ledger_propagate(	application_name,
					transaction_date_time,
					debit_account_name );

		do {
			donation_account =
				list_get_pointer(
					donation_fund->
						donation_account_list );

			if ( !propagate_only )
			{
				ledger_journal_ledger_insert(
					application_name,
					full_name,
					street_address,
					transaction_date_time,
					donation_account->account_name,
					donation_account->donation_amount,
					0 /* not is_debit */ );
			}

			ledger_propagate(
				application_name,
				transaction_date_time,
				donation_account->account_name );

		} while( list_next( donation_fund->donation_account_list ) );

	} while( list_next( donation_fund_list ) );

	ledger_transaction_amount_update(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		total_transaction_donation_amount,
		0.0 /* database_transaction_amount */ );

}

LIST *donation_get_fund_list(
			char *application_name,
			LIST *donation_account_list )
{
	DONATION_ACCOUNT *donation_account;
	DONATION_FUND *donation_fund;
	LIST *donation_fund_list;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_length( donation_account_list ) ) return (LIST *)0;

	donation_fund_list = list_new();

	/* First, subclassification = donation */
	/* ----------------------------------- */
	subclassification = ledger_new_subclassification( "donation" );

	subclassification->account_list =
		ledger_subclassification_quickly_get_account_list(
			application_name,
			subclassification->subclassification_name );

	if ( list_rewind( subclassification->account_list ) )
	{
		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			donation_fund =
				donation_get_or_set_fund(
					donation_fund_list,
					account->fund_name );

			list_rewind( donation_account_list );

			do {
				donation_account =
					list_get_pointer(
						donation_account_list );

				if ( strcmp(	donation_account->account_name,
						account->account_name ) == 0 )
				{
					list_append_pointer(
						donation_fund->
							donation_account_list,
						donation_account );
					break;
				}

			} while( list_next( donation_account_list ) );

		} while( list_next( subclassification->account_list ) );
	}

	/* Second, subclassification = dues */
	/* -------------------------------- */
	subclassification = ledger_new_subclassification( "dues" );

	subclassification->account_list =
		ledger_subclassification_quickly_get_account_list(
			application_name,
			subclassification->subclassification_name );

	if ( list_rewind( subclassification->account_list ) )
	{
		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			donation_fund =
				donation_get_or_set_fund(
					donation_fund_list,
					account->fund_name );

			list_rewind( donation_account_list );

			do {
				donation_account =
					list_get_pointer(
						donation_account_list );

				if ( strcmp(	donation_account->account_name,
						account->account_name ) == 0 )
				{
					list_append_pointer(
						donation_fund->
							donation_account_list,
						donation_account );
					break;
				}

			} while( list_next( donation_account_list ) );

		} while( list_next( subclassification->account_list ) );
	}

	if ( list_length( donation_fund_list ) )
	{
		donation_fund_list_set_total_donation_amount(
			donation_fund_list );
	}

	return donation_fund_list;

}

DONATION_FUND *donation_get_or_set_fund(
			LIST *donation_fund_list,
			char *fund_name )
{
	DONATION_FUND *donation_fund;

	if ( !donation_fund_list )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: donation_fund_list is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( donation_fund_list ) )
	{
		donation_fund = donation_fund_new( fund_name );
		list_append_pointer( donation_fund_list, donation_fund );
		return donation_fund;
	}

	do {
		donation_fund = list_get_pointer( donation_fund_list );

		if ( strcmp( donation_fund->fund_name, fund_name ) == 0 )
			return donation_fund;

	} while( list_next( donation_fund_list ) );

	donation_fund = donation_fund_new( fund_name );
	list_append_pointer( donation_fund_list, donation_fund );
	return donation_fund;

}

void donation_fund_list_set_total_donation_amount(
			LIST *donation_fund_list )
{
	DONATION_FUND *donation_fund;
	DONATION_ACCOUNT *donation_account;

	if ( !list_rewind( donation_fund_list ) ) return;

	do {
		donation_fund = list_get_pointer( donation_fund_list );

		if ( !list_rewind( donation_fund->donation_account_list ) )
			continue;

		do {
			donation_account =
				list_get_pointer(
					donation_fund->donation_account_list );

			donation_fund->total_fund_donation_amount +=
				donation_account->donation_amount;

		} while( list_next( donation_fund->donation_account_list ) );

	} while( list_next( donation_fund_list ) );

}

