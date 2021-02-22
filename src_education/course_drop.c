/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/course_drop.c		*/
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
#include "offering.h"
#include "course.h"
#include "enrollment.h"
#include "course_drop.h"

char *course_drop_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 "course_drop",
		 where );

	return strdup( sys_string );
}

COURSE_DROP *course_drop_new(
			ENTITY *student_entity,
			char *course_name,
			SEMESTER *semester )
{
	COURSE_DROP *course_drop;

	if ( ! ( course_drop = calloc( 1, sizeof( COURSE_DROP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	course_drop->course_name = course_name;
	course_drop->semester = semester;

	return course_drop;
}

COURSE_DROP *course_drop_parse(
			char *input,
			boolean fetch_enrollment,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_registration,
			boolean fetch_transaction )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char course_name[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char refund_due_yn[ 128 ];
	char transaction_date_time[ 128 ];
	COURSE_DROP *course_drop;

	if ( !input || !*input ) return (COURSE_DROP *)0;

	/* See: attribute_list course_drop */
	/* ------------------------------ */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( course_name, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );
	piece( year, SQL_DELIMITER, input, 4 );

	course_drop =
		course_drop_new(
			entity_new(
				strdup( full_name ),
				strdup( street_address ) ),
			semester_new(
				strdup( course_name ),
				strdup( season_name ) ),
			atoi( year ) );

	piece( refund_due_yn, SQL_DELIMITER, input, 5 );
	course_drop->refund_due = ( *refund_due_yn == 'y' );

	piece( transaction_date_time, SQL_DELIMITER, input, 6 );
	course_drop->transaction_date_time = transaction_date_time;

	if ( fetch_enrollment )
	{
		course_drop->enrollment =
			enrollment_fetch(
				course_drop->student_entity->full_name,
				course_drop->student_entity->street_address,
				course_drop->semester->season_name,
				course_drop->semester->year,
				fetch_offering,
				fetch_course,
				0 /* not fetch_program */,
				fetch_registration,
				0 /* not fetch_transaction */ );
	}

	if ( fetch_transaction && *course_drop->transaction_date_time )
	{
		course_drop->course_drop_transaction =
			transaction_fetch(
				full_name,
				street_address,
				course_drop->transaction_date_time );
	}

	return course_drop;
}

COURSE_DROP *course_drop_fetch(
			char *student_full_name,
			char *street_address,
			char *season_name,
			char *course_name,
			int year,
			boolean fetch_enrollment,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_registration,
			boolean fetch_transaction )
{
	COURSE_DROP *course_drop;

	course_drop =
		course_drop_parse(
			pipe2string(
				course_drop_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					course_drop_primary_where(
						student_full_name,
						street_address,
						season_name,
						course_name,
						year ) ) ),
			fetch_enrollment,
			fetch_offering,
			fetch_course,
			fetch_registration,
			fetch_transaction );

	return course_drop;
}

LIST *course_drop_system_list(
			char *sys_string,
			boolean fetch_enrollment,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_registration,
			boolean fetch_transaction )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *course_drop_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			course_drop_list,
			course_drop_parse(
				input,
				fetch_enrollment,
				fetch_offering,
				fetch_course,
				fetch_registration,
				fetch_transaction ) );
	}
	pclose( input_pipe );
	return course_drop_list;
}

FILE *course_drop_update_open( void )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"update_statement table=%s key=\"%s\" carrot=y	|"
		"tee_appaserver_error.sh			|"
		"sql						 ",
		COURSE_DROP_TABLE,
		COURSE_DROP_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void course_drop_update(
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	FILE *update_pipe;

	update_pipe = course_drop_update_open();

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

char *course_drop_primary_where(
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
		 entity_escape_full_name( student_full_name ),
		 street_address,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 course_name_escape( course_name ),
		 season_name,
		 year );

	return where;
}

TRANSACTION *course_drop_transaction(
			int *seconds_to_add,
			char *student_full_name,
			char *street_address,
			char *transaction_date_time,
			char *program_name,
			double offering_course_price,
			char *offering_revenue_account,
			char *account_paypable )
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
			course_drop_memo( program_name ),
			1 /* lock_transaction */,
			(*seconds_to_add)++ );

	transaction->program_name = program_name;

	transaction->journal_list =
		journal_binary_list(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			offering_revenue_account,
			account_payable );

	return transaction;
}

FILE *course_drop_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"grep -vi duplicate					 |"
		"cat >%s						  ",
		COURSE_DROP_TABLE,
		COURSE_DROP_INSERT_COLUMNS,
		'n',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void course_drop_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			boolean refund_due,
			char *transaction_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%d^%c^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( student_full_name ),
		street_address,
		course_name,
		season_name,
		year,
		(refund_due) ? 'y' : 'n',
		(transaction_date_time)
			? transaction_date_time
			: "" );
}

LIST *course_drop_course_name_list(
			LIST *course_drop_list )
{
	LIST *course_name_list;
	COURSE_DROP *course_drop;

	if ( !list_rewind( course_drop_list ) ) return (LIST *)0;

	course_name_list = list_new();

	do {
		course_drop =
			list_get(
				course_drop_list );

		if ( !course_drop->enrollment
		||   !course_drop->enrollment->offering
		||   !course_drop->enrollment->offering->course )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: empty enrollment, offering, or course.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			course_name_list,
			course_drop->
				enrollment->
				offering->
				course->
				course_name );

	} while ( list_next( course_drop_list ) );

	return course_name_list;
}

char *course_drop_memo( char *program_name )
{
	static char payment_memo[ 128 ];

	if ( program_name && *program_name )
	{
		sprintf(payment_memo,
			"%s/%s",
			COURSE_DROP_MEMO,
			program_name );
	}
	else
	{
		strcpy( payment_memo, COURSE_DROP_MEMO );
	}
	return payment_memo;
}

void course_drop_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *course_drop_list )
{
	COURSE_DROP *course_drop;
	char *paypable;

	if ( !list_rewind( course_drop_list ) ) return;

	payable = account_payable( (char *)0 );

	do {
		course_drop = list_get( course_drop_list );

		if ( !course_drop->offering )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty offering.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !course_drop->registration )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty registration.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		course_drop_set_transaction(
			transaction_seconds_to_add,
			course_drop,
			course_drop->offering->revenue_account,
			payable,
			course_drop->offering->course->program_name,
			course_drop->registration->registration_date_time );

	} while ( list_next( course_drop_list ) );
}

boolean course_drop_set_transaction(
			int *transaction_seconds_to_add,
			COURSE_DROP *course_drop,
			char *account_receivable,
			char *revenue_account,
			char *program_name,
			char *registration_date_time )
{
	if ( !course_drop
	||   !course_drop->registration
	||   !course_drop->offering )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: empty course_drop, registration or offering.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !course_drop->transaction_date_time
	||   !*course_drop->transaction_date_time )
	{
		course_drop->transaction_date_time =
			transaction_race_free(
				registration_date_time );
	}

	if ( ( course_drop->course_drop_transaction =
		course_drop_transaction(
			transaction_seconds_to_add,
			course_drop->
				registration->
				student_entity->
				full_name,
			course_drop->
				registration->
				student_entity->
				street_address,
			course_drop->transaction_date_time,
			program_name,
			course_drop->offering->course_price,
			account_receivable,
			revenue_account ) ) )
	{
		course_drop->transaction_date_time =
			course_drop->course_drop_transaction->
				transaction_date_time;
		return 1;
	}
	else
	{
		course_drop->transaction_date_time = (char *)0;
		return 0;
	}
}

void course_drop_fetch_update(
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

void course_drop_list_update(
			LIST *course_drop_list,
			char *season_name,
			int year )
{
	COURSE_DROP *course_drop;

	if ( !list_rewind( course_drop_list ) ) return;

	do {
		course_drop = list_get( course_drop_list );

		course_drop_update(
			course_drop->transaction_date_time,
			course_drop->
				registration->
				student_entity->
				full_name,
			course_drop->
				registration->
				student_entity->
				street_address,
			course_drop->offering->course->course_name,
			season_name,
			year );

	} while ( list_next( course_drop_list ) );
}

void course_drop_list_fetch_update(
			LIST *course_drop_list,
			char *season_name,
			int year )
{
	COURSE_DROP *course_drop;

	if ( !list_rewind( course_drop_list ) ) return;

	do {
		course_drop = list_get( course_drop_list );

		if ( !course_drop->enrollment
		||   !course_drop->enrollment->registration )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: empty enrollment or registration.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !course_drop->enrollment->offering )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty offering.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		enrollment_fetch_update(
			course_drop->
				enrollment->
				registration->
				student_entity->
				full_name,
			course_drop->
				enrollment->
				registration->
				student_entity->
				street_address,
			course_drop->enrollment->offering->course->course_name,
			season_name,
			year );

	} while ( list_next( course_drop_list ) );
}

char *course_drop_list_first_program_name(
			LIST *course_drop_list )
{
	COURSE_DROP *course_drop = {0};

	course_drop = list_first( course_drop_list );

	if ( course_drop )
	{
		if ( !course_drop->offering
		||   !course_drop->offering->course )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: empty offering or course.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return course_drop->offering->course->program_name;
	}
	else
	{
		return (char *)0;
	}
}

char *course_drop_list_revenue_account(
			LIST *course_drop_list )
{
	COURSE_DROP *course_drop = {0};

	course_drop = list_first( course_drop_list );

	if ( course_drop )
	{
		if ( !course_drop->offering )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty offering.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return course_drop->offering->revenue_account;
	}
	else
	{
		return (char *)0;
	}
}

char *course_drop_list_display( LIST *course_drop_list )
{
	char display[ 65536 ];
	char *ptr = display;
	COURSE_DROP *course_drop;

	*ptr = '\0';

	if ( !list_rewind( course_drop_list ) )
	{
		return "";
	}

	ptr += sprintf( ptr, "Course drop: " );

	do {
		course_drop =
			list_get(
				course_drop_list );

		if ( !list_at_head( course_drop_list ) )
		{
			ptr += sprintf( ptr, ", " );
		}

		if ( !course_drop->enrollment
		||   !course_drop->offering )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: empty registration or offering.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		ptr += sprintf(	ptr,
				"%s will drop %s; ",
				entity_name_display(
					course_drop->
						student_entity->
						full_name,
					course_drop->
						student_entity->
						street_address ),
				course_drop->
					enrollment->
					offering->
					course_name );

	} while ( list_next( course_drop_list ) );

	return strdup( display );
}
