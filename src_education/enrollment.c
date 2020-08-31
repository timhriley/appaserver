/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/enrollment.c		*/
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
#include "list.h"
#include "transaction.h"
#include "registration.h"
#include "payment.h"
#include "offering.h"
#include "enrollment.h"

ENROLLMENT *enrollment_new(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	ENROLLMENT *enrollment;

	if ( ! ( enrollment = calloc( 1, sizeof( ENROLLMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	enrollment->offering =
		offering_new(
			course_name,
			season_name,
			year );

	enrollment->registration =
		registration_new(
			student_full_name,
			street_address,
			season_name,
			year );

	return enrollment;
}

ENROLLMENT *enrollment_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char course_name[ 128 ];
	char season_name[ 128 ];
	char piece_buffer[ 128 ];
	int year;
	ENROLLMENT *enrollment;

	if ( !input || !*input ) return (ENROLLMENT *)0;

	/* See: attribute_list enrollment */
	/* ------------------------------ */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( course_name, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	year = atoi( piece_buffer );

	enrollment =
		enrollment_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( course_name ),
			strdup( season_name ),
			year );


	enrollment->offering =
		/* ------------------------------------ */
		/* Fetch offering->course->course_price */
		/* ------------------------------------ */
		offering_fetch(
			course_name,
			season_name,
			year,
			1 /* fetch_course */,
			0 /* not fetch_enrollment_list */ );

	enrollment->registration =
		registration_new(
			full_name,
			street_address,
			season_name,
			year );

	enrollment->enrollment_payment_list
	return enrollment;
}

char *enrollment_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 "enrollment",
		 where );

	return strdup( sys_string );
}

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *street_address,
			char *season_name,
			char *course_name,
			int year )
{
	return enrollment_parse(
			pipe2string(
				enrollment_sys_string(
					enrollment_primary_where(
						student_full_name,
						street_address,
						season_name,
						course_name,
						year ) ) ) );
}

LIST *enrollment_system_list( char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *enrollment_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set( enrollment_list, enrollment_parse( input ) );
	}
	pclose( input_pipe );
	return enrollment_list;
}

FILE *enrollment_update_open( void )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"update_statement table=%s key=\"%s\" carrot=y",
		ENROLLMENT_TABLE,
		ENROLLMENT_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void enrollment_update(
			char *transaction_date_time,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	FILE *update_pipe = enrollment_update_open();

	fprintf(update_pipe,
		"%s^%s^%s^%s^%d^transaction_date_time^%s\n",
		student_full_name,
		street_address,
		course_name,
		season_name,
		year,
		transaction_date_time );

	pclose( update_pipe );
}

/* Safely returns heap memory */
/* -------------------------- */
char *enrollment_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	char where[ 1024 ];

	sprintf(where,
		"full_name = '%s' and		"
		"street_address = '%s' and	"
		"course_name = '%s' and		"
		"season_name = '%s' and		"
		"year = %d			",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 registration_escape_full_name( student_full_name ),
		 street_address,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 offering_escape_course_name( course_name ),
		 season_name,
		 year );

	return strdup( where );
}

TRANSACTION *enrollment_transaction(
			char *student_full_name,
			char *street_address,
			char *registration_date_time
				/* transaction_date_time */,
			char *program_name,
			double offering_course_price,
			char *account_receivable,
			char *offering_revenue_account )
{
	TRANSACTION *transaction;

	transaction =
		transaction_new(
			student_full_name,
			street_address,
			registration_date_time );

	transaction->program_name = program_name;
	transaction->transaction_amount = offering_course_price;

	transaction->journal_list =
		transaction_binary_journal_list(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			account_receivable,
			offering_revenue_account );

	return transaction;
}

ENROLLMENT *enrollment_getset(
			LIST *enrollment_list,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	ENROLLMENT *enrollment;

	if ( ! ( enrollment =
			enrollment_fetch(
				student_full_name,
				street_address,
				course_name,
				season_name,
				year ) ) )
	{
		enrollment =
			enrollment_new(
				strdup( student_full_name ),
				strdup( street_address ),
				strdup( course_name ),
				strdup( season_name ),
				year );

		list_set( enrollment_list, enrollment );
	}
	return enrollment;
}

LIST *enrollment_payment_list(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	return	payment_system_list(
			payment_sys_string(
				enrollment_primary_where(
					student_full_name,
					street_address,
					course_name,
					season_name,
					year ) ) );
}

