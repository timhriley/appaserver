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

REGISTRATION *registration_getset(
			LIST *registration_list,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	REGISTRATION *registration = {0};

	if ( ! ( registration =
			registration_seek(
				registration_list,
				student_full_name,
				student_street_address,
				season_name,
				year ) ) )
	{
		list_set(
			registration_list,
			( registration =
				registration_new(
					entity_new(
						strdup(
						   student_full_name ),
						strdup(
						   student_street_address ) ),
					strdup( season_name ),
					year ) ) );
	}
	return registration;
}

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

char *registration_select( void )
{
	return	"full_name,"
		"street_address,"
		"season_name,"
		"year,"
		"tuition,"
		"tuition_payment_total,"
		"invoice_amount_due,"
		"registration_date_time";
}

REGISTRATION *registration_parse(
			char *input,
			boolean fetch_enrollment_list )
{
	char student_full_name[ 128 ];
	char student_street_address[ 128 ];
	char season_name[ 128 ];
	int year;
	char piece_buffer[ 128 ];
	REGISTRATION *registration;

	if ( !input || !*input ) return (REGISTRATION *)0;

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
	registration->tuition = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	registration->tuition_payment_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	registration->invoice_amount_due = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	registration->registration_date_time = strdup( piece_buffer );

	if ( fetch_enrollment_list )
	{
		registration->enrollment_list =
			registration_enrollment_list(
				registration->student_entity->full_name,
				registration->student_entity->street_address,
				registration->season_name,
				registration->year );
	}

	return registration;
}

LIST *registration_system_list(	char *sys_string,
				boolean fetch_enrollment_list )
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
				fetch_enrollment_list ) );
	}
	pclose( input_pipe );
	return registration_list;
}

char *registration_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '%s' %s \"%s\" select",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 registration_select(),
		 "registration",
		 where );

	return strdup( sys_string );
}

REGISTRATION *registration_fetch(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			boolean fetch_enrollment_list )
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
			fetch_enrollment_list );
}

REGISTRATION *registration_seek(
			LIST *semester_registration_list,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	REGISTRATION *registration;

	if ( !list_rewind( semester_registration_list ) )
		return (REGISTRATION *)0;

	do {
		registration = list_get( semester_registration_list );

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
	} while ( list_next( semester_registration_list ) );

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

double registration_fetch_invoice_amount_due(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	REGISTRATION *registration =
		registration_fetch(
			student_full_name,
			student_street_address,
			season_name,
			year,
			0 /* not fetch_enrollment_list */ );

	if ( registration )
		return registration->invoice_amount_due;
	else
		return 0.0;
}

LIST *registration_enrollment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	return	enrollment_system_list(
			enrollment_sys_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				registration_primary_where(
					student_full_name,
					student_street_address,
					season_name,
					year ) ),
			0 /* not fetch_tuition_payment_list */,
			0 /* not fetch_tuition_refund_list */,
			1 /* fetch_offering */,
			0 /* not fetch_registration */ );
}

LIST *registration_tuition_payment_list(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;
	LIST *payment_list;

	if ( !list_rewind( enrollment_list ) )
		return (LIST *)0;

	payment_list = list_new();

	do {
		enrollment =
			list_get(
				enrollment_list );

		list_append_list(
			payment_list,
			enrollment->
				tuition_payment_list );

	} while ( list_next( enrollment_list ) );

	return payment_list;
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
			LIST *registration_enrollment_list,
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
			registration_enrollment_list,
			semester_offering_list );

	registration->tuition_payment_list =
		registration_tuition_payment_list(
			registration_enrollment_list );

	registration->tuition_payment_total =
		registration_tuition_payment_total(
			registration->tuition_payment_list );

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
			double registration_tuition,
			double payment_total,
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
		 registration_tuition );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^payment_total^%.2lf\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 payment_total );

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
		'y',
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
		registration_date_time );
}

void registration_fetch_update(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"registration_tuition.sh \"%s\" '%s' '%s' %d y",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
		"registration_tuition_payment_total.sh \"%s\" '%s' '%s' %d y",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
		"registration_tuition_refund_total.sh \"%s\" '%s' '%s' %d y",
		student_full_name,
		student_street_address,
		season_name,
		year );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
		"registration_invoice_amount_due.sh \"%s\" '%s' '%s' %d y",
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

