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
#include "account.h"
#include "transaction.h"
#include "journal.h"
#include "entity.h"
#include "paypal_upload.h"
#include "enrollment.h"
#include "registration.h"
#include "offering.h"
#include "course.h"
#include "enrollment.h"
#include "registration.h"
#include "tuition_payment.h"
#include "tuition_refund.h"

char *enrollment_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 "enrollment",
		 where );

	return strdup( sys_string );
}

ENROLLMENT *enrollment_new(
			ENTITY *student_entity,
			char *course_name,
			char *season_name,
			int year,
			REGISTRATION *registration,
			OFFERING *offering )
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

	if ( registration )
	{
		enrollment->registration = registration;
	}
	else
	{
		enrollment->registration =
			registration_new(
				student_entity,
				season_name,
				year );
	}

	if ( offering )
	{
		enrollment->offering = offering;
	}
	else
	{
		enrollment->offering =
			offering_new(
				course_name,
				season_name,
				year );
	}

	return enrollment;
}

ENROLLMENT *enrollment_parse(
			char *input,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_registration )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char course_name[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char transaction_date_time[ 128 ];
	ENROLLMENT *enrollment;

	if ( !input || !*input ) return (ENROLLMENT *)0;

	/* See: attribute_list enrollment */
	/* ------------------------------ */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( course_name, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );
	piece( year, SQL_DELIMITER, input, 4 );

	enrollment =
		enrollment_new(
			entity_new(
				strdup( full_name ),
				strdup( street_address ) ),
			strdup( course_name ),
			strdup( season_name ),
			atoi( year ),
			(REGISTRATION *)0,
			(OFFERING *)0 );

	piece( transaction_date_time, SQL_DELIMITER, input, 5 );
	enrollment->transaction_date_time = transaction_date_time;

	if ( fetch_offering )
	{
		enrollment->offering =
			offering_fetch(
				enrollment->offering->course->course_name,
				enrollment->offering->semester->season_name,
				enrollment->offering->semester->year,
				fetch_course,
				fetch_program,
				0 /* not fetch_enrollment_list */ );
	}

	if ( fetch_registration )
	{
		enrollment->registration =
			registration_fetch(
				full_name,
				street_address,
				enrollment->offering->semester->season_name,
				enrollment->offering->semester->year,
				1 /* fetch_enrollment_list */,
				1 /* fetch_offering */,
				0 /* not fetch_course */,
				0 /* not fetch_program */,
				0 /* not fetch_tuition_payment_list */,
				0 /* notfetch_tuition_refund_list */ );
	}
	return enrollment;
}

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *street_address,
			char *season_name,
			char *course_name,
			int year,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_registration )
{
	ENROLLMENT *enrollment;

	enrollment =
		enrollment_parse(
			pipe2string(
				enrollment_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					enrollment_primary_where(
						student_full_name,
						street_address,
						season_name,
						course_name,
						year ) ) ),
			fetch_offering,
			fetch_course,
			fetch_program,
			fetch_registration );

	return enrollment;
}

LIST *enrollment_system_list(
			char *sys_string,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_registration )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *enrollment_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			enrollment_list,
			enrollment_parse(
				input,
				fetch_offering,
				fetch_course,
				fetch_program,
				fetch_registration ) );
	}
	pclose( input_pipe );
	return enrollment_list;
}

FILE *enrollment_update_open( void )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"update_statement table=%s key=\"%s\" carrot=y	|"
		"tee_appaserver_error.sh			|"
		"sql						 ",
		ENROLLMENT_TABLE,
		ENROLLMENT_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void enrollment_update(
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	FILE *update_pipe;

	update_pipe = enrollment_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^transaction_date_time^%s\n",
		 student_full_name,
		 student_street_address,
		 course_name,
		 season_name,
		 year,
		 (transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

char *enrollment_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	char static where[ 512 ];

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
		 course_name_escape( course_name ),
		 season_name,
		 year );

	return where;
}

TRANSACTION *enrollment_transaction(
			int *seconds_to_add,
			char *student_full_name,
			char *street_address,
			char *transaction_date_time,
			char *program_name,
			double offering_course_price,
			char *account_receivable,
			char *offering_revenue_account )
{
	TRANSACTION *transaction;

	if ( dollar_virtually_same( offering_course_price, 0.0 ) )
		return (TRANSACTION *)0;

	if ( !transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty transaction_date_time\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	transaction =
		transaction_full(
			student_full_name,
			street_address,
			transaction_date_time,
			offering_course_price
				/* transaction_amount */,
			enrollment_memo( program_name ),
			1 /* lock_transaction */,
			(*seconds_to_add)++ );

	transaction->program_name = program_name;

	transaction->journal_list =
		journal_binary_list(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			account_receivable,
			offering_revenue_account );

	return transaction;
}

LIST *enrollment_tuition_payment_list(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	return
		tuition_payment_system_list(
			tuition_payment_sys_string(
				enrollment_primary_where(
					student_full_name,
					street_address,
					course_name,
					season_name,
					year ) ),
			0 /* not fetch_registration */,
			1 /* fetch_enrollment_list */,
			1 /* fetch_offering */,
			1 /* fetch_course */,
			0 /* not fetch_program */ );
}

LIST *enrollment_tuition_refund_list(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	return
		tuition_refund_system_list(
			tuition_refund_sys_string(
				enrollment_primary_where(
					student_full_name,
					street_address,
					course_name,
					season_name,
					year ) ),
			0 /* not fetch_registration */,
			0 /* not fetch_enrollment_list */,
			0 /* not fetch_offering */,
			0 /* not fetch_course */,
			0 /* not fetch_program */ );
}

FILE *enrollment_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"grep -vi duplicate					 |"
		"cat >%s						  ",
		ENROLLMENT_TABLE,
		ENROLLMENT_INSERT_COLUMNS,
		'n',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void enrollment_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *transaction_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%d^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( student_full_name ),
		street_address,
		course_name,
		season_name,
		year,
		(transaction_date_time)
			? transaction_date_time
			: "" );
}

LIST *enrollment_course_name_list(
			LIST *enrollment_list )
{
	LIST *course_name_list;
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return (LIST *)0;

	course_name_list = list_new();

	do {
		enrollment =
			list_get(
				enrollment_list );

		if ( enrollment->offering
		&&   enrollment->offering->course )
		{
			list_set(
				course_name_list,
				enrollment->offering->course->course_name );
		}

	} while ( list_next( enrollment_list ) );

	return course_name_list;
}

char *enrollment_memo( char *program_name )
{
	static char payment_memo[ 128 ];

	if ( program_name && *program_name )
	{
		sprintf(payment_memo,
			"%s/%s",
			ENROLLMENT_MEMO,
			program_name );
	}
	else
	{
		strcpy( payment_memo, ENROLLMENT_MEMO );
	}
	return payment_memo;
}

void enrollment_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;
	char *receivable;

	if ( !list_rewind( enrollment_list ) ) return;

	receivable = account_receivable( (char *)0 );

	do {
		enrollment = list_get( enrollment_list );

		enrollment_set_transaction(
			transaction_seconds_to_add,
			enrollment,
			receivable,
			enrollment->offering->revenue_account,
			enrollment->offering->course->program->program_name,
			enrollment->registration->registration_date_time );

	} while ( list_next( enrollment_list ) );
}

boolean enrollment_set_transaction(
			int *transaction_seconds_to_add,
			ENROLLMENT *enrollment,
			char *account_receivable,
			char *revenue_account,
			char *program_name,
			char *registration_date_time )
{
	if ( !enrollment
	||   !enrollment->registration
	||   !enrollment->offering )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: empty enrollment, registration or offering.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !enrollment->transaction_date_time
	||   !*enrollment->transaction_date_time )
	{
		enrollment->transaction_date_time =
			transaction_race_free(
				enrollment->
					registration->
					registration_date_time );
	}

	if ( ( enrollment->enrollment_transaction =
		enrollment_transaction(
			transaction_seconds_to_add,
			enrollment->
				registration->
				student_entity->
				full_name,
			enrollment->
				registration->
				student_entity->
				street_address,
			enrollment->transaction_date_time,
			program_name,
			enrollment->offering->course_price,
			account_receivable,
			revenue_account ) ) )
	{
		enrollment->transaction_date_time =
			enrollment->enrollment_transaction->
				transaction_date_time;

		(*transaction_seconds_to_add)++;
		return 1;
	}
	else
	{
		enrollment->transaction_date_time = (char *)0;
		return 0;
	}
}

void enrollment_fetch_update(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"enrollment_tuition_payment_total.sh \"%s\" '%s' \"%s\" '%s' %d",
		student_full_name,
		student_street_address,
		course_name,
		season_name,
		year );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
	"enrollment_tuition_refund_total.sh \"%s\" '%s' \"%s\" '%s' %d",
		student_full_name,
		student_street_address,
		course_name,
		season_name,
		year );

	if ( system( sys_string ) ){}
}

void enrollment_list_update(
			LIST *enrollment_list,
			char *season_name,
			int year )
{
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return;

	do {
		enrollment = list_get( enrollment_list );

		enrollment_update(
			enrollment->transaction_date_time,
			enrollment->
				registration->
				student_entity->
				full_name,
			enrollment->
				registration->
				student_entity->
				street_address,
			enrollment->offering->course->course_name,
			season_name,
			year );

	} while ( list_next( enrollment_list ) );
}

void enrollment_list_fetch_update(
			LIST *enrollment_list,
			char *season_name,
			int year )
{
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return;

	do {
		enrollment = list_get( enrollment_list );

		enrollment_fetch_update(
			enrollment->
				registration->
				student_entity->
				full_name,
			enrollment->
				registration->
				student_entity->
				street_address,
			enrollment->offering->course->course_name,
			season_name,
			year );

	} while ( list_next( enrollment_list ) );
}

LIST *enrollment_registration_list(
			LIST *enrollment_list )
{
	LIST *registration_list;
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return (LIST *)0;

	registration_list = list_new();

	do {
		enrollment =
			list_get(
				enrollment_list );

		if ( !enrollment->registration )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty registration.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			registration_list,
			enrollment->registration );

	} while ( list_next( enrollment_list ) );

	return registration_list;
}

char *enrollment_offering_program_name(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return (char *)0;

	do {
		enrollment =
			list_get(
				enrollment_list );

		if ( !enrollment->offering
		||   !enrollment->offering->course )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: empty offering or course.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return enrollment->offering->course->program_name;

	} while ( list_next( enrollment_list ) );

	return (char *)0;
}

OFFERING *enrollment_offering_seek(
			char *course_name,
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return (OFFERING *)0;

	do {
		enrollment =
			list_get(
				enrollment_list );

		if ( !enrollment->offering
		||   !enrollment->offering->course )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: empty offering or course.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(
			enrollment->offering->course->course_name,
			course_name ) == 0 )
		{
			return enrollment->offering;
		}

	} while ( list_next( enrollment_list ) );

	return (OFFERING *)0;
}

char *enrollment_list_program_name(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment = {0};

	enrollment = list_first( enrollment_list );

	if ( enrollment )
	{
		if ( !enrollment->offering
		||   !enrollment->offering->course )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: empty offering or course.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return enrollment->offering->course->program_name;
	}
	else
	{
		return (char *)0;
	}
}

char *enrollment_list_revenue_account(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment = {0};

	enrollment = list_first( enrollment_list );

	if ( enrollment )
	{
		if ( !enrollment->offering )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty offering.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return enrollment->offering->revenue_account;
	}
	else
	{
		return (char *)0;
	}
}

