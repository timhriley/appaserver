/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/accrual.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "appaserver_library.h"
#include "accrual.h"

ACCRUAL *accrual_calloc( void )
{
	ACCRUAL *p =
		(ACCRUAL *)
			calloc( 1, sizeof( ACCRUAL ) );

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

} /* accrual_calloc() */

char *accrual_get_select( void )
{
	char *select =
"full_name,street_address,purchase_date_time,asset_name,accrual_date,accrual_amount,transaction_date_time";

	return select;
}

ACCRUAL *accrual_parse(			char *application_name,
					char *input_buffer )
{
	ACCRUAL *accrual;
	char piece_buffer[ 256 ];

	accrual = accrual_calloc();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	accrual->full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	accrual->street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	accrual->purchase_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	accrual->asset_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	accrual->accrual_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		accrual->accrual_amount =
		accrual->database_accrual_amount =
			atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		accrual->transaction_date_time =
		accrual->database_transaction_date_time =
			strdup( piece_buffer );

	if ( accrual->transaction_date_time )
	{
		accrual->transaction =
			ledger_transaction_fetch(
				application_name,
				accrual->full_name,
				accrual->street_address,
				accrual->transaction_date_time );
	}

	return accrual;

} /* accrual_parse() */

ACCRUAL *accrual_fetch(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date )
{
	char sys_string[ 1024 ];
	char *ledger_where;
	char buffer[ 128 ];
	char where[ 256 ];
	char *select;
	char *results;

	select = accrual_get_select();

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( where,
		 "%s and asset_name = '%s' and accrual_date = '%s'",
		 ledger_where,
		 escape_character(	buffer,
					asset_name,
					'\'' ),
		 accrual_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=accrual		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return (ACCRUAL *)0;
	}

	return accrual_parse( application_name, results );

} /* accrual_fetch() */

void accrual_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date,
			double accrual_amount,
			double database_accrual_amount,
			char *transaction_date_time,
			char *database_transaction_date_time )
{
	char *sys_string;
	FILE *output_pipe;

	sys_string = accrual_get_update_sys_string( application_name );
	output_pipe = popen( sys_string, "w" );

	if ( !dollar_virtually_same(
			accrual_amount,
			database_accrual_amount ) )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^%s^%s^accrual_amount^%.2lf\n",
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			accrual_date,
			accrual_amount );
	}

	if ( timlib_strcmp(	transaction_date_time,
				database_transaction_date_time ) != 0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^%s^%s^transaction_date_time^%s\n",
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			accrual_date,
			(transaction_date_time)
				? transaction_date_time
				: "" );
	}

	pclose( output_pipe );

} /* accrual_update() */

char *accrual_get_update_sys_string(
				char *application_name )
{
	static char sys_string[ 256 ];
	char *table_name;
	char *key;

	table_name =
		get_table_name(
			application_name,
			"prepaid_asset_accrual" );

	key =
"full_name,street_address,purchase_date_time,asset_name,accrual_date";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	return sys_string;

} /* accrual_get_update_sys_string() */

double accrual_get_amount(
			double extension,
			double accrual_period_years,
			char *prior_accrual_date_string,
			char *accrual_date_string,
			double accumulated_accrual )
{
	double annual_accrual_amount;
	double fraction_of_year;
	double accrual_amount;

	fraction_of_year =
		ledger_get_fraction_of_year(
			prior_accrual_date_string,
			accrual_date_string );

	if ( accrual_period_years )
	{
		annual_accrual_amount =
			extension /
			(double)accrual_period_years;
	}
	else
	{
		annual_accrual_amount = 0.0;
	}

	accrual_amount = annual_accrual_amount * fraction_of_year;

	/* If finished accruing */
	/* -------------------- */
	if ( accumulated_accrual + accrual_amount > extension )
	{
		accrual_amount =
			extension -
			accumulated_accrual;
	}

	return accrual_amount;

} /* accrual_get_amount() */

void accrual_journal_ledger_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double accrual_amount,
				char *asset_account_name,
				char *expense_account_name )
{
	if ( !asset_account_name || !*asset_account_name )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty asset_account_name.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	if ( !expense_account_name || !*expense_account_name )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty expense_account_name.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	if ( accrual_amount )
	{
		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			expense_account_name,
			accrual_amount,
			1 /* is_debit */ );

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			asset_account_name,
			accrual_amount,
			0 /* not is_debit */ );
	}

} /* accrual_journal_ledger_refresh() */

LIST *accrual_fetch_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name )
{
	ACCRUAL *accrual;
	char *select;
	LIST *accrual_list;
	char sys_string[ 1024 ];
	char *ledger_where;
	char buffer[ 128 ];
	char where[ 512 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];

	select = accrual_get_select();

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( where,
		 "%s and asset_name = '%s'",
		 ledger_where,
		 escape_character(	buffer,
					asset_name,
					'\'' ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=prepaid_asset_accrual	"
		 "			where=\"%s\"			"
		 "			order=accrual_date		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	accrual_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		accrual =
			accrual_parse(
				application_name,
				input_buffer );

		list_append_pointer( accrual_list, accrual );
	}

	pclose( input_pipe );

	return accrual_list;

} /* accrual_fetch_list() */

/* Returns new accumulated_accrual */
/* ------------------------------------ */
double accrual_list_set(
		LIST *accrual_list,
		/* ----------------------------------- */
		/* Arrived date is the effective date. */
		/* ----------------------------------- */
		char *arrived_date_string,
		double extension,
		double accrual_period_years )
{
	double accumulated_accrual;
	ACCRUAL *accrual;
	char *prior_accrual_date_string = {0};

	if ( !list_rewind( accrual_list ) ) return 0.0;

	accumulated_accrual = 0.0;
	prior_accrual_date_string = arrived_date_string;

	do {
		accrual = list_get( accrual_list );

		accrual->accrual_amount =
			accrual_get_amount(
				extension,
				accrual_period_years,
				prior_accrual_date_string,
				accrual->accrual_date,
				accumulated_accrual );

		accumulated_accrual +=
			accrual->accrual_amount;

		prior_accrual_date_string =
			accrual->accrual_date;

	} while( list_next( accrual_list ) );

	return accumulated_accrual;

} /* accrual_list_set() */

void accrual_list_update_and_transaction_propagate(
				LIST *accrual_list,
				char *application_name,
				char *asset_account_name,
				char *expense_account_name )
{
	ACCRUAL *accrual;
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( accrual_list ) ) return;

	do {
		accrual = list_get( accrual_list );

		if ( !accrual->transaction )
		{
			accrual->transaction_date_time =
				ledger_get_transaction_date_time(
					accrual->accrual_date );
	
			accrual->transaction =
				ledger_transaction_new(
					accrual->full_name,
					accrual->street_address,
					accrual->transaction_date_time,
					ACCRUAL_MEMO );
	
			accrual->transaction_date_time =
			accrual->transaction->transaction_date_time =
			ledger_transaction_insert(
				application_name,
				accrual->transaction->full_name,
				accrual->transaction->street_address,
				accrual->
					transaction->
					transaction_date_time,
				accrual->accrual_amount,
				accrual->transaction->memo,
				0 /* check_number */,
				1 /* lock_transaction */ );
		}

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				accrual->transaction_date_time;
		}

		accrual_update(
			application_name,
			accrual->full_name,
			accrual->street_address,
			accrual->purchase_date_time,
			accrual->asset_name,
			accrual->accrual_date,
			accrual->accrual_amount,
			accrual->database_accrual_amount,
			accrual->transaction_date_time,
			accrual->database_transaction_date_time );

		accrual_journal_ledger_refresh(
			application_name,
			accrual->transaction->full_name,
			accrual->transaction->street_address,
			accrual->
				transaction->
				transaction_date_time,
			accrual->accrual_amount,
			asset_account_name,
			expense_account_name );

	} while( list_next( accrual_list ) );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		asset_account_name );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		expense_account_name );

} /* accrual_list_update_and_transaction_propagate() */

void accrual_list_delete(
			LIST *accrual_list,
			char *application_name,
			char *asset_account_name,
			char *expense_account_name )
{
	ACCRUAL *accrual;
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( accrual_list ) ) return;

	do {
		accrual = list_get( accrual_list );

		accrual_delete(
			application_name,
			accrual->full_name,
			accrual->street_address,
			accrual->purchase_date_time,
			accrual->asset_name,
			accrual->accrual_date );

		if ( !accrual->transaction )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: expecting a transaction = (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	accrual->transaction_date_time );
			exit( 1 );
		}

		ledger_delete(
			application_name,
			TRANSACTION_FOLDER_NAME,
			accrual->transaction->full_name,
			accrual->transaction->street_address,
			accrual->transaction->transaction_date_time );

		ledger_delete(
			application_name,
			LEDGER_FOLDER_NAME,
			accrual->transaction->full_name,
			accrual->transaction->street_address,
			accrual->transaction->transaction_date_time );

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				accrual->
					transaction->
					transaction_date_time;
		}

	} while( list_next( accrual_list ) );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		asset_account_name );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		expense_account_name );

} /* accrual_list_delete() */

void accrual_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table_name;
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	field=
"full_name,street_address,purchase_date_time,asset_name,accrual_date";

	table_name =
		get_table_name(
			application_name,
			"prepaid_asset_accrual" );

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "sql.e							 ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"%s^%s^%s^%s^%s\n",
		 	escape_character(	buffer1,
						full_name,
						'\'' ),
			street_address,
			purchase_date_time,
		 	escape_character(	buffer2,
						asset_name,
						'\'' ),
			accrual_date );

	pclose( output_pipe );

} /* accrual_delete() */

ACCRUAL *accrual_list_seek(
			LIST *accrual_list,
			char *accrual_date )
{
	ACCRUAL *accrual;

	if ( !list_rewind( accrual_list ) ) return (ACCRUAL *)0;

	do {
		accrual = list_get( accrual_list );

		if ( strcmp(	accrual->accrual_date,
				accrual_date ) == 0 )
		{
			return accrual;
		}

	} while( list_next( accrual_list ) );

	return (ACCRUAL *)0;

} /* accrual_list_seek() */

char *accrual_get_prior_accrual_date( LIST *accrual_list )
{
	ACCRUAL *accrual;
	char *prior_accrual_date;

	if ( !list_length( accrual_list )
	||   list_at_head( accrual_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty list or at beginning of list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	list_previous( accrual_list );
	accrual = list_get( accrual_list );
	prior_accrual_date = accrual->accrual_date;
	list_next( accrual_list );

	return prior_accrual_date;

} /* accrual_get_prior_accrual_date() */

double accrual_monthly_accrue(	char *begin_date_string,
				char *end_date_string,
				double monthly_accrual )
{
	DATE *begin_date;
	DATE *end_date;
	int months_between;
	double total_accrual;

	if ( ! ( end_date =
			date_yyyy_mm_dd_new(
				end_date_string ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid end_date_string = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 end_date_string );

		exit( 1 );
	}

	if ( !begin_date_string || !*begin_date_string )
	{
		begin_date_string =
			accrual_get_opening_begin_date_string(
				end_date );

		if ( ! ( begin_date =
				date_yyyy_mm_dd_new(
					begin_date_string ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid begin_date_string = (%s)\n",
		 		__FILE__,
		 		__FUNCTION__,
		 		__LINE__,
		 		begin_date_string );

			exit( 1 );
		}
	}
	else
	{
		if ( ! ( begin_date =
				date_yyyy_mm_dd_new(
					begin_date_string ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid begin_date_string = (%s)\n",
		 		__FILE__,
		 		__FUNCTION__,
		 		__LINE__,
		 		begin_date_string );

			exit( 1 );
		}

		date_increment_days(
			begin_date,
			1.0,
			0 /* utc_offset */ );
	}

	months_between =
		date_months_between(
			begin_date,
			end_date );

	if ( months_between == 0 )
	{
		total_accrual =
			accrual_monthly_within_month_accrue(
				begin_date,
				end_date,
				monthly_accrual );
	}
	else
	if ( months_between == 1 )
	{
		total_accrual =
			accrual_monthly_next_month_accrue(
				begin_date,
				end_date,
				monthly_accrual );
	}
	else
	{
		total_accrual =
			accrual_monthly_multi_month_accrue(
				begin_date,
				end_date,
				monthly_accrual,
				months_between );
	}

	return total_accrual;

} /* accrual_monthly_accrue() */

char *accrual_get_opening_begin_date_string(
				DATE *end_date )
{
	char begin_date_string[ 16 ];
	DATE *begin_date;

	if ( !end_date )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty end_date.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( date_get_day_number( end_date ) > 15 )
	{
		sprintf( begin_date_string,
			 "%d-%d-01",
			 date_get_year( end_date ),
			 date_get_month( end_date ) );
	}
	else
	{
		begin_date = date_calloc();

		date_copy( begin_date, end_date );

		date_increment_months(
			begin_date,
			-1 /* months */,
			0 /* utc_offset */ );

		date_set_day( begin_date, 1, 0 /* utc_offset */ );

		date_get_yyyy_mm_dd(
			begin_date_string,
			begin_date );
	}

	return strdup( begin_date_string );

} /* accrual_get_opening_begin_date_string() */

/* begin_date and end_date are both in the same month. */
/* --------------------------------------------------- */
double accrual_monthly_within_month_accrue(
				DATE *begin_date,
				DATE *end_date,
				double monthly_accrual )
{
	int begin_date_day;
	int end_date_day;
	int days_in_month;
	double month_percent;
	double month_accrual_amount;

	begin_date_day =
		date_get_day_number(
			begin_date );

	end_date_day =
		date_get_day_number(
			end_date );

	days_in_month =
		date_get_last_month_day(
			date_get_month( begin_date ),
			date_get_year( begin_date ) );

	month_percent =
		accrual_get_month_percent(
			begin_date_day,
			end_date_day,
			days_in_month );

	month_accrual_amount = monthly_accrual * month_percent;

	return month_accrual_amount;

} /* accrual_monthly_within_month_accrue() */

/* begin_date and end_date are one month apart. */
/* -------------------------------------------- */
double accrual_monthly_next_month_accrue(
				DATE *begin_date,
				DATE *end_date,
				double monthly_accrual )
{
	int begin_first_date_day;
	int end_second_date_day;
	int days_in_first_month;
	int days_in_second_month;
	double first_month_percent;
	double second_month_percent;
	double first_month_accrual_amount;
	double second_month_accrual_amount;

	begin_first_date_day =
		date_get_day_number(
			begin_date );

	end_second_date_day =
		date_get_day_number(
			end_date );

	days_in_first_month =
		date_get_last_month_day(
			date_get_month( begin_date ),
			date_get_year( begin_date ) );

	days_in_second_month =
		date_get_last_month_day(
			date_get_month( end_date ),
			date_get_year( end_date ) );

	first_month_percent =
		accrual_get_month_percent(
			begin_first_date_day,
			days_in_first_month,
			days_in_first_month );

	second_month_percent =
		accrual_get_month_percent(
			1,
			end_second_date_day,
			days_in_second_month );

	first_month_accrual_amount =
		monthly_accrual * first_month_percent;

	second_month_accrual_amount =
		monthly_accrual * second_month_percent;

	return first_month_accrual_amount + second_month_accrual_amount;

} /* accrual_monthly_next_month_accrue() */

/* begin_date and end_date are more than one month apart. */
/* ------------------------------------------------------ */
double accrual_monthly_multi_month_accrue(
				DATE *begin_date,
				DATE *end_date,
				double monthly_accrual,
				int months_between )
{
	int begin_first_date_day;
	int end_last_date_day;
	int days_in_first_month;
	int days_in_last_month;
	double first_month_percent;
	double last_month_percent;
	double first_month_accrual_amount;
	double middle_months_accrual_amount;
	double last_month_accrual_amount;

	begin_first_date_day =
		date_get_day_number(
			begin_date );

	end_last_date_day =
		date_get_day_number(
			end_date );

	days_in_first_month =
		date_get_last_month_day(
			date_get_month( begin_date ),
			date_get_year( begin_date ) );

	days_in_last_month =
		date_get_last_month_day(
			date_get_month( end_date ),
			date_get_year( end_date ) );

	first_month_percent =
		accrual_get_month_percent(
			begin_first_date_day,
			days_in_first_month,
			days_in_first_month );

	last_month_percent =
		accrual_get_month_percent(
			1,
			end_last_date_day,
			days_in_last_month );

	first_month_accrual_amount =
		monthly_accrual * first_month_percent;

	last_month_accrual_amount =
		monthly_accrual * last_month_percent;

	middle_months_accrual_amount =
		(double)(months_between - 1) * monthly_accrual;

	return	first_month_accrual_amount +
		middle_months_accrual_amount +
		last_month_accrual_amount;

} /* accrual_monthly_multi_month_accrue() */

double accrual_get_month_percent(
			int begin_date_day,
			int end_date_day,
			int days_in_month )
{
	double month_percent;

	if ( !days_in_month ) return 0.0;

	month_percent =
		(double)( ( end_date_day - begin_date_day ) + 1 ) /
			  (double)days_in_month;

	return month_percent;

} /* accrual_get_month_percent() */
