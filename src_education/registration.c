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
#include "enrollment.h"
#include "payment.h"
#include "registration.h"

LIST *registration_enrollment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	return enrollment_registration_enrollment_list(
			student_full_name,
			student_street_address,
			season_name,
			year );
}

LIST *registration_payment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	return payment_registration_payment_list(
			student_full_name,
			student_street_address,
			season_name,
			year );
}

REGISTRATION *registration_getset(
			LIST *semester_registration_list,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	REGISTRATION *registration;

	if ( ! ( registration =
			registration_seek(
				semester_registration_list,
				student_full_name,
				student_street_address,
				season_name,
				year ) ) )
	{
		registration =
			registration_new(
				student_full_name,
				student_street_address,
				season_name,
				year );

		list_set( semester_registration_list, registration );
	}
	return registration;
}

double registration_tuition(
			LIST *registration_enrollment_list )
{
	ENROLLMENT *enrollment;
	OFFERING *offering;
	double tuition;

	if ( !list_rewind( registration_enrollment_list ) ) return 0.0;

	tuition = 0.0;

	do {
		enrollment = list_get( registration_enrollment_list );

		offering = enrollment->offering;

		if ( !offering->course )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: course is empty.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		tuition += offering->course->course_price;

	} while ( list_next( registration_enrollment_list ) );

	return tuition;
}

double registration_payment_total(
			LIST *registration_payment_list )
{
	PAYMENT *payment;
	double payment_total;

	if ( !list_rewind( registration_payment_list ) ) return 0.0;

	payment_total = 0.0;

	do {
		payment = list_get( registration_payment_list );

		if ( payment->payment_amount_calculate )
			payment_total += payment->payment_amount_calculate;
		else
			payment_total += payment->payment_amount_database;

	} while ( list_next( registration_payment_list ) );

	return payment_total;
}

double registration_invoice_amount_due(
			double registration_tuition,
			double registration_payment_total )
{
	return registration_tuition - registration_payment_total;
}

TRANSACTION *registration_revenue_transaction(
			char *student_full_name,
			char *street_address,
			char *registration_date_time,
			double registration_tuition,
			char *ledger_receivable_account,
			char *program_name,
			char *offering_revenue_account )
{
	return (TRANSACTION *)0;
}

void registration_payment_list_refresh(
			LIST *registration_payment_list )
{
}

void registration_enrollment_list_refresh(
			LIST *registration_enrollment_list )
{
}

/* Returns the true transaction_date_time */
/* -------------------------------------- */
char *registration_refresh(
			double registration_tuition,
			double registration_payment_total,
			double registration_invoice_amount_due,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];
	FILE *update_pipe;

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 "registration",
		 REGISTRATION_PRIMARY_KEY );

	update_pipe = popen( sys_string, "w" );

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
		 registration_payment_total );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^invoice_amount_due^%.2lf\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 registration_invoice_amount_due );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^transaction_date_time^%s\n",
		 student_full_name,
		 student_street_address,
		 season_name,
		 year,
		 transaction_date_time );

	pclose( update_pipe );

	/* Need to return the true one. */
	/* ---------------------------- */
	return transaction_date_time;
}

/* Safely returns heap memory */
/* -------------------------- */
char *registration_primary_where(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	char where[ 1024 ];

	sprintf( where,
"full_name = '%s' street_address = '%s' and season_name = '%s' and year = %d",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 registration_escape_full_name( student_full_name ),
		 student_street_address,
		 season_name,
		 year );

	return strdup( where );
}

char *registration_select( void )
{
	return "full_name,street_address,season_name,year,tuition,payment_total,invoice_amount_due,registration_date_time,transaction_date_time";
}

REGISTRATION *registration_parse( char *input_buffer )
{
	char student_full_name[ 128 ];
	char student_street_address[ 128 ];
	char season_name[ 128 ];
	int year;
	char piece_buffer[ 128 ];
	REGISTRATION *registration;

	if ( !input_buffer ) return (REGISTRATION *)0;

	piece( student_full_name, SQL_DELIMITER, input_buffer, 0 );
	piece( student_street_address, SQL_DELIMITER, input_buffer, 1 );
	piece( season_name, SQL_DELIMITER, input_buffer, 2 );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 3 );
	year = atoi( piece_buffer );

	registration = registration_new(
			strdup( student_full_name ),
			strdup( student_street_address ),
			strdup( season_name ),
			year );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 4 );
	registration->registration_tuition = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 5 );
	registration->registration_payment_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 6 );
	registration->registration_invoice_amount_due = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 7 );
	registration->registration_date_time = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 8 );
	registration->registration_revenue_transaction =
		ledger_transaction_fetch(
			environment_get( "DATABASE" )
				/* application_name */,
			registration->student_full_name,
			registration->student_street_address,
			piece_buffer /* transaction_date_time */ );

	registration->registration_enrollment_list =
		registration_enrollment_list(
			registration->student_full_name,
			registration->student_street_address,
			registration->season_name,
			registration->year );

	registration->registration_payment_list =
		registration_payment_list(
			registration->student_full_name,
			registration->student_street_address,
			registration->season_name,
			registration->year );

	return registration;
}

REGISTRATION *registration_fetch(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 registration_select(),
		 "registration",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 registration_primary_where(
			student_full_name,
			student_street_address,
			season_name,
			year ),
		 registration_select() );

	return registration_parse( pipe2string( sys_string ) );
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

		if ( strcmp(	registration->student_full_name,
				student_full_name ) == 0
		&&   strcmp(	registration->student_street_address,
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

char *registration_escape_full_name(
			char *full_name )
{
	static char escape_full_name[ 256 ];

	string_escape_quote( escape_full_name, full_name );
	return escape_full_name;
}

REGISTRATION *registration_new(
			char *student_full_name,
			char *student_street_address,
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

	registration->student_full_name = student_full_name;
	registration->student_street_address = student_street_address;
	registration->season_name = season_name;
	registration->year = year;
	return registration;
}

