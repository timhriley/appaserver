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
#include "receivable.h"
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

	course_drop->student_entity = student_entity;
	course_drop->course_name = course_name;
	course_drop->semester = semester;

	return course_drop;
}

COURSE_DROP *course_drop_parse(
			char *input,
			boolean fetch_enrollment,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_registration )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char course_name[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char course_drop_date_time[ 128 ];
	char refund_due_yn[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
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
			strdup( course_name ),
			semester_new(
				strdup( season_name ),
				atoi( year ) ) );

	piece( course_drop_date_time, SQL_DELIMITER, input, 5 );
	course_drop->course_drop_date_time = strdup( course_drop_date_time );

	piece( refund_due_yn, SQL_DELIMITER, input, 6 );
	course_drop->refund_due = ( *refund_due_yn == 'y' );

	piece( payor_full_name, SQL_DELIMITER, input, 7 );

	if ( *payor_full_name )
	{
		piece( payor_street_address, SQL_DELIMITER, input, 8 );

		course_drop->payor_entity =
			entity_new(
				strdup( payor_full_name ),
				strdup( payor_street_address ) );
	}

	piece( transaction_date_time, SQL_DELIMITER, input, 9 );
	course_drop->transaction_date_time = strdup( transaction_date_time );

	if ( fetch_enrollment )
	{
		course_drop->enrollment =
			enrollment_fetch(
				course_drop->student_entity->full_name,
				course_drop->student_entity->street_address,
				course_drop->course_name,
				course_drop->semester->season_name,
				course_drop->semester->year,
				fetch_offering,
				fetch_course,
				0 /* not fetch_program */,
				fetch_registration );
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
			boolean fetch_registration )
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
			fetch_registration );

	return course_drop;
}

LIST *course_drop_system_list(
			char *sys_string,
			boolean fetch_enrollment,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_registration )
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
				fetch_registration ) );
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
			char *course_drop_date_time,
			ENTITY *payor_entity,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	FILE *update_pipe;
	char *payor_full_name = "";
	char *payor_street_address = "";

	update_pipe = course_drop_update_open();

	if ( transaction_date_time
	&&   *transaction_date_time
	&&   payor_entity )
	{
		payor_full_name = payor_entity->full_name;
		payor_street_address = payor_entity->street_address;
	}

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^course_drop_date_time^%s\n",
		 student_full_name,
		 student_street_address,
		 course_name,
		 season_name,
		 year,
		 course_drop_date_time );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^payor_full_name^%s\n",
		 student_full_name,
		 student_street_address,
		 course_name,
		 season_name,
		 year,
		 payor_full_name );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^payor_street_address^%s\n",
		 student_full_name,
		 student_street_address,
		 course_name,
		 season_name,
		 year,
		 payor_street_address );

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
		"student_full_name = '%s' and		"
		"student_street_address = '%s' and	"
		"course_name = '%s' and			"
		"season_name = '%s' and			"
		"year = %d				",
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
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *program_name,
			char *course_name,
			double offering_course_price,
			double receivable_expecting,
			char *account_receivable,
			char *account_payable,
			char *offering_revenue_account )
{
	TRANSACTION *transaction;
	double receivable_credit_amount = {0};
	double payable_credit_amount = {0};
	JOURNAL *journal;

	if ( !transaction_date_time || !*transaction_date_time )
	{
		return (TRANSACTION *)0;
	}

	if ( dollar_virtually_same( offering_course_price, 0.0 ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty offering_course_price\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (TRANSACTION *)0;
	}

	if ( !offering_revenue_account )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: empty offering_revenue_account\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (TRANSACTION *)0;
	}

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			transaction_date_time,
			offering_course_price
				/* transaction_amount */,
			course_drop_memo( course_name ),
			1 /* lock_transaction */,
			(*seconds_to_add)++ );

	transaction->program_name = program_name;
	transaction->transaction_amount = offering_course_price;

	if ( !transaction->journal_list )
		transaction->journal_list =
			list_new();

	if ( receivable_expecting == 0.0 )
	{
		payable_credit_amount = offering_course_price;
		receivable_credit_amount = 0.0;
	}
	else
	if ( receivable_expecting > 0.0 )
	{
		if ( receivable_expecting >= offering_course_price )
		{
			payable_credit_amount = 0.0;
			receivable_credit_amount = offering_course_price;
		}
		else
		if ( receivable_expecting < offering_course_price )
		{
			payable_credit_amount =
				offering_course_price -
				receivable_expecting;

			receivable_credit_amount = receivable_expecting;
		}
	}

	/* Debit */
	/* ----- */
	journal =
		journal_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			offering_revenue_account );

	journal->debit_amount = offering_course_price;

	list_set( transaction->journal_list, journal );

	/* Credits */
	/* ------- */
	if ( receivable_credit_amount )
	{
		journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_receivable );

		journal->credit_amount = receivable_credit_amount;

		list_set( transaction->journal_list, journal );
	}

	if ( payable_credit_amount )
	{
		journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_payable );

		journal->credit_amount = payable_credit_amount;

		list_set( transaction->journal_list, journal );
	}

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
			char *course_drop_date_time,
			boolean refund_due,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%d^%s^%c^%s^%s^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( student_full_name ),
		street_address,
		course_name,
		season_name,
		year,
		course_drop_date_time,
		(refund_due) ? 'y' : 'n',
		payor_full_name,
		payor_street_address,
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

char *course_drop_memo( char *course_name )
{
	static char memo[ 128 ];

	sprintf(memo,
		"%s/%s",
		COURSE_DROP_MEMO,
		course_name );

	return memo;
}

void course_drop_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *course_drop_list )
{
	COURSE_DROP *course_drop;
	char *receivable;
	char *payable;

	if ( !list_rewind( course_drop_list ) ) return;

	receivable = account_receivable( (char *)0 );
	payable = account_payable( (char *)0 );

	do {
		course_drop = list_get( course_drop_list );

		if ( !course_drop->enrollment
		||   !course_drop->enrollment->offering
		||   !course_drop->enrollment->offering->course )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: empty enrollment, offering or course.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( course_drop->payor_entity )
		{
			if ( ( course_drop->course_drop_transaction =
				course_drop_transaction(
					transaction_seconds_to_add,
					course_drop->
						payor_entity->
						full_name,
					course_drop->
						payor_entity->
						street_address,
					course_drop->transaction_date_time,
					course_drop->
						enrollment->
						offering->
						course->
						program_name,
					course_drop->
						enrollment->
						offering->
						course_name,
					course_drop->
						enrollment->
						offering->
						course_price,
					course_drop->
					       course_drop_receivable_expecting,
					receivable,
					payable,
					course_drop->
						enrollment->
						offering->
						revenue_account ) ) )
			{
				course_drop->transaction_date_time =
					course_drop->course_drop_transaction->
						transaction_date_time;
			}
		}

	} while ( list_next( course_drop_list ) );
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

		ptr += sprintf(	ptr,
				"%s will drop %s; ",
				entity_name_display(
					course_drop->
						student_entity->
						full_name,
					course_drop->
						student_entity->
						street_address ),
				course_drop->course_name );

	} while ( list_next( course_drop_list ) );

	return strdup( display );
}

COURSE_DROP *course_drop_steady_state(
			COURSE_DROP *course_drop,
			char *course_drop_date_time,
			ENTITY *payor_entity )
{
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

	if ( !course_drop_date_time || !*course_drop_date_time )
	{
		course_drop->course_drop_date_time =
			date_now19( date_utc_offset() );
	}

	if ( !payor_entity )
	{
		course_drop->payor_entity =
			course_drop->
				enrollment->
				registration->
				payor_entity;
	}

	course_drop->course_drop_receivable_expecting =
		course_drop_receivable_expecting(
			course_drop->payor_entity->full_name,
			course_drop->payor_entity->street_address );

	return course_drop;
}

LIST *course_drop_list_enrollment_list(
			LIST *course_drop_list )
{
	LIST *enrollment_list;
	COURSE_DROP *course_drop;

	if ( !list_rewind( course_drop_list ) ) return (LIST *)0;

	enrollment_list = list_new();

	do {
		course_drop =
			list_get(
				course_drop_list );

		if ( !course_drop->enrollment )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty enrollment.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			enrollment_list,
			course_drop->enrollment );

	} while ( list_next( course_drop_list ) );

	return enrollment_list;
}

double course_drop_receivable_expecting(
			char *payor_full_name,
			char *payor_street_address )
{
	RECEIVABLE *receivable;

	receivable =
		receivable_new(
			payor_full_name,
			payor_street_address );

	receivable =
		/* ------------------------- */
		/* Sets receivable_expecting */
		/* ------------------------- */
		receivable_steady_state(
			receivable );

	if ( !receivable )
		return 0.0;
	else
		return receivable->receivable_expecting;
}

