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
#include "offering.h"
#include "course.h"
#include "course_drop.h"
#include "registration.h"
#include "tuition_payment.h"
#include "tuition_refund.h"
#include "liability.h"
#include "enrollment.h"

char *enrollment_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "select.sh '*' %s \"%s\" select",
		 "enrollment",
		 where );

	return strdup( system_string );
}

ENROLLMENT *enrollment_new(
			ENTITY *student_entity,
			char *course_name,
			SEMESTER *semester )
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

	enrollment->student_entity = student_entity;
	enrollment->course_name = course_name;
	enrollment->semester = semester;

	return enrollment;
}

ENROLLMENT *enrollment_parse(
			char *input,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_registration )
{
	char student_full_name[ 128 ];
	char student_street_address[ 128 ];
	char course_name[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char enrollment_date_time[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char transaction_date_time[ 128 ];
	ENROLLMENT *enrollment;

	if ( !input || !*input ) return (ENROLLMENT *)0;

	/* See: attribute_list enrollment */
	/* ------------------------------ */
	piece( student_full_name, SQL_DELIMITER, input, 0 );
	piece( student_street_address, SQL_DELIMITER, input, 1 );
	piece( course_name, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );
	piece( year, SQL_DELIMITER, input, 4 );

	enrollment =
		enrollment_new(
			entity_new(
				strdup( student_full_name ),
				strdup( student_street_address ) ),
			strdup( course_name ),
			semester_new(
				strdup( season_name ),
				atoi( year ) ) );

	piece( enrollment_date_time, SQL_DELIMITER, input, 5 );
	enrollment->enrollment_date_time = strdup( enrollment_date_time );

	piece( payor_full_name, SQL_DELIMITER, input, 6 );

	if ( *payor_full_name )
	{
		piece( payor_street_address, SQL_DELIMITER, input, 7 );

		enrollment->payor_entity =
			entity_new(
				strdup( payor_full_name ),
				strdup( payor_street_address ) );
	}

	piece( transaction_date_time, SQL_DELIMITER, input, 8 );
	enrollment->transaction_date_time = strdup( transaction_date_time );

	if ( fetch_offering )
	{
		enrollment->offering =
			offering_fetch(
				enrollment->course_name,
				enrollment->semester->season_name,
				enrollment->semester->year,
				fetch_course,
				fetch_program );
	}

	if ( fetch_registration )
	{
		enrollment->registration =
			registration_fetch(
				student_full_name,
				student_street_address,
				enrollment->semester->season_name,
				enrollment->semester->year );
	}

	return enrollment;
}

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *student_street_address,
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
			string_pipe_fetch(
				enrollment_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					enrollment_primary_where(
						student_full_name,
						student_street_address,
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
			char *enrollment_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	FILE *update_pipe;

	if ( !enrollment_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty enrollment_date_time.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_pipe = enrollment_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^enrollment_date_time^%s\n",
		 student_full_name,
		 student_street_address,
		 course_name,
		 season_name,
		 year,
		 enrollment_date_time );

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

char *enrollment_primary_where(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	char static where[ 1024 ];

	sprintf(where,
		"student_full_name = '%s' and		"
		"student_street_address = '%s' and	"
		"course_name = '%s' and			"
		"season_name = '%s' and			"
		"year = %d				",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 registration_escape_full_name( student_full_name ),
		 student_street_address,
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
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *program_name,
			char *course_name,
			double offering_course_price,
			double liability_prepaid,
			char *account_receivable,
			char *account_payable,
			char *offering_revenue_account )
{
	TRANSACTION *transaction;
	double receivable_debit_amount = {0};
	double payable_debit_amount = {0};
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
			enrollment_memo( course_name ),
			1 /* lock_transaction */,
			(*seconds_to_add)++ );

	transaction->program_name = program_name;
	transaction->transaction_amount = offering_course_price;

	if ( !transaction->journal_list )
		transaction->journal_list =
			list_new();

	if ( liability_prepaid == 0.0 )
	{
		receivable_debit_amount = offering_course_price;
		payable_debit_amount = 0.0;
	}
	else
	if (liability_prepaid > 0.0 )
	{
		if ( liability_prepaid >= offering_course_price )
		{
			receivable_debit_amount = 0.0;
			payable_debit_amount = offering_course_price;
		}
		else
		if ( liability_prepaid < offering_course_price )
		{
			receivable_debit_amount =
				offering_course_price -
				liability_prepaid;
			payable_debit_amount = liability_prepaid;
		}
	}

	if ( receivable_debit_amount )
	{
		journal =
			journal_new(
				payor_full_name,
				payor_street_address,
				transaction_date_time,
				account_receivable );

		journal->debit_amount = receivable_debit_amount;
		journal->transaction_date_time = transaction_date_time;

		list_set( transaction->journal_list, journal );
	}

	if ( payable_debit_amount )
	{
		journal =
			journal_new(
				payor_full_name,
				payor_street_address,
				transaction_date_time,
				account_payable );

		journal->debit_amount = payable_debit_amount;
		journal->transaction_date_time = transaction_date_time;

		list_set( transaction->journal_list, journal );
	}

	journal =
		journal_new(
			payor_full_name,
			payor_street_address,
			transaction_date_time,
			offering_revenue_account );

	journal->credit_amount = offering_course_price;
	journal->transaction_date_time = transaction_date_time;

	list_set( transaction->journal_list, journal );

	return transaction;
}

void enrollment_list_insert(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( enrollment_list ) ) return;

	insert_pipe =
		enrollment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		enrollment = list_get( enrollment_list );

		enrollment_insert_pipe(
			insert_pipe,
			enrollment->student_entity->full_name,
			enrollment->student_entity->street_address,
			enrollment->course_name,
			enrollment->semester->season_name,
			enrollment->semester->year,
			enrollment->enrollment_date_time,
			enrollment->payor_entity->full_name,
			enrollment->payor_entity->street_address,
			enrollment->transaction_date_time );

	} while ( list_next( enrollment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						   |"
			"queue_top_bottom_lines.e 300			   |"
			"html_table.e 'Insert Enrollment Errors' '' '^'",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
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
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *enrollment_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time )
{
	if ( !enrollment_date_time || !*enrollment_date_time )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: empty enrollment_date_time\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	fprintf(insert_pipe,
		"%s^%s^%s^%s^%d^%s^%s^%s^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		registration_escape_full_name( student_full_name ),
		student_street_address,
		course_name,
		season_name,
		year,
		enrollment_date_time,
		registration_escape_full_name( payor_full_name ),
		payor_street_address,
		(transaction_date_time)
			? transaction_date_time
			: "" );
}

LIST *enrollment_list_offering_list(
			LIST *enrollment_list )
{
	LIST *offering_list;
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return (LIST *)0;

	offering_list = list_new();

	do {
		enrollment =
			list_get(
				enrollment_list );

		if ( !enrollment->offering )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty offering.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			offering_list,
			enrollment->offering );

	} while ( list_next( enrollment_list ) );

	return offering_list;
}

char *enrollment_memo( char *course_name )
{
	static char memo[ 128 ];

	sprintf(memo,
		"%s/%s",
		ENROLLMENT_MEMO,
		course_name );

	return memo;
}

void enrollment_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;
	char *receivable;
	char *payable;

	if ( !list_rewind( enrollment_list ) ) return;

	if ( ! ( receivable = account_receivable( (char *)0 ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: account_receivable() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( payable = account_payable( (char *)0 ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: account_payable() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		enrollment = list_get( enrollment_list );

		if ( !enrollment->payor_entity )
		{
			fprintf(stderr,
				"Warning in %s/%s()/%d: empty payor_entity.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			continue;
		}

		if ( !enrollment->enrollment_date_time
		||   !*enrollment->enrollment_date_time )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: empty enrollment_date_time\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			continue;
		}

		if ( !enrollment
		||   !enrollment->offering
		||   !enrollment->offering->course )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: empty enrollment, offering or course.\n",
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
					enrollment->enrollment_date_time );
		}

		enrollment->enrollment_transaction =
			enrollment_transaction(
				transaction_seconds_to_add,
				enrollment->payor_entity->full_name,
				enrollment->payor_entity->street_address,
				enrollment->transaction_date_time,
				enrollment->offering->course->program_name,
				enrollment->offering->course_name,
				enrollment->offering->course_price,
				enrollment->liability_prepaid,
				receivable,
				payable,
				enrollment->offering->revenue_account );

		if ( enrollment->enrollment_transaction )
		{
			enrollment->transaction_date_time =
				enrollment->enrollment_transaction->
					transaction_date_time;
		}
		else
		{
			enrollment->transaction_date_time = (char *)0;
		}

	} while ( list_next( enrollment_list ) );
}

void enrollment_list_fetch_update(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return;

	do {
		enrollment = list_get( enrollment_list );

		if ( !enrollment->offering )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty offering.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		registration_fetch_update(
			enrollment->student_entity->full_name,
			enrollment->student_entity->street_address,
			enrollment->offering->semester->season_name,
			enrollment->offering->semester->year );

	} while ( list_next( enrollment_list ) );
}

LIST *enrollment_list_registration_list(
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

char *enrollment_list_first_program_name(
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

char *enrollment_list_display( LIST *enrollment_list )
{
	char display[ 65536 ];
	char *ptr = display;
	ENROLLMENT *enrollment;

	*ptr = '\0';

	if ( !list_rewind( enrollment_list ) )
	{
		return "";
	}

	ptr += sprintf( ptr, "Enrollment: " );

	do {
		enrollment =
			list_get(
				enrollment_list );

		if ( !list_at_head( enrollment_list ) )
		{
			ptr += sprintf( ptr, ", " );
		}

		if ( !enrollment->registration
		||   !enrollment->offering )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: empty registration or offering.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		ptr += sprintf(	ptr,
				"%s will enroll in %s; ",
				entity_name_display(
					enrollment->
						student_entity->
						full_name,
					enrollment->
						student_entity->
						street_address ),
				enrollment->offering->course_name );

	} while ( list_next( enrollment_list ) );

	return strdup( display );
}

ENROLLMENT *enrollment_integrity_fetch(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	return
		enrollment_parse(
			string_pipe_fetch(
				enrollment_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					enrollment_primary_where(
						student_full_name,
						student_street_address,
						course_name,
						season_name,
						year ) ) ),
			0 /* not fetch_offering */,
			0 /* not fetch_course */,
			0 /* not fetch_program */,
			0 /* not fetch_registration */ );
}

ENROLLMENT *enrollment_list_seek(
			char *full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return (ENROLLMENT *)0;

	do {
		enrollment = list_get( enrollment_list );

		if ( strcmp(	enrollment->student_entity->full_name,
				full_name ) == 0
		&&   strcmp(	enrollment->student_entity->street_address,
				street_address ) == 0
		&&   strcmp(	enrollment->offering->course_name,
				course_name ) == 0
		&&   strcmp(	enrollment->semester->season_name,
				season_name ) == 0
		&&   enrollment->semester->year == year )
		{
			return enrollment;
		}
	} while ( list_next( enrollment_list ) );

	return (ENROLLMENT *)0;
}

LIST *enrollment_list_course_drop_list(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;
	LIST *course_drop_list = {0};

	if ( !list_rewind( enrollment_list ) ) return (LIST *)0;

	do {
		enrollment = list_get( enrollment_list );

		if ( list_length( enrollment->course_drop_list ) )
		{
			if ( !course_drop_list )
				course_drop_list =
					list_new();

			list_set_list(
				course_drop_list,
				enrollment->course_drop_list );
		}
	} while ( list_next( enrollment_list ) );

	return course_drop_list;
}

char *enrollment_list_first_course_drop_program_name(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment = {0};
	COURSE_DROP *course_drop;

	enrollment = list_first( enrollment_list );

	if ( enrollment && list_length( enrollment->course_drop_list ) )
	{
		course_drop =
			list_first(
				enrollment->
					course_drop_list );

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

		return course_drop->enrollment->offering->course->program_name;
	}
	else
	{
		return (char *)0;
	}
}

ENROLLMENT *enrollment_steady_state(
			ENROLLMENT *enrollment,
			char *enrollment_date_time,
			ENTITY *payor_entity )
{
	if ( !enrollment->registration )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !enrollment->offering )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty offering.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !enrollment_date_time || !*enrollment_date_time )
	{
		enrollment->enrollment_date_time =
			date_now19( date_utc_offset() );
	}

	if ( !payor_entity )
	{
		enrollment->payor_entity =
			enrollment->registration->payor_entity;
	}

	if ( enrollment->payor_entity )
	{
		enrollment->liability_prepaid =
			entity_liability_prepaid(
				enrollment->payor_entity->full_name,
				enrollment->payor_entity->street_address );
	}

	return enrollment;
}

