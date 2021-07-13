/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/investment.c		*/
/* ---------------------------------------------------- */
/* This is the appaserver investment ADT.		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "investment.h"

INVESTMENT_EQUITY *investment_equity_new(
					char *application_name,
					char *full_name,
					char *street_address,
					char *fund_name,
					char *account_number,
					char *date_time,
					char *state,
					char *preupdate_full_name,
					char *preupdate_street_address,
					char *preupdate_account_number,
					char *preupdate_date_time )
{
	INVESTMENT_EQUITY *t;
	ACCOUNT_BALANCE *first_account_balance;

	if ( ! ( t = calloc( 1, sizeof( INVESTMENT_EQUITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->investment_account.full_name = full_name;
	t->investment_account.street_address = street_address;
	t->investment_account.account_number = account_number;

	t->input.date_time = date_time;
	t->input.fund_name = fund_name;
	t->input.state = state;

	t->input.preupdate_full_name = preupdate_full_name;
	t->input.preupdate_street_address = preupdate_street_address;
	t->input.preupdate_account_number = preupdate_account_number;
	t->input.preupdate_date_time = preupdate_date_time;

	if ( !investment_fetch_account(
		&t->investment_account.investment_account,
		&t->investment_account.fair_value_adjustment_account,
		&t->investment_account.classification,
		application_name,
		t->investment_account.full_name,
		t->investment_account.street_address,
		t->investment_account.account_number ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot fetch from %s (%s,%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 INVESTMENT_ACCOUNT_FOLDER_NAME,
			 t->investment_account.full_name,
			 t->investment_account.street_address,
			 t->investment_account.account_number );
		exit( 1 );
	}

	t->process = investment_process_new(
			t->investment_account.full_name,
			t->investment_account.street_address,
			t->investment_account.account_number,
			t->input.date_time,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_account_number,
			preupdate_date_time );

	if ( t->process->earlier_date_time
	&&   *t->process->earlier_date_time )
	{
		t->process->begin_date_time =
			investment_account_balance_fetch_prior_date_time(
				application_name,
				t->investment_account.full_name,
				t->investment_account.street_address,
				t->investment_account.account_number,
				t->process->earlier_date_time );
	}

	if ( ! ( t->input_account_balance_list =
			investment_fetch_account_balance_list(
				application_name,
				t->investment_account.full_name,
				t->investment_account.street_address,
				t->investment_account.account_number,
				t->process->begin_date_time ) )
	||   !list_length( t->input_account_balance_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot fetch from EQUITY_ACCOUNT_BALANCE (%s,%s,%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 t->investment_account.full_name,
			 t->investment_account.street_address,
			 t->investment_account.account_number,
			 t->input.date_time );
		exit( 1 );
	}

	first_account_balance =
		list_get_first_pointer(
			t->input_account_balance_list );

	first_account_balance->table_first_row =
		investment_get_account_balance_table_first_row(
			application_name,
			t->investment_account.full_name,
			t->investment_account.street_address,
			t->investment_account.account_number,
			t->input.date_time );

	return t;

} /* investment_equity_new() */

INVESTMENT_PROCESS *investment_process_new(
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time,
				char *preupdate_full_name,
				char *preupdate_street_address,
				char *preupdate_account_number,
				char *preupdate_date_time )
{
	INVESTMENT_PROCESS *p;

	if ( ! ( p = calloc( 1, sizeof( INVESTMENT_PROCESS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	p->date_time_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_date_time,
			date_time /* postupdate_data */,
			"preupdate_date_time" );

	if (	p->date_time_change_state ==
		from_something_to_something_else )
	{
		p->earlier_date_time =
			ledger_earlier_of_two_date(
				date_time
					/* date1 */,
				preupdate_date_time
					/* date2 */ );
	}
	else
	{
		p->earlier_date_time = date_time;
	}

	p->full_name_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_full_name,
			full_name /* postupdate_data */,
			"preupdate_full_name" );

	p->street_address_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_street_address,
			street_address /* postupdate_data */,
			"preupdate_street_address" );

	p->account_number_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_account_number,
			account_number /* postupdate_data */,
			"preupdate_account_number" );

	return p;

} /* investment_process_new() */

ACCOUNT_BALANCE *investment_account_balance_new(
					char *date_time )
{
	ACCOUNT_BALANCE *t;

	if ( ! ( t = calloc( 1, sizeof( ACCOUNT_BALANCE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->date_time = date_time;
	return t;

} /* investment_account_balance_new() */

char *investment_account_balance_get_select( void )
{
	char *select =
		"date_time,			"
		"investment_operation,		"
		"share_price,			"
		"share_quantity_change,		"
		"share_quantity_balance,	"
		"book_value_change,		"
		"book_value_balance,		"
		"moving_share_price,		"
		"cash_in,			"
		"market_value,			"
		"unrealized_gain_balance,	"
		"unrealized_gain_change,	"
		"realized_gain,			"
		"transaction_date_time		";

	return select;

} /* investment_account_balance_get_select() */

LIST *investment_fetch_account_balance_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *begin_date_time )
{
	LIST *account_balance_list;
	char sys_string[ 1024 ];
	char begin_date_time_where[ 128 ];
	char where[ 512 ];
	char input_buffer[ 512 ];
	char buffer[ 128 ];
	char *select;
	ACCOUNT_BALANCE *account_balance;
	FILE *input_pipe;

	select = investment_account_balance_get_select();

	if ( begin_date_time && *begin_date_time )
	{
		sprintf( begin_date_time_where,
			 "date_time >= '%s'",
			 begin_date_time );
	}
	else
	{
		strcpy( begin_date_time_where, "1 = 1" );
	}

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "account_number = '%s' and		"
		 "%s 					",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 account_number,
		 begin_date_time_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	"
		 "			order=date_time	",
		 application_name,
		 select,
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 where );

	input_pipe = popen( sys_string, "r" );

	account_balance_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		account_balance =
			investment_account_balance_parse(
				input_buffer );

		list_append_pointer( account_balance_list, account_balance );
	}

	pclose( input_pipe );

	return account_balance_list;

} /* investment_fetch_account_balance_list() */

boolean investment_fetch_account_balance(
					double *share_price,
					double *share_quantity_change,
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char buffer[ 128 ];
	char *select;
	char *results;

	select = "share_price,share_quantity_change";

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "account_number = '%s' and 		"
		 "date_time = '%s'			",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 account_number,
		 date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece( buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*share_price = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*share_quantity_change = atof( buffer );

	return 1;

} /* investment_fetch_account_balance() */

boolean investment_fetch_account(	char **investment_account,
					char **fair_value_adjustment_account,
					char **classification,
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char buffer[ 128 ];
	char *select;
	char *results;

	select =
	"investment_account,fair_value_adjustment_account,classification";

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "account_number = '%s' 		",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 account_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 INVESTMENT_ACCOUNT_FOLDER_NAME,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece( buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*investment_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*fair_value_adjustment_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*classification = strdup( buffer );

	return 1;

} /* investment_fetch_account() */

ACCOUNT_BALANCE *investment_account_balance_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char buffer[ 128 ];
	char *select;
	ACCOUNT_BALANCE *account_balance;
	char *input_buffer;

	select = investment_account_balance_get_select();

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and		"
		 "account_number = '%s' and		"
		 "date_time = '%s' 			",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 account_number,
		 date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 where );

	if ( ! ( input_buffer = pipe2string( sys_string ) ) )
		return (ACCOUNT_BALANCE *)0;

	account_balance =
		investment_account_balance_parse(
			input_buffer );

	account_balance->transaction =
		ledger_transaction_fetch(
			application_name,
			full_name,
			street_address,
			account_balance->transaction_date_time );

	return account_balance;

} /* investment_account_balance_fetch() */

ACCOUNT_BALANCE *investment_account_balance_parse(
				char *input_buffer )
{
	ACCOUNT_BALANCE *account_balance;
	char buffer[ 128 ];
	char *date_time;

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	date_time = strdup( buffer );

	account_balance = investment_account_balance_new( date_time );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	account_balance->investment_operation = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	account_balance->share_price = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	account_balance->share_quantity_change = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	account_balance->share_quantity_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	account_balance->book_value_change = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	account_balance->book_value_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	account_balance->moving_share_price = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	account_balance->cash_in = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	account_balance->market_value = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
	account_balance->unrealized_gain_balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
	account_balance->unrealized_gain_change = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );
	account_balance->realized_gain = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 13 );
	account_balance->transaction_date_time = strdup( buffer );

	return account_balance;

} /* investment_account_balance_parse() */

char *investment_account_balance_fetch_prior_date_time(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time )
{
	char buffer[ 128 ];
	char sys_string[ 1024 ];
	char where[ 256 ];
	char *select;
	char *folder;
	char *results;

	select = "max( date_time )";

	folder = ACCOUNT_BALANCE_FOLDER_NAME;

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "account_number = '%s' and		"
		 "date_time < '%s'			",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 account_number,
		 date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( results && !*results )
		return (char *)0;
	else
		return results;

} /* investment_account_balance_fetch_prior_date_time() */

ACCOUNT_BALANCE *investment_account_balance_sale(
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_book_value_balance,
					double prior_moving_share_price,
					double prior_unrealized_gain_balance )
{
	ACCOUNT_BALANCE *a;

	a = investment_account_balance_new( date_time );

	a->share_price = share_price;
	a->share_quantity_change = share_quantity_change;

	a->share_quantity_balance =
		prior_share_quantity_balance +
		a->share_quantity_change;

	a->cash_in = a->share_price * ( 0.0 - a->share_quantity_change );
	a->market_value = a->share_price * a->share_quantity_balance;
	a->moving_share_price = prior_moving_share_price;
	a->book_value_change = a->moving_share_price * a->share_quantity_change;

	a->book_value_balance =
		prior_book_value_balance +
		a->book_value_change;

	a->unrealized_gain_balance =
		a->market_value -
		a->book_value_balance;

	a->unrealized_gain_change =
		a->unrealized_gain_balance -
		prior_unrealized_gain_balance;

	a->realized_gain = a->cash_in + a->book_value_change;

	return a;

} /* investment_account_balance_sale() */

ACCOUNT_BALANCE *investment_account_balance_time_passage(
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_book_value_balance,
					double prior_unrealized_gain_balance )
{
	ACCOUNT_BALANCE *a;

	a = investment_account_balance_new( date_time );

	a->share_price = share_price;
	a->share_quantity_change = share_quantity_change;

	a->share_quantity_balance =
		prior_share_quantity_balance +
		a->share_quantity_change;

	a->market_value = a->share_price * a->share_quantity_balance;
	a->book_value_change = a->share_price * a->share_quantity_change;
	a->book_value_balance = prior_book_value_balance + a->book_value_change;

	if ( a->share_quantity_balance )
	{
		a->moving_share_price =
			a->book_value_balance /
			a->share_quantity_balance;
	}

	a->unrealized_gain_balance =
		a->market_value -
		a->book_value_balance;

	a->unrealized_gain_change =
		a->unrealized_gain_balance -
		prior_unrealized_gain_balance;

	if ( !timlib_double_virtually_same(
		a->share_quantity_change, 0.0 ) )
	{
		a->realized_gain = a->book_value_change;
	}

	return a;

} /* investment_account_balance_time_passage() */

ACCOUNT_BALANCE *investment_account_balance_purchase(
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_book_value_balance,
					double prior_unrealized_gain_balance,
					boolean table_first_row )
{
	ACCOUNT_BALANCE *a;

	a = investment_account_balance_new( date_time );

	a->share_price = share_price;
	a->share_quantity_change = share_quantity_change;

	if ( table_first_row )
	{
		a->share_quantity_balance = a->share_quantity_change;
	}
	else
	{
		a->share_quantity_balance =
			prior_share_quantity_balance +
			a->share_quantity_change;
	}

	a->cash_in = a->share_price * ( 0.0 - a->share_quantity_change );
	a->market_value = a->share_price * a->share_quantity_balance;
	a->book_value_change = a->share_price * a->share_quantity_change;

	if ( table_first_row )
	{
		a->book_value_balance = a->book_value_change;
	}
	else
	{
		a->book_value_balance =
			prior_book_value_balance +
			a->book_value_change;
	}

	if ( a->share_quantity_balance )
	{
		a->moving_share_price =
			a->book_value_balance /
			a->share_quantity_balance;
	}

	if ( table_first_row )
	{
		a->unrealized_gain_balance = 0.0;
	}
	else
	{
		a->unrealized_gain_balance =
			a->market_value -
			a->book_value_balance;

		a->unrealized_gain_change =
			a->unrealized_gain_balance -
			prior_unrealized_gain_balance;
	}

	return a;

} /* investment_account_balance_purchase() */

LIST *investment_calculate_account_balance_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *fund_name,
				char *investment_account,
				char *fair_value_adjustment_account,
				LIST *input_account_balance_list )
{
	ACCOUNT_BALANCE *account_balance;
	ACCOUNT_BALANCE *prior_account_balance = {0};
	ACCOUNT_BALANCE *new_account_balance;
	LIST *output_account_balance_list;

	if ( !list_rewind( input_account_balance_list ) ) return (LIST *)0;

	output_account_balance_list = list_new();

	do {
		account_balance =
			list_get_pointer(
				input_account_balance_list );

		/* ---------------------------------------------------- */
		/* First item is either the very first			*/
		/* purchase/transfer transaction or the prior		*/
		/* transaction to the one being inserted or changed.	*/
		/* ---------------------------------------------------- */
		if ( list_at_first( input_account_balance_list ) )
		{
			if ( account_balance->table_first_row )
			{
				new_account_balance =
			   	investment_account_balance_calculate(
					account_balance->date_time,
					account_balance->share_price,
					account_balance->share_quantity_change,
					0.0 /* prior_share_quantity_balance */,
					0.0 /* prior_book_value_balance */,
					0.0 /* prior_moving_share_price */,
					0.0 /* prior_unrealized_gain_balance */,
					account_balance->investment_operation,
					1 /* table_first_row */ );
			}
			else
			{
				new_account_balance = account_balance;
			}
		}
		else
		{
			new_account_balance =
			   investment_account_balance_calculate(
				account_balance->date_time,
				account_balance->share_price,
				account_balance->share_quantity_change,
				prior_account_balance->share_quantity_balance,
				prior_account_balance->book_value_balance,
				prior_account_balance->moving_share_price,
				prior_account_balance->
					unrealized_gain_balance,
				account_balance->investment_operation,
				0 /* not table_first_row */ );
		}

		if ( !new_account_balance )
		{
			fprintf( stderr,
	"Warning in %s/%s()/%d: cannot calculate investment_operation = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 account_balance->investment_operation );

			printf( "<h3>An error occurred. Check log.</h3>\n" );
			continue;
		}

		list_append_pointer(
			output_account_balance_list,
			new_account_balance );

		prior_account_balance = new_account_balance;

		new_account_balance->transaction =
			investment_build_transaction(
				application_name,
				full_name,
				street_address,
				account_balance->date_time,
				fund_name,
				new_account_balance->investment_operation,
				investment_account,
				fair_value_adjustment_account,
				new_account_balance->share_quantity_change,
				new_account_balance->book_value_change,
				new_account_balance->unrealized_gain_change,
				new_account_balance->realized_gain );

		/* ----------------------------------------------------- */
		/* Note: new_account_balance->transaction_date_time gets */
		/*       set from ledger_transaction_refresh()		 */
		/* ----------------------------------------------------- */

	} while( list_next( input_account_balance_list ) );

	return output_account_balance_list;

} /* investment_calculate_account_balance_list() */

ACCOUNT_BALANCE *investment_account_balance_calculate(
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_book_value_balance,
					double prior_moving_share_price,
					double prior_unrealized_gain_balance,
					char *investment_operation,
					boolean table_first_row )
{
	ACCOUNT_BALANCE *a = {0};

	if ( timlib_strcmp(
		investment_operation,
		INVESTMENT_OPERATION_TRANSFER ) == 0
	&&   !timlib_double_virtually_same( share_quantity_change, 0.0 ) )
	{
		if ( share_quantity_change > 0.0 )
		{
			a = investment_account_balance_purchase(
				date_time,
				share_price,
				share_quantity_change,
				prior_share_quantity_balance,
				prior_book_value_balance,
				prior_unrealized_gain_balance,
				table_first_row );
		}
		else
		{
			a = investment_account_balance_sale(
				date_time,
				share_price,
				share_quantity_change,
				prior_share_quantity_balance,
				prior_book_value_balance,
				prior_moving_share_price,
				prior_unrealized_gain_balance );
		}

		if ( timlib_double_virtually_same( a->book_value_change, 0.0 ) )
		{
			return (ACCOUNT_BALANCE *)0;
		}

		a->cash_in = 0.0;
	}
	else
	if ( timlib_strcmp(
		investment_operation,
		INVESTMENT_OPERATION_PURCHASE ) == 0 )
	{
		a = investment_account_balance_purchase(
				date_time,
				share_price,
				share_quantity_change,
				prior_share_quantity_balance,
				prior_book_value_balance,
				prior_unrealized_gain_balance,
				table_first_row );

		if ( timlib_double_virtually_same( a->book_value_change, 0.0 ) )
		{
			return (ACCOUNT_BALANCE *)0;
		}

	}
	else
	if ( timlib_strcmp(
		investment_operation,
		INVESTMENT_OPERATION_TIME_PASSAGE ) == 0 )
	{
		a = investment_account_balance_time_passage(
				date_time,
				share_price,
				share_quantity_change,
				prior_share_quantity_balance,
				prior_book_value_balance,
				prior_unrealized_gain_balance );
	}
	else
	if ( timlib_strcmp(
		investment_operation,
		INVESTMENT_OPERATION_SALE ) == 0 )
	{
		a = investment_account_balance_sale(
				date_time,
				share_price,
				share_quantity_change,
				prior_share_quantity_balance,
				prior_book_value_balance,
				prior_moving_share_price,
				prior_unrealized_gain_balance );
	}

	if ( a )
	{
		a->investment_operation = investment_operation;
	}

	return a;

} /* investment_account_balance_calculate() */

FILE *investment_open_update_pipe( void )
{
	char *key_column_list;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	key_column_list =
	"full_name,street_address,account_number,date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y	|"
		 "sql.e						 ",
		 ACCOUNT_BALANCE_FOLDER_NAME,
		 key_column_list );

	output_pipe = popen( sys_string, "w" );

	return output_pipe;

} /* investment_open_update_pipe() */

void investment_account_balance_list_update(
					LIST *output_account_balance_list,
					char *application_name,
					LIST *input_account_balance_list,
					char *full_name,
					char *street_address,
					char *account_number )
{
	FILE *output_pipe;
	ACCOUNT_BALANCE *account_balance;
	ACCOUNT_BALANCE *output_account_balance;

	if ( !full_name
	||   !street_address
	||   !account_number )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: received null input.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_length( input_account_balance_list ) ) return;

	if ( list_length( input_account_balance_list ) !=
	     list_length( output_account_balance_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: length = %d <> length = %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 list_length( input_account_balance_list ),
	     		 list_length( output_account_balance_list ) );
		exit( 1 );
	}

	output_pipe = investment_open_update_pipe();

	list_rewind( input_account_balance_list );
	list_rewind( output_account_balance_list );

	do {
		account_balance =
			list_get_pointer(
				input_account_balance_list );

		output_account_balance =
			list_get_pointer(
				output_account_balance_list );

		investment_account_balance_update(
			output_pipe,
			output_account_balance,
			application_name,
			account_balance
				/* input_account_balance */,
			full_name,
			street_address,
			account_number );

		list_next( output_account_balance_list );

	} while( list_next( input_account_balance_list ) );

	pclose( output_pipe );

} /* investment_account_balance_list_update() */

void investment_account_balance_update(	FILE *output_pipe,
					ACCOUNT_BALANCE *output_account_balance,
					char *application_name,
					ACCOUNT_BALANCE *input_account_balance,
					char *full_name,
					char *street_address,
					char *account_number )
{
	char *transaction_date_time;

	if ( !output_account_balance
	||   !input_account_balance )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: received null input.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		pclose( output_pipe );
		exit( 1 );
	}

	if ( timlib_strcmp(
		output_account_balance->investment_operation,
		input_account_balance->investment_operation ) != 0 )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^investment_operation^%s\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->investment_operation );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->share_price,
			input_account_balance->share_price ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^share_price^%.4lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->share_price );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->share_quantity_change,
			input_account_balance->share_quantity_change ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^share_quantity_change^%.4lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->share_quantity_change );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->share_quantity_balance,
			input_account_balance->share_quantity_balance ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^share_quantity_balance^%.4lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->share_quantity_balance );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->book_value_change,
			input_account_balance->book_value_change ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^book_value_change^%.4lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->book_value_change );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->book_value_balance,
			input_account_balance->book_value_balance ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^book_value_balance^%.4lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->book_value_balance );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->moving_share_price,
			input_account_balance->moving_share_price ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^moving_share_price^%.4lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->moving_share_price );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->cash_in,
			input_account_balance->cash_in ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^cash_in^%.2lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->cash_in );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->market_value,
			input_account_balance->market_value ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^market_value^%.4lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->market_value );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->unrealized_gain_balance,
			input_account_balance->unrealized_gain_balance ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^unrealized_gain_balance^%.4lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->unrealized_gain_balance );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->unrealized_gain_change,
			input_account_balance->unrealized_gain_change ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^unrealized_gain_change^%.4lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->unrealized_gain_change );
	}

	if ( !timlib_double_virtually_same(
			output_account_balance->realized_gain,
			input_account_balance->realized_gain ) )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^realized_gain^%.4lf\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		output_account_balance->realized_gain );
	}

	/* If no transaction */
	/* ----------------- */
	if ( !output_account_balance->transaction )
	{
		output_account_balance->transaction_date_time = (char *)0;

		/* If was a transaction but no longer one. */
		/* --------------------------------------- */
		if ( input_account_balance->transaction_date_time
		&&   *input_account_balance->transaction_date_time )
		{
			LIST *account_name_list;
			char *account_name;

			ledger_delete(	application_name,
					TRANSACTION_FOLDER_NAME,
					full_name,
					street_address,
					input_account_balance->
						transaction_date_time );

			ledger_delete(	application_name,
					LEDGER_FOLDER_NAME,
					full_name,
					street_address,
					input_account_balance->
						transaction_date_time );

			/* Propagate */
			/* --------- */
			account_name_list =
				ledger_get_unique_account_name_list(
					input_account_balance->
						transaction->
						journal_ledger_list );

			if ( list_rewind( account_name_list ) )
			{
				do {
					account_name =
						list_get_pointer(
							account_name_list );

					ledger_propagate(
						application_name,
						input_account_balance->
							transaction->
							transaction_date_time,
						account_name );

				} while( list_next( account_name_list ) );
			}

		} /* if transaction_date_time */
	}
	else
	/* -------------------- */
	/* Else has transaction */
	/* -------------------- */
	{
		output_account_balance->transaction_date_time =
			ledger_transaction_refresh(
				application_name,
				full_name,
				street_address,
				output_account_balance->
					transaction->
						transaction_date_time,
				output_account_balance->
					transaction->
					transaction_amount,
				investment_get_memo(
					output_account_balance->
						investment_operation ),
				0 /* check_number */,
				1 /* lock_transaction */,
				output_account_balance->
					transaction->
					journal_ledger_list );
	}

	if ( timlib_strcmp(
			output_account_balance->transaction_date_time,
			input_account_balance->transaction_date_time ) != 0 )
	{
		transaction_date_time =
	 		output_account_balance->
				transaction_date_time;

		fprintf(output_pipe,
			"%s^%s^%s^%s^transaction_date_time^%s\n",
	 		full_name,
	 		street_address,
	 		account_number,
	 		output_account_balance->date_time,
	 		(transaction_date_time)
				? transaction_date_time
				: "" );
	}

} /* investment_account_balance_update() */

char *investment_get_memo( char *investment_operation )
{
	char *memo;

	if ( strcmp(	investment_operation,
			INVESTMENT_OPERATION_TRANSFER ) == 0 )
		memo = "Investment Transfer";
	else
	if ( strcmp(	investment_operation,
			INVESTMENT_OPERATION_TIME_PASSAGE ) == 0 )
		memo = "Investment Time Passage";
	else
	if ( strcmp(	investment_operation,
			INVESTMENT_OPERATION_PURCHASE ) == 0 )
		memo = "Purchase Investment";
	else
	if ( strcmp(	investment_operation,
			INVESTMENT_OPERATION_SALE ) == 0 )
		memo = "Sell Investment";
	else
		memo = "";

	return memo;

} /* investment_get_memo() */

TRANSACTION *investment_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *date_time,
				char *fund_name,
				char *investment_operation,
				char *investment_account,
				char *fair_value_adjustment_account,
				double share_quantity_change,
				double book_value_change,
				double unrealized_gain_change,
				double realized_gain )
{
	TRANSACTION *transaction;

	if ( !investment_account || !*investment_account  )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: empty investment_account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (TRANSACTION *)0;
	}


	if ( timlib_strcmp(
			investment_operation,
			INVESTMENT_OPERATION_TRANSFER ) == 0 )
	{
		char *cash_equity_account;

		cash_equity_account =
			investment_fetch_cash_or_equity_account_name(
				application_name,
				fund_name,
				INVESTMENT_OPERATION_TRANSFER );

		if ( !cash_equity_account || !*cash_equity_account )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot fetch credit account name for operation = %s.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 INVESTMENT_OPERATION_TRANSFER );
			exit( 1 );
		}

		if ( share_quantity_change > 0.0 )
		{
			transaction =
				investment_build_purchase_transaction(
					application_name,
					full_name,
					street_address,
					date_time,
					fund_name,
					investment_operation,
					investment_account,
					fair_value_adjustment_account,
					cash_equity_account,
					share_quantity_change,
					book_value_change,
					unrealized_gain_change );
		}
		else
		{
			transaction =
				investment_build_sale_transaction(
					application_name,
					full_name,
					street_address,
					date_time,
					fund_name,
					investment_operation,
					investment_account,
					cash_equity_account
					   /* fair_value_adjustment_account */,
					unrealized_gain_change,
					realized_gain,
					book_value_change );
		}
	}
	else
	if ( timlib_strcmp(
			investment_operation,
			INVESTMENT_OPERATION_PURCHASE ) == 0 )
	{
		char *credit_account;

		credit_account =
			investment_fetch_cash_or_equity_account_name(
				application_name,
				fund_name,
				INVESTMENT_OPERATION_PURCHASE );

		transaction =
			investment_build_purchase_transaction(
				application_name,
				full_name,
				street_address,
				date_time,
				fund_name,
				investment_operation,
				investment_account,
				fair_value_adjustment_account,
				credit_account,
				share_quantity_change,
				book_value_change,
				unrealized_gain_change );
	}
	else
	if ( timlib_strcmp(
			investment_operation,
			INVESTMENT_OPERATION_TIME_PASSAGE ) == 0 )
	{
		transaction =
			investment_build_time_passage_transaction(
				application_name,
				full_name,
				street_address,
				date_time,
				fund_name,
				investment_operation,
				investment_account,
				fair_value_adjustment_account,
				unrealized_gain_change,
				realized_gain );
	}
	else
	if ( timlib_strcmp(
			investment_operation,
			INVESTMENT_OPERATION_SALE ) == 0 )
	{
		transaction =
			investment_build_sale_transaction(
				application_name,
				full_name,
				street_address,
				date_time,
				fund_name,
				investment_operation,
				investment_account,
				fair_value_adjustment_account,
				unrealized_gain_change,
				realized_gain,
				book_value_change );
	}
	else
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: unrecognized investment_operation = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 investment_operation );
		exit( 1 );
	}

	if ( !transaction
	||   !list_length( transaction->journal_ledger_list ) )
	{
		return (TRANSACTION *)0;
	}

	transaction->journal_ledger_list =
		ledger_consolidate_journal_ledger_list(
			application_name,
			transaction->journal_ledger_list );

	return transaction;

} /* investment_build_transaction() */

TRANSACTION *investment_build_purchase_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *date_time,
				char *fund_name,
				char *investment_operation,
				char *investment_account,
				char *fair_value_adjustment_account,
				char *credit_account,
				double share_quantity_change,
				double book_value_change,
				double unrealized_gain_change )
{
	TRANSACTION *transaction;
	char *unrealized_investment = {0};
	char *realized_gain = {0};
	char *realized_loss = {0};
	char *checking_account = {0};
	char *contributed_capital_account = {0};

	if ( !credit_account || !*credit_account )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: empty credit_account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (TRANSACTION *)0;
	}

	if ( !investment_account || !*investment_account )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: empty investment_account (debit_account).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (TRANSACTION *)0;
	}

	if ( !fair_value_adjustment_account || !*fair_value_adjustment_account )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: empty fair_value_adjustment_account (debit_account).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (TRANSACTION *)0;
	}

	/* if ( share_quantity_change <= 0.0 ) return (TRANSACTION *)0; */

	if ( timlib_double_virtually_same( share_quantity_change, 0.0 ) )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: empty share_quantity_change.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (TRANSACTION *)0;
	}

	if ( timlib_double_virtually_same( book_value_change, 0.0 ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: empty book_value_change.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (TRANSACTION *)0;
	}

	ledger_get_investment_account_names(
		&unrealized_investment,
		&realized_gain,
		&realized_loss,
		&checking_account,
		&contributed_capital_account,
		application_name,
		fund_name );

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			date_time /* transaction_date_time */,
			investment_get_memo( investment_operation ) );

	transaction->transaction_amount = book_value_change;

	transaction->journal_ledger_list =
		ledger_get_binary_ledger_list(
			transaction->transaction_amount,
			investment_account /* debit_account */,
			credit_account );

	if ( unrealized_gain_change > 0.0 )
	{
		list_append_list(
			transaction->journal_ledger_list,
			ledger_get_binary_ledger_list(
				unrealized_gain_change
					/* transaction_amount */,
				fair_value_adjustment_account
					/* debit_account */,
				unrealized_investment
					/* credit_account */ ) );
	}
	else
	if ( unrealized_gain_change < 0.0 )
	{
		list_append_list(
			transaction->journal_ledger_list,
			ledger_get_binary_ledger_list(
				0.0 - unrealized_gain_change
					/* transaction_amount */,
				unrealized_investment
					/* debit_account */,
				fair_value_adjustment_account
					/* credit_account */ ) );
	}

	return transaction;

} /* investment_build_purchase_transaction() */

TRANSACTION *investment_build_sale_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *date_time,
				char *fund_name,
				char *investment_operation,
				char *investment_account,
				char *fair_value_adjustment_account,
				double unrealized_gain_change,
				double realized_gain,
				double book_value_change )
{
	TRANSACTION *transaction;
	char *unrealized_investment = {0};
	char *realized_gain_account = {0};
	char *realized_loss = {0};
	char *checking_account = {0};
	char *contributed_capital_account = {0};

	if ( !investment_account || !*investment_account )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: empty investment_account (debit_account).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (TRANSACTION *)0;
	}

	if ( !fair_value_adjustment_account || !*fair_value_adjustment_account )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: empty fair_value_adjustment_account (debit_account).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (TRANSACTION *)0;
	}

/* Now could be a transfer to or from an equity account
   ----------------------------------------------------
	if ( timlib_double_virtually_same( cash_in, 0.0 ) )
	{
		return (TRANSACTION *)0;
	}
*/

	ledger_get_investment_account_names(
		&unrealized_investment,
		&realized_gain_account,
		&realized_loss,
		&checking_account,
		&contributed_capital_account,
		application_name,
		fund_name );

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			date_time /* transaction_date_time */,
			investment_get_memo( investment_operation ) );

	transaction->transaction_amount = float_abs( book_value_change );
	
	transaction->journal_ledger_list =
		ledger_get_binary_ledger_list(
			transaction->transaction_amount,
			checking_account
				/* debit_account */,
			investment_account /* credit_account */ );

	if ( !timlib_double_virtually_same(
		unrealized_gain_change, 0.0 ) )
	{
		if ( unrealized_gain_change > 0.0 )
		{
			list_append_list(
				transaction->journal_ledger_list,
				ledger_get_binary_ledger_list(
					unrealized_gain_change,
					fair_value_adjustment_account
						/* debit_account */,
					unrealized_investment
						/* credit_account */ ) );
		}
		else
		{
			list_append_list(
				transaction->journal_ledger_list,
				ledger_get_binary_ledger_list(
					0.0 - unrealized_gain_change,
					unrealized_investment
						/* credit_account */,
					fair_value_adjustment_account
						/* debit_account */ ) );
		}
	}

	if ( !timlib_double_virtually_same( realized_gain, 0.0 ) )
	{
		if ( realized_gain > 0.0 )
		{
			list_append_list(
				transaction->journal_ledger_list,
				ledger_get_binary_ledger_list(
					realized_gain,
					investment_account
						/* debit_account */,
					realized_gain_account
						/* credit_account */ ) );
		}
		else
		{
			list_append_list(
				transaction->journal_ledger_list,
				ledger_get_binary_ledger_list(
					0.0 - realized_gain,
					realized_loss
						/* credit_account */,
					investment_account
						/* debit_account */ ) );
		}
	}

	return transaction;

} /* investment_build_sale_transaction() */

TRANSACTION *investment_build_time_passage_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *date_time,
				char *fund_name,
				char *investment_operation,
				char *investment_account,
				char *fair_value_adjustment_account,
				double unrealized_gain_change,
				double realized_gain )
{
	TRANSACTION *transaction;
	char *unrealized_investment = {0};
	char *realized_gain_account = {0};
	char *realized_loss = {0};
	char *checking_account = {0};
	char *contributed_capital_account = {0};

	if ( !investment_account || !*investment_account )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: empty investment_account (debit_account).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (TRANSACTION *)0;
	}

	if ( !fair_value_adjustment_account || !*fair_value_adjustment_account )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: empty fair_value_adjustment_account (debit_account).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (TRANSACTION *)0;
	}

	if ( timlib_double_virtually_same(
		unrealized_gain_change, 0.0 )
	&&   timlib_double_virtually_same(
		realized_gain, 0.0 ) )
	{
		return (TRANSACTION *)0;
	}

	ledger_get_investment_account_names(
		&unrealized_investment,
		&realized_gain_account,
		&realized_loss,
		&checking_account,
		&contributed_capital_account,
		application_name,
		fund_name );

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			date_time /* transaction_date_time */,
			investment_get_memo( investment_operation ) );

	if ( !timlib_double_virtually_same(
		unrealized_gain_change, 0.0 ) )
	{
		if ( unrealized_gain_change > 0.0 )
		{
			transaction->transaction_amount =
				unrealized_gain_change;
	
			transaction->journal_ledger_list =
				ledger_get_binary_ledger_list(
					transaction->transaction_amount,
					fair_value_adjustment_account
						/* debit_account */,
					unrealized_investment
						/* credit_account */ );
		}
		else
		{
			transaction->transaction_amount =
				0.0 - unrealized_gain_change;
	
			transaction->journal_ledger_list =
				ledger_get_binary_ledger_list(
					transaction->transaction_amount,
					unrealized_investment
						/* credit_account */,
					fair_value_adjustment_account
						/* debit_account */ );
		}
	}

	if ( !timlib_double_virtually_same(
		realized_gain, 0.0 ) )
	{
		if ( !transaction->journal_ledger_list )
		{
			transaction->journal_ledger_list = list_new();
		}
	
		if ( realized_gain > 0.0 )
		{
			if ( !transaction->transaction_amount )
			{
				transaction->transaction_amount = realized_gain;
			}
		
			list_append_list(
				transaction->journal_ledger_list,
				ledger_get_binary_ledger_list(
					realized_gain,
					investment_account
						/* debit_account */,
					realized_gain_account
						/* credit_account */ ) );
		}
		else
		{
			if ( !transaction->transaction_amount )
			{
				transaction->transaction_amount =
					0.0 - realized_gain;
			}
		
			list_append_list(
				transaction->journal_ledger_list,
				ledger_get_binary_ledger_list(
					0.0 - realized_gain,
					realized_loss
						/* debit_account */,
					investment_account
						/* credit_account */ ) );
		}
	}

	return transaction;

} /* investment_build_time_passage_transaction() */

void investment_transaction_date_time_update(
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time,
				char *transaction_date_time,
				char *application_name )
{
	char *sys_string;
	FILE *output_pipe;
	char buffer[ 128 ];

	sys_string = investment_get_update_sys_string( application_name );
	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^%s^transaction_date_time^%s\n",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		street_address,
		account_number,
		date_time,
		(transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( output_pipe );

} /* investment_transaction_date_time_update() */

char *investment_get_update_sys_string(
				char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name =
		get_table_name(
			application_name,
			"equity_account_balance" );

	key = "full_name,street_address,account_number,date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* investment_get_update_sys_string() */

char *investment_fetch_cash_or_equity_account_name(
				char *application_name,
				char *fund_name,
				char *investment_operation )
{
	char *unrealized_investment = {0};
	char *realized_gain = {0};
	char *realized_loss = {0};
	char *checking_account = {0};
	char *contributed_capital_account = {0};

	ledger_get_investment_account_names(
		&unrealized_investment,
		&realized_gain,
		&realized_loss,
		&checking_account,
		&contributed_capital_account,
		application_name,
		fund_name );

	if ( strcmp(	investment_operation, 
			INVESTMENT_OPERATION_TRANSFER ) == 0 )
	{
		return contributed_capital_account;
	}
	else
	if ( strcmp(	investment_operation, 
			INVESTMENT_OPERATION_PURCHASE ) == 0 )
	{
		return checking_account;
	}
	else
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid investment_operation = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 investment_operation );
		exit( 1 );
	}

} /* investment_fetch_cash_or_equity_account_name() */

boolean investment_get_account_balance_table_first_row(
				char *application_name,
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time )
{
	char *prior_date_time;

	prior_date_time =
		investment_account_balance_fetch_prior_date_time(
			application_name,
			full_name,
			street_address,
			account_number,
			date_time );

	if ( prior_date_time )
		return 0;
	else
		return 1;

} /* investment_get_account_balance_table_first_row() */

ACCOUNT_BALANCE *investment_account_balance_seek(
					LIST *account_balance_list,
					char *date_time )
{
	ACCOUNT_BALANCE *account_balance;

	if ( !list_rewind( account_balance_list ) ) return (ACCOUNT_BALANCE *)0;

	do {
		account_balance =
			list_get_pointer(
				account_balance_list );

		if ( strcmp( account_balance->date_time, date_time ) == 0 )
			return account_balance;

	} while( list_next( account_balance_list ) );

	return (ACCOUNT_BALANCE *)0;

} /* investment_account_balance_seek() */

