/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/tuition_refund.c	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "boolean.h"
#include "list.h"
#include "entity_self.h"
#include "transaction.h"
#include "journal.h"
#include "entity.h"
#include "account.h"
#include "paypal_deposit.h"
#include "paypal_item.h"
#include "education.h"
#include "registration.h"
#include "tuition_refund.h"

TUITION_REFUND *tuition_refund_calloc( void )
{
	TUITION_REFUND *tuition_refund;

	if ( ! ( tuition_refund = calloc( 1, sizeof( TUITION_REFUND ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return tuition_refund;
}

TUITION_REFUND *tuition_refund_fetch(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *refund_date_time,
			boolean fetch_registration )
{
	return
		tuition_refund_parse(
			string_pipe_fetch(
				tuition_refund_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					tuition_refund_primary_where(
						student_full_name,
						student_street_address,
						season_name,
						year,
						refund_date_time ) ) ),
			fetch_registration );
}

LIST *tuition_refund_system_list(
			char *system_string,
			boolean fetch_registration )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *tuition_refund_list = list_new();

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			tuition_refund_list,
			tuition_refund_parse(
				input,
				fetch_registration ) );
	}

	pclose( input_pipe );
	return tuition_refund_list;
}

char *tuition_refund_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "select.sh '*' %s \"%s\" select",
		 TUITION_REFUND_TABLE,
		 where );

	return strdup( system_string );
}

void tuition_refund_list_insert( LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( tuition_refund_list ) ) return;

	insert_pipe =
		tuition_refund_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		tuition_refund = list_get( tuition_refund_list );

		tuition_refund_insert_pipe(
			insert_pipe,
			tuition_refund->student_entity->full_name,
			tuition_refund->student_entity->street_address,
			tuition_refund->
				semester->
				season_name,
			tuition_refund->
				semester->
				year,
			tuition_refund->refund_date_time,
			tuition_refund->refund_amount,
			tuition_refund->merchant_fees_expense,
			tuition_refund->net_refund_amount,
			tuition_refund->payor_entity->full_name,
			tuition_refund->payor_entity->street_address,
			tuition_refund->transaction_date_time,
			tuition_refund->paypal_date_time );

	} while ( list_next( tuition_refund_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Tuition Refund Errors";

		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e '%s' '' '^'			 ",
			error_filename,
			title );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

FILE *tuition_refund_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		TUITION_REFUND_TABLE,
		TUITION_REFUND_INSERT_COLUMNS,
		'n',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void tuition_refund_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *refund_date_time,
			double refund_amount,
			double merchant_fees_expense,
			double net_refund_amount,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *paypal_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%d^%s^%.2lf^%.2lf^%.2lf^%s^%s^%s^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		registration_escape_full_name( student_full_name ),
		student_street_address,
		season_name,
		year,
		refund_date_time,
		refund_amount,
		merchant_fees_expense,
		net_refund_amount,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		(transaction_date_time)
			? transaction_date_time
			: "",
		(paypal_date_time)
			? paypal_date_time
			: "" );
}

TUITION_REFUND *tuition_refund_parse(
			char *input,
			boolean fetch_registration )
{
	char student_full_name[ 128 ];
	char student_street_address[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char refund_date_time[ 128 ];
	char refund_amount[ 128 ];
	char merchant_fees_expense[ 128 ];
	char net_refund_amount[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char paypal_date_time[ 128 ];
	TUITION_REFUND *tuition_refund;

	if ( !input || !*input ) return (TUITION_REFUND *)0;

	/* See: attribute_list tuition_refund */
	/* ----------------------------------- */
	piece( student_full_name, SQL_DELIMITER, input, 0 );
	piece( student_street_address, SQL_DELIMITER, input, 1 );
	piece( season_name, SQL_DELIMITER, input, 2 );
	piece( year, SQL_DELIMITER, input, 3 );
	piece( refund_date_time, SQL_DELIMITER, input, 4 );

	tuition_refund =
		tuition_refund_new(
			entity_new(
				strdup( student_full_name ),
				strdup( student_street_address ) ),
			semester_new(
				strdup( season_name ),
				atoi( year ) ),
			strdup( refund_date_time ) );

	piece( refund_amount, SQL_DELIMITER, input, 5 );
	tuition_refund->refund_amount = atof( refund_amount );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 6 );
	tuition_refund->merchant_fees_expense = atof( merchant_fees_expense );

	piece( net_refund_amount, SQL_DELIMITER, input, 7 );
	tuition_refund->net_refund_amount = atof( net_refund_amount );

	piece( payor_full_name, SQL_DELIMITER, input, 8 );

	if ( *payor_full_name )
	{
		piece( payor_street_address, SQL_DELIMITER, input, 9 );

		tuition_refund->payor_entity =
			entity_new(
				strdup( payor_full_name ),
				strdup( payor_street_address ) );
	}

	piece( transaction_date_time, SQL_DELIMITER, input, 10 );
	tuition_refund->transaction_date_time = strdup( transaction_date_time );

	piece( paypal_date_time, SQL_DELIMITER, input, 11 );
	tuition_refund->paypal_date_time = strdup( paypal_date_time );

	if ( fetch_registration )
	{
		tuition_refund->registration =
			registration_fetch(
				student_full_name,
				student_street_address,
				tuition_refund->
					semester->
					season_name,
				tuition_refund->
					semester->
					year );
	}
	return tuition_refund;
}

char *tuition_refund_primary_where(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *refund_date_time )
{
	char static where[ 1024 ];

	sprintf(where,
		"student_full_name = '%s' and		"
		"student_street_address = '%s' and	"
		"season_name = '%s' and			"
		"year = %d and				"
		"refund_date_time = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 registration_escape_full_name( student_full_name ),
		 student_street_address,
		 season_name,
		 year,
		 refund_date_time );

	return where;
}

TRANSACTION *tuition_refund_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			double refund_amount,
			double merchant_fees_expense,
			double net_refund_amount,
			char *account_payable,
			char *account_cash,
			char *account_fees_expense )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( dollar_virtually_same( refund_amount, 0.0 ) )
	{
		return (TRANSACTION *)0;
	}

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			refund_date_time
				/* transaction_date_time */,
			refund_amount
				/* transaction_amount */,
			TUITION_REFUND_MEMO,
			1 /* lock_transaction */,
			(*seconds_to_add)++ );

	transaction->journal_list = list_new();

	/* Debit payable */
	/* ------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_payable ) ) );

	/* Refund amount is always negative */
	/* -------------------------------- */
	journal->debit_amount = 0.0 - refund_amount;

	/* Credit cash */
	/* ----------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_cash ) ) );

	journal->credit_amount = 0.0 - net_refund_amount;

	/* Credit fees_expense */
	/* ------------------- */
	list_set(
		transaction->journal_list,
		journal_merchant_fees_expense(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			0.0 - merchant_fees_expense,
			account_fees_expense ) );

	return transaction;
}

FILE *tuition_refund_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 TUITION_REFUND_TABLE,
		 TUITION_REFUND_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void tuition_refund_update(
			double refund_amount,
			double net_refund_amount,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *refund_date_time )
{
	FILE *update_pipe = tuition_refund_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^refund_amount^%.2lf\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 refund_date_time,
		 refund_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^net_refund_amount^%.2lf\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 refund_date_time,
		 net_refund_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^payor_full_name^%s\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 refund_date_time,
		 payor_full_name );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^payor_street_address^%s\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 refund_date_time,
		 payor_street_address );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^transaction_date_time^%s\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 refund_date_time,
		 (transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

char *tuition_refund_list_display( LIST *tuition_refund_list )
{
	char display[ 65536 ];
	char *ptr = display;
	TUITION_REFUND *tuition_refund;

	*ptr = '\0';

	if ( !list_rewind( tuition_refund_list ) )
	{
		return "";
	}

	ptr += sprintf( ptr, "Tuition refund: " );

	do {
		tuition_refund =
			list_get(
				tuition_refund_list );

		if ( !list_at_head( tuition_refund_list ) )
		{
			ptr += sprintf( ptr, ", " );
		}

		ptr += sprintf(	ptr,
				"refund of %.2lf for %s",
				tuition_refund->refund_amount,
				entity_name_display(
					tuition_refund->
						student_entity->
						full_name,
					tuition_refund->
						student_entity->
						street_address ) );

	} while ( list_next( tuition_refund_list ) );

	ptr += sprintf( ptr, "; " );

	return strdup( display );
}

TUITION_REFUND *tuition_refund_steady_state(
			TUITION_REFUND *tuition_refund,
			double refund_amount,
			double merchant_fees_expense,
			ENTITY *payor_entity )
{
	if ( !tuition_refund->registration )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty registration.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Refund amount is always negative */
	/* -------------------------------- */
	if ( refund_amount > 0.0 )
	{
		tuition_refund->refund_amount =
			tuition_refund_amount(
				refund_amount );
	}

	tuition_refund->net_refund_amount =
		education_net_refund_amount(
			tuition_refund->refund_amount,
			merchant_fees_expense );

	if ( !payor_entity )
	{
		tuition_refund->payor_entity =
			tuition_refund->registration->payor_entity;
	}

	return tuition_refund;
}

void tuition_refund_trigger(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"tuition_refund_trigger \"%s\" '%s' '%s' %d \"%s\" '%s' '%s' '%s'",
		student_full_name,
		student_street_address,
		season_name,
		year,
		payor_full_name,
		payor_street_address,
		refund_date_time,
		state );

	if ( system( sys_string ) ){}
}

double tuition_refund_total( LIST *refund_list )
{
	TUITION_REFUND *refund;
	double total;

	if ( !list_rewind( refund_list ) ) return 0.0;

	total = 0.0;

	do {
		refund = list_get( refund_list );
		total += refund->refund_amount;

	} while ( list_next( refund_list ) );

	return total;
}

LIST *tuition_refund_transaction_list(
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;
	LIST *transaction_list;

	if ( !list_rewind( tuition_refund_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		tuition_refund =
			list_get(
				tuition_refund_list );

		if ( tuition_refund->tuition_refund_transaction )
		{
			list_set(
				transaction_list,
				tuition_refund->tuition_refund_transaction );
		}

	} while ( list_next( tuition_refund_list ) );

	return transaction_list;
}

void tuition_refund_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;
	char *payable;
	char *cash_account_name;
	char *fees_expense;

	if ( !list_rewind( tuition_refund_list ) ) return;

	payable = account_payable( (char *)0 );
	cash_account_name = entity_self_paypal_cash_account_name();
	fees_expense = account_fees_expense( (char *)0 );

	do {
		tuition_refund = list_get( tuition_refund_list );

		tuition_refund_set_transaction(
			transaction_seconds_to_add,
			tuition_refund,
			payable,
			cash_account_name,
			fees_expense );

	} while ( list_next( tuition_refund_list ) );
}

void tuition_refund_set_transaction(
			int *transaction_seconds_to_add,
			TUITION_REFUND *tuition_refund,
			char *account_payable,
			char *account_cash,
			char *account_fees_expense )
{
	if ( !tuition_refund->payor_entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty payor_entity.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ( tuition_refund->tuition_refund_transaction =
		tuition_refund_transaction(
			transaction_seconds_to_add,
			tuition_refund->payor_entity->full_name,
			tuition_refund->payor_entity->street_address,
			tuition_refund->refund_date_time,
			tuition_refund->refund_amount,
			tuition_refund->merchant_fees_expense,
			tuition_refund->net_refund_amount,
			account_payable,
			account_cash,
			account_fees_expense ) ) )
	{
		tuition_refund->transaction_date_time =
			tuition_refund->tuition_refund_transaction->
				transaction_date_time;
	}
	else
	{
		tuition_refund->transaction_date_time = (char *)0;
	}
}

TUITION_REFUND *tuition_refund_seek(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *refund_date_time,
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;

	if ( !list_rewind( tuition_refund_list ) )
		return (TUITION_REFUND *)0;

	do {
		tuition_refund = list_get( tuition_refund_list );

		if ( strcmp(
			tuition_refund->student_entity->full_name,
			student_full_name ) == 0
		&&   strcmp(
			tuition_refund->student_entity->street_address,
			student_street_address ) == 0
		&&   strcmp(
			tuition_refund->semester->season_name,
			season_name ) == 0
		&&   tuition_refund->semester->year == year
		&&   strcmp(
			tuition_refund->refund_date_time,
			refund_date_time ) == 0 )
		{
			return tuition_refund;
		}
	} while ( list_next( tuition_refund_list ) );

	return (TUITION_REFUND *)0;
}

boolean tuition_refund_list_any_exists(
			LIST *tuition_refund_list,
			LIST *existing_tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;

	if ( !list_rewind( tuition_refund_list ) ) return 0;

	do {
		tuition_refund = list_get( tuition_refund_list );

		if ( tuition_refund_seek(
			tuition_refund->student_entity->full_name,
			tuition_refund->student_entity->street_address,
			tuition_refund->semester->season_name,
			tuition_refund->semester->year,
			tuition_refund->refund_date_time,
			existing_tuition_refund_list ) )
		{
			return 1;
		}
	} while ( list_next( tuition_refund_list ) );

	return 0;
}

LIST *tuition_refund_list_enrollment_list(
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;
	LIST *enrollment_list;

	if ( !list_rewind( tuition_refund_list ) ) return (LIST *)0;

	enrollment_list = list_new();

	do {
		tuition_refund =
			list_get(
				tuition_refund_list );

		if ( !tuition_refund->registration )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( !list_length( 
			tuition_refund->
			     registration->
			     enrollment_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty enrollment_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );

		}

		list_set_list(
			enrollment_list,
			tuition_refund->
			       registration->
			       enrollment_list );

	} while ( list_next( tuition_refund_list ) );

	return enrollment_list;
}

LIST *tuition_refund_enrollment_list(
			TUITION_REFUND *tuition_refund )
{
	if ( !tuition_refund->registration )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_length( tuition_refund->registration->enrollment_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty enrollment_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return tuition_refund->registration->enrollment_list;
}

TUITION_REFUND *tuition_refund_new(
			ENTITY *student_entity,
			SEMESTER *semester,
			char *refund_date_time )
{
	TUITION_REFUND *tuition_refund = tuition_refund_calloc();

	tuition_refund->student_entity = student_entity;
	tuition_refund->semester = semester;
	tuition_refund->refund_date_time = refund_date_time;

	return tuition_refund;
}

double tuition_refund_amount(
			double refund_amount )
{
	/* Refund amount is always negative */
	/* -------------------------------- */
	if ( refund_amount > 0.0 )
	{
		refund_amount = 0.0 - refund_amount;
	}
	return refund_amount;
}

double tuition_refund_fee_total(
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;
	double total;

	if ( !list_rewind( tuition_refund_list ) ) return 0.0;

	total = 0.0;

	do {
		tuition_refund = list_get( tuition_refund_list );
		total += tuition_refund->merchant_fees_expense;

	} while ( list_next( tuition_refund_list ) );

	return total;
}

