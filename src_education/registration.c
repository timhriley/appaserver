/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/registration.c	*/
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
#include "environ.h"
#include "list.h"
#include "paypal_upload.h"
#include "paypal_deposit.h"
#include "enrollment.h"
#include "entity.h"
#include "paypal_item.h"
#include "education.h"
#include "tuition_payment.h"
#include "tuition_refund.h"
#include "registration.h"

char *registration_primary_where(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	char static where[ 512 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "season_name = '%s' and	"
		 "year = %d			",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 registration_escape_full_name( student_full_name ),
		 student_street_address,
		 season_name,
		 year );

	return where;
}

REGISTRATION *registration_parse(
			char *input )
{
	char student_full_name[ 128 ];
	char student_street_address[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char registration_date_time[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char tuition[ 128 ];
	char tuition_payment_total[ 128 ];
	char tuition_refund_total[ 128 ];
	char invoice_amount_due[ 128 ];
	REGISTRATION *registration;

	if ( !input || !*input ) return (REGISTRATION *)0;

	/* See: attribute_list registration */
	/* -------------------------------- */
	piece( student_full_name, SQL_DELIMITER, input, 0 );
	piece( student_street_address, SQL_DELIMITER, input, 1 );
	piece( season_name, SQL_DELIMITER, input, 2 );
	piece( year, SQL_DELIMITER, input, 3 );

	registration =
		registration_new(
			entity_new(
				strdup( student_full_name ),
				strdup( student_street_address ) ),
			semester_new(
				strdup( season_name ),
				atoi( year ) ) );

	piece( registration_date_time, SQL_DELIMITER, input, 4 );
	registration->registration_date_time = strdup( registration_date_time );

	piece( payor_full_name, SQL_DELIMITER, input, 5 );

	if ( !*payor_full_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty payor_full_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (REGISTRATION *)0;
	}

	piece( payor_street_address, SQL_DELIMITER, input, 6 );

	registration->payor_entity =
		entity_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ) );

	piece( tuition, SQL_DELIMITER, input, 7 );
	registration->tuition = atof( tuition );

	piece( tuition_payment_total, SQL_DELIMITER, input, 8 );
	registration->tuition_payment_total = atof( tuition_payment_total );

	piece( tuition_refund_total, SQL_DELIMITER, input, 9 );
	registration->tuition_refund_total = atof( tuition_refund_total );

	piece( invoice_amount_due, SQL_DELIMITER, input, 10 );
	registration->invoice_amount_due = atof( invoice_amount_due );

	return registration;
}

LIST *registration_system_list(
			char *system_string )
{
	LIST *registration_list = list_new();
	char input[ 1024 ];
	FILE *input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			registration_list,
			registration_parse(
				input ) );
	}
	pclose( input_pipe );
	return registration_list;
}

char *registration_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "select.sh '*' %s \"%s\" select",
		 "registration",
		 where );

	return strdup( system_string );
}

REGISTRATION *registration_fetch(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	return	registration_parse(
			string_pipe_fetch(
				registration_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					registration_primary_where(
						student_full_name,
						student_street_address,
						season_name,
						year ) ) ) );
}

char *registration_escape_full_name(
			char *full_name )
{
	static char escape_full_name[ 256 ];

	string_escape_quote( escape_full_name, full_name );
	return escape_full_name;
}

REGISTRATION *registration_new(
			ENTITY *student_entity,
			SEMESTER *semester )
{
	REGISTRATION *registration;

	if ( ! ( registration = calloc( 1, sizeof( REGISTRATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	registration->student_entity = student_entity;
	registration->semester = semester;

	return registration;
}

void registration_list_insert(
			LIST *registration_list )
{
	REGISTRATION *registration;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( registration_list ) ) return;

	insert_pipe =
		registration_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		registration = list_get( registration_list );

		registration_insert_pipe(
			insert_pipe,
			registration->student_entity->full_name,
			registration->student_entity->street_address,
			registration->semester->season_name,
			registration->semester->year,
			registration->registration_date_time,
			registration->payor_entity->full_name,
			registration->payor_entity->street_address );

	} while ( list_next( registration_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						   |"
			"queue_top_bottom_lines.e 300			   |"
			"html_table.e 'Insert Registration Errors' '' '^'",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

FILE *registration_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"grep -vi duplicate					 |"
		"cat >%s						  ",
		REGISTRATION_TABLE,
		REGISTRATION_INSERT_COLUMNS,
		'n',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void registration_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *registration_date_time,
			char *payor_full_name,
			char *payor_street_address )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%d^%s^%s^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		registration_escape_full_name( student_full_name ),
		student_street_address,
		season_name,
		year,
		registration_date_time,
		entity_escape_full_name( payor_full_name ),
		payor_street_address );
}

void registration_fetch_update(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"registration_enrollment_tuition_total.sh \"%s\" '%s' '%s' %d",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( system_string ) ){}

	sprintf(system_string,
		"registration_course_drop_total.sh \"%s\" '%s' '%s' %d",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( system_string ) ){}

	sprintf(system_string,
		"registration_tuition_payment_total.sh \"%s\" '%s' '%s' %d",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( system_string ) ){}

	sprintf(system_string,
		"registration_tuition_refund_total.sh \"%s\" '%s' '%s' %d",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( system_string ) ){}

	sprintf(system_string,
		"registration_invoice_amount_due.sh \"%s\" '%s' '%s' %d",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( system_string ) ){}
}

void registration_list_fetch_update(
			LIST *registration_list )
{
	REGISTRATION *registration;

	if ( !list_rewind( registration_list ) ) return;

	do {
		registration = list_get( registration_list );

		registration_fetch_update(
			registration->student_entity->full_name,
			registration->student_entity->street_address,
			registration->semester->season_name,
			registration->semester->year );

	} while ( list_next( registration_list ) );
}

LIST *registration_list_offering_list(
			LIST *registration_list )
{
	LIST *offering_list;
	REGISTRATION *registration;

	if ( !list_rewind( registration_list ) ) return (LIST *)0;

	offering_list = list_new();

	do {
		registration = list_get( registration_list );

		list_set_list(
			offering_list,
			enrollment_list_offering_list(
				registration->enrollment_list ) );

	} while ( list_next( registration_list ) );

	return offering_list;
}

LIST *registration_list_paypal(
			char *season_name,
			int year,
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *semester_offering_list,
			LIST *paypal_deposit_list )
{
	LIST *registration_list = {0};
	PAYPAL_ITEM *paypal_item;
	OFFERING *offering;
	SEMESTER *semester;

	if ( !list_rewind( paypal_item_list ) ) return (LIST *)0;

	semester =
		semester_new(
			season_name,
			year );
	do {
		paypal_item = list_get( paypal_item_list );

		if ( paypal_item->taken ) continue;

		if ( !paypal_item->benefit_entity ) continue;

		if ( ( offering =
			offering_list_seek( 
				paypal_item->item_data,
				semester_offering_list ) ) )
		{
			if ( !registration_list )
				registration_list =
					list_new();

			list_set(
				registration_list,
				registration_paypal(
					semester,
					paypal_item->benefit_entity,
					payor_entity,
					paypal_date_time,
					paypal_item->item_value,
					paypal_item->item_fee,
					offering,
					paypal_deposit_list ) );

			paypal_item->taken = 1;
		}

	} while ( list_next( paypal_item_list ) );

	return registration_list;
}

REGISTRATION *registration_paypal(
			SEMESTER *semester,
			ENTITY *student_entity,
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			OFFERING *offering,
			LIST *paypal_deposit_list )
{
	REGISTRATION *registration;
	ENROLLMENT *enrollment;
	TUITION_PAYMENT *tuition_payment;
	TUITION_REFUND *tuition_refund;

	if ( !student_entity || !payor_entity )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: missing student_entity or payor_entity.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	registration =
		registration_new(
			student_entity,
			semester );

	registration->registration_date_time = paypal_date_time;
	registration->enrollment_list = list_new();

	if ( item_value > 0.0 )
	{
		registration->tuition_payment_list = list_new();

		enrollment =
			enrollment_new(
				student_entity,
				offering->course_name,
				registration->semester );

		enrollment->enrollment_date_time = paypal_date_time;

		list_set( registration->enrollment_list, enrollment );

		tuition_payment =
			tuition_payment_new(
				student_entity,
				registration->semester,
				paypal_date_time
					/* payment_date_time */ );

		tuition_payment->payor_entity = payor_entity;
		tuition_payment->paypal_date_time = paypal_date_time;
		tuition_payment->payment_amount = item_value;
		tuition_payment->merchant_fees_expense = item_fee;
		tuition_payment->registration = registration;

		tuition_payment->net_payment_amount =
			education_net_payment_amount(
				tuition_payment->payment_amount,
				tuition_payment->merchant_fees_expense );

		list_set( registration->tuition_payment_list, tuition_payment );
	}
	else
	if ( item_value < 0.0 )
	{
		registration->tuition_refund_list = list_new();

		if ( ! ( enrollment =
			     enrollment_integrity_fetch(
				student_entity->full_name,
				student_entity->street_address,
				offering->course->course_name,
				registration->semester->season_name,
				registration->semester->year ) ) )
		{
			if ( ! ( enrollment =
				     enrollment_list_seek(
					student_entity->full_name,
					student_entity->street_address,
					offering->course->course_name,
					registration->semester->season_name,
					registration->semester->year,
					paypal_deposit_list_enrollment_list(
						paypal_deposit_list ) ) ) )
			{
				return (REGISTRATION *)0;
			}
		}

		list_set( registration->enrollment_list, enrollment );

		tuition_refund =
			tuition_refund_new(
				student_entity,
				registration->semester,
				paypal_date_time
					/* refund_date_time */ );

		tuition_refund->payor_entity = payor_entity;
		tuition_refund->paypal_date_time = paypal_date_time;
		tuition_refund->refund_amount = item_value;
		tuition_refund->merchant_fees_expense = item_fee;
		tuition_refund->registration = registration;

		tuition_refund->net_refund_amount =
			education_net_refund_amount(
				tuition_refund->refund_amount,
				tuition_refund->merchant_fees_expense );

		list_set( registration->tuition_refund_list, tuition_refund );
	}
	return registration;
}

LIST *registration_list_enrollment_list(
			LIST *registration_list )
{
	LIST *enrollment_list;
	REGISTRATION *registration;

	if ( !list_rewind( registration_list ) ) return (LIST *)0;

	enrollment_list = list_new();

	do {
		registration = list_get( registration_list );

		list_set_list(
			enrollment_list,
			registration->enrollment_list );

	} while ( list_next( registration_list ) );

	return enrollment_list;
}

LIST *registration_list_tuition_payment_list(
			LIST *registration_list )
{
	LIST *tuition_payment_list;
	REGISTRATION *registration;

	if ( !list_rewind( registration_list ) ) return (LIST *)0;

	tuition_payment_list = list_new();

	do {
		registration = list_get( registration_list );

		if ( list_length( registration->tuition_payment_list ) )
		{
			list_set_list(
				tuition_payment_list,
				registration->tuition_payment_list );
		}

	} while ( list_next( registration_list ) );

	return tuition_payment_list;
}

LIST *registration_list_tuition_refund_list(
			LIST *registration_list )
{
	LIST *tuition_refund_list;
	REGISTRATION *registration;

	if ( !list_rewind( registration_list ) ) return (LIST *)0;

	tuition_refund_list = list_new();

	do {
		registration = list_get( registration_list );

		if ( list_length( registration->tuition_refund_list ) )
		{
			list_set_list(
				tuition_refund_list,
				registration->tuition_refund_list );
		}

	} while ( list_next( registration_list ) );

	return tuition_refund_list;
}

