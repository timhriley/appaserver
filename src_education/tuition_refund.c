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
#include "paypal_upload.h"
#include "product.h"
#include "transaction.h"
#include "journal.h"
#include "entity.h"
#include "course.h"
#include "account.h"
#include "paypal_deposit.h"
#include "paypal_item.h"
#include "tuition_payment.h"
#include "registration.h"
#include "education.h"
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
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time,
			boolean fetch_payment )
{
	TUITION_REFUND *tuition_refund;

	tuition_refund =
		tuition_refund_parse(
			pipe2string(
				tuition_refund_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					tuition_refund_primary_where(
						student_full_name,
						student_street_address,
						course_name,
						season_name,
						year,
						payor_full_name,
						payor_street_address,
						payment_date_time,
						refund_date_time ) ) ),
			fetch_payment );

	return tuition_refund;
}

LIST *tuition_refund_system_list(
			char *sys_string,
			boolean fetch_payment )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *tuition_refund_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			tuition_refund_list,
			tuition_refund_parse(
				input,
				fetch_payment ) );
	}

	pclose( input_pipe );
	return tuition_refund_list;
}

char *tuition_refund_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 TUITION_REFUND_TABLE,
		 where );

	return strdup( sys_string );
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
			tuition_refund->
				tuition_payment->
				enrollment->
				registration->
				student_entity->
				full_name,
			tuition_refund->
				tuition_payment->
				enrollment->
				registration->
				student_entity->
				street_address,
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				course->
				course_name,
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				semester->
				season_name,
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				semester->
				year,
			tuition_refund->payor_entity->full_name,
			tuition_refund->payor_entity->street_address,
			tuition_refund->tuition_payment->payment_date_time,
			tuition_refund->refund_date_time,
			tuition_refund->refund_amount,
			tuition_refund->net_refund_amount,
			tuition_refund->transaction_date_time,
			tuition_refund->merchant_fees_expense,
			tuition_refund->paypal_date_time );

	} while ( list_next( tuition_refund_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Ticket Refund Errors";

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
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void tuition_refund_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time,
			double refund_amount,
			double net_refund_amount,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *paypal_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%d^%s^%s^%s^%s^%.2lf^%.2lf^%s^%.2lf^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		registration_escape_name( student_full_name ),
		student_street_address,
		course_name,
		season_name,
		year,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		payment_date_time,
		refund_date_time,
		refund_amount,
		net_refund_amount,
		(transaction_date_time)
			? transaction_date_time
			: "",
		merchant_fees_expense,
		(paypal_date_time)
			? paypal_date_time
			: "" );
}

TUITION_REFUND *tuition_refund_parse(
			char *input,
			boolean fetch_payment )
{
	char student_full_name[ 128 ];
	char student_street_address[ 128 ];
	char course_name[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char payment_date_time[ 128 ];
	char refund_date_time[ 128 ];
	char refund_amount[ 128 ];
	char net_refund_amount[ 128 ];
	char transaction_date_time[ 128 ];
	char merchant_fees_expense[ 128 ];
	char paypal_date_time[ 128 ];
	TUITION_REFUND *tuition_refund;

	if ( !input || !*input ) return (TUITION_REFUND *)0;

	tuition_refund = tuition_refund_calloc();

	/* See: attribute_list tuition_refund */
	/* ----------------------------------- */
	piece( student_full_name, SQL_DELIMITER, input, 0 );
	piece( student_street_address, SQL_DELIMITER, input, 1 );
	piece( course_name, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );
	piece( year, SQL_DELIMITER, input, 4 );
	piece( payor_full_name, SQL_DELIMITER, input, 5 );
	piece( payor_street_address, SQL_DELIMITER, input, 6 );
	piece( payment_date_time, SQL_DELIMITER, input, 7 );

	tuition_refund->tuition_payment =
		tuition_payment_new(
			entity_new(
				strdup( student_full_name ),
				strdup( student_street_address ) ),
			strdup( course_name ),
			strdup( season_name ),
			atoi( year ),
			entity_new(
				strdup( payor_full_name ),
				strdup( payor_street_address ) ),
			strdup( payment_date_time ) );

	piece( refund_date_time, SQL_DELIMITER, input, 8 );
	tuition_refund->refund_date_time = strdup( refund_date_time );

	piece( refund_amount, SQL_DELIMITER, input, 9 );
	tuition_refund->refund_amount = atof( refund_amount );

	piece( net_refund_amount, SQL_DELIMITER, input, 10 );
	tuition_refund->net_refund_amount = atof( net_refund_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 11 );
	tuition_refund->transaction_date_time = strdup( transaction_date_time );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 12 );
	tuition_refund->merchant_fees_expense = atof( merchant_fees_expense );

	piece( paypal_date_time, SQL_DELIMITER, input, 13 );
	tuition_refund->paypal_date_time = strdup( paypal_date_time );

	if ( fetch_payment )
	{
		tuition_refund->tuition_payment =
			tuition_payment_fetch(
				tuition_refund->
					tuition_payment->
					enrollment->
					registration->
					student_entity->
					full_name,
				tuition_refund->
					tuition_payment->
					enrollment->
					registration->
					student_entity->
					street_address,
				tuition_refund->
					tuition_payment->
					enrollment->
					offering->
					course->
					course_name,
				tuition_refund->
					tuition_payment->
					enrollment->
					offering->
					semester->
					season_name,
				tuition_refund->
					tuition_payment->
					enrollment->
					offering->
					semester->
					year,
				tuition_refund->payor_entity->full_name,
				tuition_refund->payor_entity->street_address,
				tuition_refund->
					tuition_payment->
					payment_date_time,
				1 /* fetch_payment */ );
	}

	return tuition_refund;
}

char *tuition_refund_primary_where(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time )
{
	char static where[ 1024 ];

	sprintf(where,
		"full_name = '%s' and			"
		"street_address = '%s' and		"
		"course_name = '%s' and			"
		"season_name = '%s' and			"
		"year = %d and				"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s' and	"
		"payment_date_time = '%s' and		"
		"refund_date_time = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 registration_name_escape( student_full_name ),
		 student_street_address,
		 course_name,
		 season_name,
		 year,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address,
		 payment_date_time,
		 refund_date_time );

	return where;
}

TRANSACTION *tuition_refund_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			char *course_name,
			char *program_name,
			double refund_amount,
			double merchant_fees_expense,
			double net_refund_amount,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( dollar_virtually_same( refund_amount, 0.0 ) )
		return (TRANSACTION *)0;

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			refund_date_time
				/* transaction_date_time */,
			refund_amount
				/* transaction_amount */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			strdup( tuition_refund_memo( course_name ) ),
			1 /* lock_transaction */,
			(*seconds_to_add)++ );

	transaction->program_name = program_name;

	if ( !transaction->journal_list )
	{
		transaction->journal_list = list_new();
	}

	/* Debit revenue */
	/* ------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				revenue_account ) ) );

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
				cash_account_name ) ) );

	journal->credit_amount = 0.0 - net_refund_amount;

	/* Credit fees_expense */
	/* ------------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_fees_expense ) ) );

	journal->credit_amount = merchant_fees_expense;

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
			double net_refund_amount,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time )
{
	FILE *update_pipe = tuition_refund_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^%s^%s^%s^net_refund_amount^%.2lf\n",
		 student_full_name,
		 student_street_address,
		 course_name,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 payment_date_time,
		 refund_date_time,
		 net_refund_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^%s^%s^%s^transaction_date_time^%s\n",
		 student_full_name,
		 student_street_address,
		 course_name,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 payment_date_time,
		 refund_date_time,
		 (transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

char *tuition_refund_list_display( LIST *refund_list )
{
	char display[ 65536 ];
	char *ptr = display;
	TUITION_REFUND *refund;

	*ptr = '\0';

	if ( !list_rewind( refund_list ) )
	{
		return "";
	}

	ptr += sprintf( ptr, "Tuition refund: " );

	do {
		refund =
			list_get(
				refund_list );

		if ( !list_at_head( refund_list ) )
		{
			ptr += sprintf( ptr, ", " );
		}

		if (	refund
		&&	refund->
				tuition_payment
		&&	refund->
				tuition_payment->
				enrollment )
		{
			ptr += sprintf(	ptr,
					"%s",
					refund->
						tuition_payment->
						enrollment->
						offering->
						course->
						course_name );
		}
		else
		{
			ptr += sprintf(	ptr,
					"%s",
					"Non existing course" );
		}

	} while ( list_next( refund_list ) );

	ptr += sprintf( ptr, "; " );

	return strdup( display );
}

TUITION_REFUND *tuition_refund_steady_state(
			TUITION_REFUND *tuition_refund,
			double refund_amount,
			double merchant_fees_expense )
{
	if ( !tuition_refund->tuition_payment
	||   !tuition_refund->tuition_payment->enrollment )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty tuition_payment or enrollment.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !tuition_refund->
			tuition_payment->
			enrollment->
			offering->
			revenue_account
	||   !*tuition_refund->
			tuition_payment->
			enrollment->
			offering->
			revenue_account )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty revenue_account.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (TUITION_REFUND *)0;
	}

	tuition_refund->net_refund_amount =
		education_net_refund_amount(
			refund_amount,
			merchant_fees_expense );

	return tuition_refund;
}

void tuition_refund_trigger(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"tuition_refund_trigger \"%s\" '%s' '%s' '%s' %d \"%s\" '%s' '%s' '%s' '%s'",
		student_full_name,
		student_street_address,
		course_name,
		season_name,
		year,
		payor_full_name,
		payor_street_address,
		payment_date_time,
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

void tuition_refund_list_trigger(
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;

	if ( !list_rewind( tuition_refund_list ) ) return;

	do {
		tuition_refund = list_get( tuition_refund_list );

		if ( !tuition_refund->tuition_payment
		||   !tuition_refund->tuition_payment->enrollment )
		{
			fprintf(stderr,
		"Warning in %s/%s()/%d: empty tuition_payment or enrollment.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			continue;
		}

		tuition_refund_trigger(
			tuition_refund->
				tuition_payment->
				enrollment->
				registration->
				student_entity->
				full_name,
			tuition_refund->
				tuition_payment->
				enrollment->
				registration->
				student_entity->
				street_address,
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				course->
				course_name,
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				semester->
				season_name,
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				semester->
				year,
			tuition_refund->
				tuition_payment->
				payor_entity->
				full_name,
			tuition_refund->
				tuition_payment->
				payor_entity->
				street_address,
			tuition_refund->
				tuition_payment->
				payment_date_time,
			tuition_refund->refund_date_time,
			"insert" /* state */ );

	} while ( list_next( tuition_refund_list ) );
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

LIST *tuition_refund_list_steady_state(
			LIST *tuition_refund_list,
			double refund_amount,
			double merchant_fees_expense )
{
	TUITION_REFUND *tuition_refund;

	if ( !list_rewind( tuition_refund_list ) ) return (LIST *)0;

	do {
		tuition_refund = list_get( tuition_refund_list );

		tuition_refund =
			tuition_refund_steady_state(
				tuition_refund,
				refund_amount,
				merchant_fees_expense );

	} while( list_next( tuition_refund_list ) );

	return tuition_refund_list;
}

char *tuition_refund_memo( char *tuition_name )
{
	static char payment_memo[ 128 ];

	if ( tuition_name && *tuition_name )
	{
		sprintf(payment_memo,
			"%s/%s",
			TUITION_REFUND_MEMO,
			tuition_name );
	}
	else
	{
		sprintf(payment_memo,
			"%s",
			TUITION_REFUND_MEMO );
	}
	return payment_memo;
}

void tuition_refund_list_payor_entity_insert(
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( tuition_refund_list ) ) return;

	insert_pipe =
		entity_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		tuition_refund = list_get( tuition_refund_list );

		if ( !tuition_refund->payor_entity )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: empty payor_entity.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			return;
		}

		entity_insert_pipe(
			insert_pipe,
			tuition_refund->payor_entity->full_name,
			tuition_refund->payor_entity->street_address,
			tuition_refund->payor_entity->email_address );

	} while ( list_next( tuition_refund_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Entity Errors' '' '^'	 ",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

void tuition_refund_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;
	char *cash_account_name;
	char *revenue_account;
	char *fees_expense;

	if ( !list_rewind( tuition_refund_list ) ) return;

	cash_account_name = entity_self_paypal_cash_account_name();
	fees_expense = account_fees_expense( (char *)0 );

	do {
		tuition_refund = list_get( tuition_refund_list );

		revenue_account =
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				revenue_account;

		tuition_refund_set_transaction(
			transaction_seconds_to_add,
			tuition_refund,
			cash_account_name,
			fees_expense,
			revenue_account );

	} while ( list_next( tuition_refund_list ) );
}

void tuition_refund_set_transaction(
			int *transaction_seconds_to_add,
			TUITION_REFUND *tuition_refund,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account )
{
	if ( ( tuition_refund->tuition_refund_transaction =
		tuition_refund_transaction(
			transaction_seconds_to_add,
			tuition_refund->payor_entity->full_name,
			tuition_refund->payor_entity->street_address,
			tuition_refund->refund_date_time,
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				course->
				course_name,
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				course->
				program->
				program_name,
			tuition_refund->refund_amount,
			tuition_refund->merchant_fees_expense,
			tuition_refund->net_refund_amount,
			cash_account_name,
			account_fees_expense,
			revenue_account ) ) )
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

LIST *tuition_refund_registration_list(
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;
	REGISTRATION *registration;
	LIST *registration_list;

	if ( !list_rewind( tuition_refund_list ) ) return (LIST *)0;

	registration_list = list_new();

	do {
		tuition_refund =
			list_get(
				tuition_refund_list );

		if ( !tuition_refund->tuition_payment )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty tuition_payment.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( !tuition_refund->
			tuition_payment->
			enrollment->
			registration )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		registration =
			tuition_refund->
				tuition_payment->
				enrollment->
				registration;

		list_set(
			registration_list,
			registration );

	} while ( list_next( tuition_refund_list ) );

	return registration_list;
}

LIST *tuition_refund_enrollment_list(
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

		if ( !tuition_refund->tuition_payment )
		{
			fprintf( stderr,
				"ERROR in %s/%s()/%d: empty tuition_payment.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		list_set(
			enrollment_list,
			tuition_refund->tuition_payment->enrollment );

	} while ( list_next( tuition_refund_list ) );

	return enrollment_list;
}

LIST *tuition_refund_list_paypal(
			char *season_name,
			int year,
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *semester_offering_list )
{
	LIST *tuition_refund_list = {0};
	PAYPAL_ITEM *paypal_item;
	OFFERING *offering;

	if ( !list_rewind( paypal_item_list ) ) return (LIST *)0;

	do {
		paypal_item = list_get( paypal_item_list );

		if ( paypal_item->taken ) continue;

		if ( !paypal_item->benefit_entity ) continue;

		if ( ( offering =
			offering_seek(
				paypal_item->item_data,
				semester_offering_list ) ) )
		{
			if ( !tuition_refund_list )
				tuition_refund_list =
					list_new();

			list_set(
				tuition_refund_list,
				tuition_refund_paypal(
					season_name,
					year,
					paypal_item->benefit_entity
						/* student_entity */,
					payor_entity,
					paypal_date_time,
					paypal_item->item_value,
					paypal_item->item_fee,
					offering ) );

			paypal_item->taken = 1;
		}
	} while ( list_next( paypal_item_list ) );

	return tuition_refund_list;
}

TUITION_REFUND *tuition_refund_paypal(
			char *season_name,
			int year,
			ENTITY *student_entity,
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			OFFERING *offering )
{
	TUITION_REFUND *tuition_refund;

	tuition_refund = tuition_refund_calloc();

	tuition_refund->payor_entity = payor_entity;

	tuition_refund->refund_date_time =
	tuition_refund->paypal_date_time = paypal_date_time;

	tuition_refund->refund_amount = item_value;
	tuition_refund->merchant_fees_expense = item_fee;

	tuition_refund->net_refund_amount =
		education_net_refund_amount(
			tuition_refund->refund_amount,
			tuition_refund->merchant_fees_expense );

	tuition_refund->tuition_payment =
		tuition_payment_new(
			student_entity,
			offering->course->course_name,
			season_name,
			year,
			payor_entity,
			paypal_date_time /* payment_date_time */ );

	if ( !tuition_refund->tuition_payment->enrollment )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty enrollment.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	tuition_refund->tuition_payment->enrollment->offering = offering;

	return tuition_refund;
}

void tuition_refund_list_registration_insert(
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( tuition_refund_list ) ) return;

	insert_pipe =
		registration_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		tuition_refund = list_get( tuition_refund_list );

		registration_insert_pipe(
			insert_pipe,
			tuition_refund->
				tuition_payment->
				enrollment->
				registration->
				student_entity->
				full_name,
			tuition_refund->
				tuition_payment->
				enrollment->
				registration->
				student_entity->
				street_address,
			tuition_refund->
				tuition_payment->
				enrollment->
				registration->
				season_name,
			tuition_refund->
				tuition_payment->
				enrollment->
				registration->
				year,
			tuition_refund->
				tuition_payment->
				enrollment->
				registration->
				registration_date_time );

	} while ( list_next( tuition_refund_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Refund Registration Errors";

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

