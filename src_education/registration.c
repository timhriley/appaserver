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
#include "registration.h"
#include "enrollment.h"
#include "entity.h"
#include "tuition_payment.h"
#include "tuition_refund.h"
#include "registration.h"
#include "enrollment.h"

double registration_tuition(
			LIST *enrollment_list,
			LIST *semester_offering_list )
{
	ENROLLMENT *enrollment;
	OFFERING *offering;
	double tuition;

	if ( !list_rewind( enrollment_list ) ) return 0.0;

	tuition = 0.0;

	do {
		enrollment = list_get( enrollment_list );

		/* Not found offering will report later. */
		/* ------------------------------------- */
		if ( !enrollment->offering ) return 0.0;

		if ( ! ( offering =
				offering_seek(
					enrollment->
						offering->
						course->
						course_name,
					semester_offering_list ) ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: offering not exists = [%s].\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				enrollment->offering->course->course_name );
			exit( 1 );
		}

		enrollment->offering->course_price = offering->course_price;

		tuition += enrollment->offering->course_price;

	} while ( list_next( enrollment_list ) );

	return tuition;
}

double registration_invoice_amount_due(
			double registration_tuition,
			double registration_tuition_payment_total )
{
	return registration_tuition - registration_tuition_payment_total;
}

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
			char *input,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list )
{
	char student_full_name[ 128 ];
	char student_street_address[ 128 ];
	char season_name[ 128 ];
	int year;
	char piece_buffer[ 128 ];
	REGISTRATION *registration;

	if ( !input || !*input ) return (REGISTRATION *)0;

	/* See: attribute_list registration */
	/* -------------------------------- */
	piece( student_full_name, SQL_DELIMITER, input, 0 );
	piece( student_street_address, SQL_DELIMITER, input, 1 );
	piece( season_name, SQL_DELIMITER, input, 2 );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	year = atoi( piece_buffer );

	registration =
		registration_new(
			entity_new(
				strdup( student_full_name ),
				strdup( student_street_address ) ),
			strdup( season_name ),
			year );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	registration->registration_date_time = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	registration->tuition = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	registration->tuition_payment_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	registration->tuition_refund_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	registration->invoice_amount_due = atof( piece_buffer );

	if ( fetch_enrollment_list )
	{
		registration->enrollment_list =
			registration_enrollment_list(
				registration->student_entity->full_name,
				registration->student_entity->street_address,
				registration->season_name,
				registration->year,
				fetch_offering,
				fetch_course,
				fetch_program );
	}

	if ( fetch_tuition_payment_list )
	{
		registration->registration_tuition_payment_list =
			registration_tuition_payment_list(
				registration->student_entity->full_name,
				registration->student_entity->street_address,
				registration->season_name,
				registration->year );
	}

	if ( fetch_tuition_refund_list )
	{
		registration->registration_tuition_refund_list =
			registration_tuition_refund_list(
				registration->student_entity->full_name,
				registration->student_entity->street_address,
				registration->season_name,
				registration->year );
	}

	return registration;
}

LIST *registration_system_list(
			char *sys_string,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list )
{
	LIST *registration_list = list_new();
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			registration_list,
			registration_parse(
				input,
				fetch_enrollment_list,
				fetch_offering,
				fetch_course,
				fetch_program,
				fetch_tuition_payment_list,
				fetch_tuition_refund_list ) );
	}
	pclose( input_pipe );
	return registration_list;
}

char *registration_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 "registration",
		 where );

	return strdup( sys_string );
}

REGISTRATION *registration_fetch(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list )
{
	return	registration_parse(
			pipe2string(
				registration_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					registration_primary_where(
						student_full_name,
						student_street_address,
						season_name,
						year ) ) ),
			fetch_enrollment_list,
			fetch_offering,
			fetch_course,
			fetch_program,
			fetch_tuition_payment_list,
			fetch_tuition_refund_list );
}

REGISTRATION *registration_seek(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			LIST *registration_list )
{
	REGISTRATION *registration;

	if ( !list_rewind( registration_list ) )
		return (REGISTRATION *)0;

	do {
		registration = list_get( registration_list );

		if ( strcmp(	registration->student_entity->full_name,
				student_full_name ) == 0
		&&   strcmp(	registration->student_entity->street_address,
				student_street_address ) == 0
		&&   strcmp(	registration->season_name,
				season_name ) == 0
		&&   registration->year == year )
		{
			return registration;
		}
	} while ( list_next( registration_list ) );

	return (REGISTRATION *)0;
}

char *registration_name_escape(
			char *full_name )
{
	return registration_escape_full_name( full_name );
}

char *registration_escape_name(
			char *full_name )
{
	return registration_escape_full_name( full_name );
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
			char *season_name,
			int year )
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
	registration->season_name = season_name;
	registration->year = year;
	return registration;
}

double registration_tuition_total(
			LIST *registration_list )
{
	REGISTRATION *registration;
	double tuition_total;

	if ( !list_rewind( registration_list ) ) return 0.0;

	tuition_total = 0.0;

	do {
		registration =
			list_get(
				registration_list );

		tuition_total += registration->tuition;

	} while ( list_next( registration_list ) );

	return tuition_total;
}

double registration_tuition_payment_total(
			LIST *tuition_payment_list )
{
	return tuition_payment_total( tuition_payment_list );
}

double registration_tuition_refund_total(
			LIST *tuition_refund_list )
{
	return tuition_refund_total( tuition_refund_list );
}

REGISTRATION *registration_steady_state(
			REGISTRATION *registration,
			LIST *enrollment_list,
			LIST *semester_offering_list )
{
	if ( !registration )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty registration\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	registration->tuition =
		registration_tuition(
			enrollment_list,
			semester_offering_list );

	registration->tuition_payment_total =
		registration_tuition_payment_total(
			registration->registration_tuition_payment_list );

	registration->invoice_amount_due =
		registration_invoice_amount_due(
			registration->tuition,
			registration->tuition_payment_total );

	return registration;
}

FILE *registration_update_open( void )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh			|"
		 "sql						 ",
		 REGISTRATION_TABLE,
		 REGISTRATION_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void registration_update(
			double tuition,
			double tuition_payment_total,
			double tuition_refund_total,
			double invoice_amount_due,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	FILE *update_pipe;

	update_pipe = registration_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%d^tuition^%.2lf\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 tuition );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^tuition_payment_total^%.2lf\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 tuition_payment_total );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^tuition_refund_total^%.2lf\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 tuition_refund_total );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^invoice_amount_due^%.2lf\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 invoice_amount_due );

	pclose( update_pipe );
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
			char *registration_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%d^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( student_full_name ),
		student_street_address,
		season_name,
		year,
		(registration_date_time)
			? registration_date_time
 			: "" );
}

void registration_fetch_update(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"registration_tuition.sh \"%s\" '%s' '%s' %d",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
		"registration_tuition_payment_total.sh \"%s\" '%s' '%s' %d",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
		"registration_tuition_refund_total.sh \"%s\" '%s' '%s' %d",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
		"registration_invoice_amount_due.sh \"%s\" '%s' '%s' %d",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( sys_string ) ){}
}

void registration_list_fetch_update(
			LIST *registration_list,
			char *season_name,
			int year )
{
	REGISTRATION *registration;

	if ( !list_rewind( registration_list ) ) return;

	do {
		registration = list_get( registration_list );

		registration_fetch_update(
			registration->student_entity->full_name,
			registration->student_entity->street_address,
			season_name,
			year );

	} while ( list_next( registration_list ) );
}

LIST *registration_enrollment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program )
{
	return
		enrollment_system_list(
			enrollment_sys_string(
				registration_primary_where(
					student_full_name,
					student_street_address,
					season_name,
					year ) ),
			fetch_offering,
			fetch_course,
			fetch_program,
			0 /* not fetch_registration */,
			0 /* not fetch_transaction */ );
}

LIST *registration_tuition_payment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	return
		tuition_payment_system_list(
			tuition_payment_sys_string(
				registration_primary_where(
					student_full_name,
					student_street_address,
					season_name,
					year ) ),
			0 /* not fetch_registration */,
			1 /* fetch_enrollment_list */,
			1 /* fetch_offering */,
			1 /* fetch_course */,
			0 /* not fetch_program */,
			0 /* not fetch_transaction */ );
}

LIST *registration_tuition_refund_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	return
		tuition_refund_system_list(
			tuition_refund_sys_string(
				registration_primary_where(
					student_full_name,
					student_street_address,
					season_name,
					year ) ),
			0 /* not fetch_registration */,
			1 /* fetch_enrollment_list */,
			1 /* fetch_offering */,
			1 /* fetch_course */,
			1 /* fetch_program */ );
}

FILE *registration_enrollment_insert_open(
			char *error_filename )
{
	return enrollment_insert_open( error_filename );
}

void registration_enrollment_list_insert(
			FILE *insert_pipe,
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return;

	do {
		enrollment = list_get( enrollment_list );

		enrollment_insert_pipe(
			insert_pipe,
			enrollment->
				registration->
				student_entity->
				full_name,
			enrollment->
				registration->
				student_entity->
				street_address,
			enrollment->
				offering->
				course->
				course_name,
			enrollment->
				offering->
				semester->
				season_name,
			enrollment->
				offering->
				semester->
				year,
			enrollment->transaction_date_time );

	} while ( list_next( enrollment_list ) );
}

TRANSACTION *registration_enrollment_seek_transaction(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return (TRANSACTION *)0;

	do {
		enrollment = list_get( enrollment_list );

		if ( enrollment->enrollment_transaction )
			return enrollment->enrollment_transaction;

	} while ( list_next( enrollment_list ) );
	return (TRANSACTION *)0;
}

LIST *registration_course_name_list(
			LIST *registration_list )
{
	LIST *course_name_list;
	REGISTRATION *registration;

	if ( !list_rewind( registration_list ) ) return (LIST *)0;

	course_name_list = list_new();

	do {
		registration = list_get( registration_list );

		list_unique_list(
			course_name_list,
			enrollment_course_name_list(
				registration->
					enrollment_list ) );

	} while ( list_next( registration_list ) );

	return course_name_list;
}

LIST *registration_list( char *where )
{
	return	registration_system_list(
			registration_sys_string( where ),
			1 /* fetch_enrollment_list */,
			0 /* not fetch_offering */,
			0 /* not fetch_course */,
			0 /* not fetch_program */,
			1 /* fetch_tuition_payment_list */,
			1 /* fetch_tuition_refund_list */ );
}

LIST *registration_list_paypal(
			char *season_name,
			int year,
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *semester_offering_list )
{
	LIST *registration_list = {0};
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
			if ( !registration_list )
				registration_list =
					list_new();

			list_set(
				registration_list,
				registration_paypal(
					season_name,
					year,
					paypal_item->benefit_entity,
					payor_entity,
					paypal_date_time,
					paypal_item->item_value,
					paypal_item->item_fee,
					offering ) );

			paypal_item->taken = 1;
		}

	} while ( list_next( paypal_item_list ) );

	return registration_list;
}

REGISTRATION *registration_paypal(
			char *season_name,
			int year,
			ENTITY *student_entity,
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			OFFERING *offering )
{
	REGISTRATION *registration;
	ENROLLMENT *enrollment;

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
			season_name,
			year );

	enrollment = enrollment_calloc();

	enrollment->payor_entity = payor_entity;

	enrollment->payment_date_time =
	enrollment->paypal_date_time = paypal_date_time;

	enrollment->registration->registration_date_time =
		enrollment->payment_date_time;

	enrollment =
		enrollment_new(
			student_entity,
			offering->course->course_name,
			semester_new(
				season_name,
				year ) );

	enrollment->registration = registration;
	enrollment->offering = offering;

	enrollment->registration->enrollment_list =
		list_new();

	list_set( 
		enrollment->registration->enrollment_list,
		enrollment );

	enrollment->registration->registration_date_time =
		paypal_date_time;

	enrollment->course_name = offering->course->course_name;

	enrollment->payment_amount =
	enrollment->enrollment_receivable_credit_amount = item_value;

	enrollment->merchant_fees_expense = item_fee;

	enrollment->net_payment_amount =
		education_net_payment_amount(
			enrollment->payment_amount,
			enrollment->merchant_fees_expense );

	return enrollment;
}

