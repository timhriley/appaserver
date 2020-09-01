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
#include "entity.h"
#include "payment.h"
#include "registration.h"

REGISTRATION *registration_getset(
			LIST *semester_registration_list,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year )
{
	REGISTRATION *registration;

	if ( ! ( registration =
			registration_seek(
				semester_registration_list,
				student_full_name,
				street_address,
				season_name,
				year ) ) )
	{
		registration =
			registration_new(
				strdup( student_full_name ),
				strdup( street_address ),
				strdup( season_name ),
				year );

		list_set( semester_registration_list, registration );
	}
	return registration;
}

double registration_tuition(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;
	OFFERING *offering;
	double tuition;

	if ( !list_rewind( enrollment_list ) ) return 0.0;

	tuition = 0.0;

	do {
		enrollment = list_get( enrollment_list );

		if ( !enrollment->offering )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: offering is empty.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

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

	} while ( list_next( enrollment_list ) );

	return tuition;
}

double registration_invoice_amount_due(
			double registration_tuition,
			double payment_total )
{
	return registration_tuition - payment_total;
}

void registration_payment_list_refresh(
			LIST *registration_payment_list )
{
if ( registration_payment_list ) {}
}

void registration_enrollment_list_refresh(
			LIST *registration_enrollment_list )
{
if ( registration_enrollment_list ) {}
}

void registration_refresh(
			double registration_tuition,
			double registration_payment_total,
			double registration_invoice_amount_due,
			char *student_full_name,
			char *street_address,
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
		 street_address,
		 season_name,
		 year,
		 registration_tuition );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^payment_total^%.2lf\n",
		 student_full_name,
		 street_address,
		 season_name,
		 year,
		 registration_payment_total );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^invoice_amount_due^%.2lf\n",
		 student_full_name,
		 street_address,
		 season_name,
		 year,
		 registration_invoice_amount_due );

	pclose( update_pipe );
}

char *registration_primary_where(
			char *student_full_name,
			char *street_address,
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
		 street_address,
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
		"payment_total,"
		"invoice_amount_due,"
		"registration_date_time";
}

REGISTRATION *registration_parse(
			char *input,
			boolean fetch_enrollment_list )
{
	char student_full_name[ 128 ];
	char street_address[ 128 ];
	char season_name[ 128 ];
	int year;
	char piece_buffer[ 128 ];
	REGISTRATION *registration;

	if ( !input || !*input ) return (REGISTRATION *)0;

	piece( student_full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( season_name, SQL_DELIMITER, input, 2 );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	year = atoi( piece_buffer );

	registration =
		registration_new(
			strdup( student_full_name ),
			strdup( street_address ),
			strdup( season_name ),
			year );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	registration->registration_tuition = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	registration->registration_payment_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	registration->registration_invoice_amount_due = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	registration->registration_date_time = strdup( piece_buffer );

	if ( fetch_enrollment_list )
	{
		registration->registration_enrollment_list =
			registration_enrollment_list(
				registration->student_full_name,
				registration->street_address,
				registration->season_name,
				registration->year );
	}

	return registration;
}

LIST *registration_system_list( char *sys_string )
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
				1 /* fetch_enrollment_list */ ) );
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
			char *street_address,
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
						street_address,
						season_name,
						year ) ) ),
			fetch_enrollment_list );
}

REGISTRATION *registration_seek(
			LIST *semester_registration_list,
			char *student_full_name,
			char *street_address,
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
		&&   strcmp(	registration->street_address,
				street_address ) == 0
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
			char *street_address,
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
	registration->street_address = street_address;
	registration->season_name = season_name;
	registration->year = year;
	return registration;
}

LIST *registration_enrollment_list(
			char *student_full_name,
			char *street_address,
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
					street_address,
					season_name,
					year ) ) );
}

LIST *registration_payment_list(
			LIST *registration_enrollment_list )
{
	ENROLLMENT *enrollment;
	LIST *payment_list;

	if ( !list_rewind( registration_enrollment_list ) )
		return (LIST *)0;

	payment_list = list_new();

	do {
		enrollment =
			list_get(
				registration_enrollment_list );

		list_append_list(
			payment_list,
			enrollment->
				enrollment_payment_list );

	} while ( list_next( registration_enrollment_list ) );

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

		tuition_total +=
			registration_tuition(
				registration->
					registration_enrollment_list );

	} while ( list_next( registration_list ) );

	return tuition_total;
}

double registration_payment_total(
			LIST *enrollment_list )
{
	return payment_total( enrollment_list );
}

void registration_update(
			double registration_tuition,
			double registration_payment_total,
			double registration_invoice_amount_due,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year )
{
if ( registration_tuition ){}
if ( registration_payment_total ){}
if ( registration_invoice_amount_due ){}
if ( student_full_name ){}
if ( street_address ){}
if ( season_name ){}
if ( year ){}

}

