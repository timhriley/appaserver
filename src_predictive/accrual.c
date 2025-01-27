/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/accrual.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "date_convert.h"
#include "piece.h"
#include "sql.h"
#include "date.h"
#include "column.h"
#include "folder_attribute.h"
#include "html.h"
#include "entity.h"
#include "transaction.h"
#include "journal.h"
#include "accrual.h"

char *accrual_escape_description( char *accrual_description )
{
	static char escape_description[ 64 ];

	if ( !accrual_description )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: accrual_description is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	string_escape_quote(
		escape_description,
		accrual_description );
}

char *accrual_primary_where(
		char *full_name,
		char *street_address,
		char *accrual_description )
{
	char where[ 256 ];

	if ( !full_name
	||   !street_address
	||   !accrual_description )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	sprintf(where,
		"full_name = '%s' and			"
		"street_address = '%s' and		"
		"accrual_description = '%s'	",
		entity_escape_full_name( full_name ),
		street_address,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		accrual_escape_description(
			accrual_description ) );

	return strdup( where );
}

ACCRUAL *accrual_parse(
		boolean property_attribute_exists,
		char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char accrual_description[ 128 ];
	char buffer[ 128 ];
	ACCRUAL *accrual;

	if ( !input || !*input ) return (ACCRUAL *)0;

	/* See accrual_select() */
	/* ------------------------ */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( accrual_description, SQL_DELIMITER, input, 2 );

	accrual =
		accrual_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( accrual_description ) );

	accrual->property_attribute_exists =
		property_attribute_exists;

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) accrual->debit_account = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) accrual->credit_account = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) accrual->accrued_daily_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) accrual->accrued_monthly_amount = atof( buffer );

	if ( accrual->property_attribute_exists )
	{
		piece( buffer, SQL_DELIMITER, input, 7 );
		if ( *buffer )
		{
			accrual->rental_property_street_address =
				strdup( buffer );
		}
	}

	accrual->transaction_increment_date_time =
		/* ------------------------------------ */
		/* Returns heap memory.			*/
		/* Increments second each invocation.   */
		/* ------------------------------------ */
		transaction_increment_date_time(
			(char *)0 /* transaction_date */ );

	return accrual;
}

FILE *accrual_input_pipe( char *system_string )
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

LIST *accrual_system_list(
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

	input_pipe = accrual_input_pipe( system_string );
	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			accrual_parse(
				property_attribute_exists,
				input ) );
	}

	pclose( input_pipe );
	return list;
}

char *accrual_memo(
		char *accrual_description,
		char *credit_account )
{
	char memo[ 256 ];

	sprintf( memo,
		 "%s/%s",
		 accrual_description,
		 credit_account );


	return strdup(
		format_initial_capital(
			memo,
			memo ) );
}

int accrual_last_transaction_days_between(
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
		accrual_max_transaction_date_time(
			transaction_table,
			full_name,
			street_address,
			debit_account,
			credit_account )
				/* from_date_string */,
		transaction_increment_date_time
				/* to_date_string */ );
}

char *accrual_journal_transaction_subquery(
		char *journal_table,
		char *transaction_table,
		char *debit_account,
		char *credit_account )
{
	char subquery[ 1024 ];

	sprintf(subquery,
		"exists ( select 1 from %s			"
		"	   where %s.full_name =			"
		"		%s.full_name 			"
		"	     and %s.street_address =		"
		"		%s.street_address 		"
		"	     and %s.transaction_date_time =	"
		"		%s.transaction_date_time	"
		"	     and account = '%s' ) and		"
		"exists ( select 1 from %s			"
		"	   where %s.full_name =			"
		"		%s.full_name 			"
		"	     and %s.street_address =		"
		"		%s.street_address 		"
		"	     and %s.transaction_date_time =	"
		"		%s.transaction_date_time	"
		"	     and account = '%s' ) 		",
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

TRANSACTION *accrual_daily_transaction(
		char *full_name,
		char *street_address,
		char *accrual_description,
		char *transaction_increment_date_time,
		char *debit_account,
		char *credit_account,
		double accrued_daily_amount )
{
	int days_between;

	if ( !full_name
	||   !street_address
	||   !accrual_description
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
			accrual_last_transaction_days_between(
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
		accrual_daily_accrued_amount(
			accrued_daily_amount,
			days_between ),
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		accrual_memo(
			accrual_description,
			credit_account ),
		debit_account,
		credit_account );
}

TRANSACTION *accrual_monthly_transaction(
		char *full_name,
		char *street_address,
		char *accrual_description,
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
	||   !accrual_description
	||   !transaction_increment_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !accrued_monthly_amount
	||   !debit_account
	||   !credit_account )
	{
		return NULL;
	}

	max_transaction_date_time =
		accrual_max_transaction_date_time(
			TRANSACTION_TABLE,
			full_name,
			street_address,
			debit_account,
			credit_account );

	monthly_accrue =
		accrual_monthly_accrue(
			max_transaction_date_time
				/* begin_date_string */,
			transaction_increment_date_time
				/* end_date_string */,
			accrued_monthly_amount );

	if ( !monthly_accrue ) return NULL;

	transaction =
		transaction_binary(
			full_name,
			street_address,
			transaction_increment_date_time,
			monthly_accrue,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			accrual_memo(
				accrual_description,
				credit_account ),
			debit_account,
			credit_account );

	transaction->rental_property_street_address =
		rental_property_street_address;

	return transaction;
}

char *accrual_system_string(
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

char *accrual_select(
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

boolean accrual_property_attribute_exists(
		char *accrual_table,
		char *accrual_property_attribute )
{
	return
	folder_attribute_exists(
		accrual_table /* folder_name */,
		accrual_property_attribute /* attribute_name */ );
}

double accrual_daily_accrued_amount(
		double accrued_daily_amount,
		int days_between )
{
	return (double)days_between * accrued_daily_amount;

}

char *accrual_max_transaction_date_time(
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
		( tmp = accrual_journal_transaction_subquery(
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

LIST *accrual_list_fetch( void )
{
	LIST *accrual_list;
	boolean property_attribute_exists;

	property_attribute_exists =
		accrual_property_attribute_exists(
			ACCRUAL_TABLE,
			ACCRUAL_PROPERTY_ATTRIBUTE );

	accrual_list =
		accrual_system_list(
			accrual_system_string(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				accrual_select(
					ACCRUAL_SELECT_ATTRIBUTES,
					ACCRUAL_PROPERTY_ATTRIBUTE,
					property_attribute_exists ),
				ACCRUAL_TABLE,
				"1 = 1" /* where */ ),
			property_attribute_exists );

	if ( list_length( accrual_list ) )
	{
		accrual_list_transaction_set(
			accrual_list /* in/out */ );
	}

	return accrual_list;
}

void accrual_list_transaction_set( LIST *accrual_list )
{
	ACCRUAL *accrual;

	if ( !list_rewind( accrual_list ) ) return;

	do {
		accrual = list_get( accrual_list );

		accrual->transaction =
			accrual_transaction(
				accrual->full_name,
				accrual->street_address,
				accrual->accrual_description,
				accrual->
					transaction_increment_date_time,
				accrual->debit_account,
				accrual->credit_account,
				accrual->accrued_daily_amount,
				accrual->accrued_monthly_amount,
				accrual->
					rental_property_street_address );

	} while ( list_next( accrual_list ) );
}

ACCRUAL *accrual_fetch(
		char *full_name,
		char *street_address,
		char *accrual_description )
{
	ACCRUAL *accrual;
	boolean property_attribute_exists;
	char *select;
	char *primary_where;
	char *system_string;

	if ( !full_name
	||   !street_address
	||   !accrual_description )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	property_attribute_exists =
		accrual_property_attribute_exists(
			ACCRUAL_TABLE,
			ACCRUAL_PROPERTY_ATTRIBUTE );

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		accrual_select(
			ACCRUAL_SELECT_ATTRIBUTES,
			ACCRUAL_PROPERTY_ATTRIBUTE,
			property_attribute_exists );
	
	primary_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		accrual_primary_where(
			full_name,
			street_address,
			accrual_description );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		accrual_system_string(
			select,
			ACCRUAL_TABLE,
			primary_where );

	accrual =
		accrual_parse(
			property_attribute_exists,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_fetch(
				system_string ) );

	if ( accrual )
	{
		accrual->transaction =
			accrual_transaction(
				accrual->full_name,
				accrual->street_address,
				accrual->accrual_description,
				accrual->transaction_increment_date_time,
				accrual->debit_account,
				accrual->credit_account,
				accrual->accrued_daily_amount,
				accrual->accrued_monthly_amount,
				accrual->rental_property_street_address );
	}

	free( select );
	free( primary_where );
	free( system_string );

	return accrual;
}

TRANSACTION *accrual_transaction(
		char *full_name,
		char *street_address,
		char *accrual_description,
		char *transaction_increment_date_time,
		char *debit_account,
		char *credit_account,
		double accrued_daily_amount,
		double accrued_monthly_amount,
		char *rental_property_street_address )
{
	TRANSACTION *transaction = {0};

	if ( accrued_daily_amount )
	{
		transaction =
			accrual_daily_transaction(
				full_name,
				street_address,
				accrual_description,
				transaction_increment_date_time,
				debit_account,
				credit_account,
				accrued_daily_amount );
	}
	else
	if ( accrued_monthly_amount )
	{
		transaction =
			accrual_monthly_transaction(
				full_name,
				street_address,
				accrual_description,
				transaction_increment_date_time,
				debit_account,
				credit_account,
				accrued_monthly_amount,
				rental_property_street_address );
	}

	return transaction;
}

LIST *accrual_transaction_list_extract( LIST *accrual_list )
{
	LIST *transaction_list;
	ACCRUAL *accrual;

	if ( !list_rewind( accrual_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		accrual = list_get( accrual_list );

		list_set(
			transaction_list,
			accrual->transaction );

	} while ( list_next( accrual_list ) );

	return transaction_list;
}

ACCRUAL *accrual_new(
		char *full_name,
		char *street_address,
		char *accrual_description )
{
	ACCRUAL *accrual;

	if ( !full_name
	||   !street_address
	||   !accrual_description )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	accrual = accrual_calloc();

	accrual->full_name = full_name;
	accrual->street_address = street_address;
	accrual->accrual_description = accrual_description;

	return accrual;
}

ACCRUAL *accrual_calloc( void )
{
	ACCRUAL *accrual;

	if ( ! ( accrual = calloc( 1, sizeof ( ACCRUAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return accrual;
}

char *accrual_opening_begin_date_string( DATE *end_date )
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

	if ( date_day_number( end_date ) > 15 )
	{
		sprintf( begin_date_string,
			 "%d-%d-01",
			 date_year_number( end_date ),
			 date_month_number( end_date ) );
	}
	else
	{
		begin_date = date_calloc();

		date_copy( begin_date, end_date );

		date_increment_months(
			begin_date,
			-1 /* months */ );

		date_set_day( begin_date, 1, 0 /* utc_offset */ );

		strcpy(
			begin_date_string,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			date_yyyy_mm_dd_display(
				begin_date ) );
	}

	return strdup( begin_date_string );
}

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
		date_day_number(
			begin_date );

	end_date_day =
		date_day_number(
			end_date );

	days_in_month =
		date_last_month_day(
			date_month_number( begin_date ),
			date_year_number( begin_date ) );

	month_percent =
		accrual_month_percent(
			begin_date_day,
			end_date_day,
			days_in_month );

	month_accrual_amount = monthly_accrual * month_percent;

	return month_accrual_amount;
}

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
		date_day_number(
			begin_date );

	end_second_date_day =
		date_day_number(
			end_date );

	days_in_first_month =
		date_last_month_day(
			date_month_number( begin_date ),
			date_year_number( begin_date ) );

	days_in_second_month =
		date_last_month_day(
			date_month_number( end_date ),
			date_year_number( end_date ) );

	first_month_percent =
		accrual_month_percent(
			begin_first_date_day,
			days_in_first_month,
			days_in_first_month );

	second_month_percent =
		accrual_month_percent(
			1,
			end_second_date_day,
			days_in_second_month );

	first_month_accrual_amount =
		monthly_accrual * first_month_percent;

	second_month_accrual_amount =
		monthly_accrual * second_month_percent;

	return first_month_accrual_amount + second_month_accrual_amount;
}

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
		date_day_number(
			begin_date );

	end_last_date_day =
		date_day_number(
			end_date );

	days_in_first_month =
		date_last_month_day(
			date_month_number( begin_date ),
			date_year_number( begin_date ) );

	days_in_last_month =
		date_last_month_day(
			date_month_number( end_date ),
			date_year_number( end_date ) );

	first_month_percent =
		accrual_month_percent(
			begin_first_date_day,
			days_in_first_month,
			days_in_first_month );

	last_month_percent =
		accrual_month_percent(
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
}

double accrual_month_percent(
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
}

double accrual_monthly_accrue(
		char *begin_date_string,
		char *end_date_string,
		double accrued_monthly_amount )
{
	DATE *begin_date;
	DATE *end_date;
	int months_between;
	double monthly_accrue;

	if ( ! ( end_date =
			/* ------------------- */
			/* Trims trailing time */
			/* ------------------- */
			date_yyyy_mm_dd_new(
				end_date_string ) ) )
	{
		fprintf(stderr,
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
			accrual_opening_begin_date_string(
				end_date );

		if ( ! ( begin_date =
				/* ------------------- */
				/* Trims trailing time */
				/* ------------------- */
				date_yyyy_mm_dd_new(
					begin_date_string ) ) )
		{
			fprintf(stderr,
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
				/* ------------------- */
				/* Trims trailing time */
				/* ------------------- */
				date_yyyy_mm_dd_new(
					begin_date_string ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: invalid begin_date_string = (%s)\n",
		 		__FILE__,
		 		__FUNCTION__,
		 		__LINE__,
		 		begin_date_string );

			exit( 1 );
		}

		date_increment_days(
			begin_date,
			1.0 );
	}

	months_between =
		date_months_between(
			begin_date,
			end_date );

	if ( months_between == 0 )
	{
		monthly_accrue =
			accrual_monthly_within_month_accrue(
				begin_date,
				end_date,
				accrued_monthly_amount );
	}
	else
	if ( months_between == 1 )
	{
		monthly_accrue =
			accrual_monthly_next_month_accrue(
				begin_date,
				end_date,
				accrued_monthly_amount );
	}
	else
	{
		monthly_accrue =
			accrual_monthly_multi_month_accrue(
				begin_date,
				end_date,
				accrued_monthly_amount,
				months_between );
	}

	return monthly_accrue;
}

