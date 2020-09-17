/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/education.c		*/
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
#include "boolean.h"
#include "list.h"
#include "date_convert.h"
#include "transaction.h"
#include "entity.h"
#include "enrollment.h"
#include "offering.h"
#include "semester.h"
#include "payment.h"
#include "deposit.h"
#include "spreadsheet.h"
#include "education.h"

EDUCATION *education_calloc( void )
{
	EDUCATION *education;

	if ( ! ( education = calloc( 1, sizeof( EDUCATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return education;
}

EDUCATION *education_new(
			char *season_name,
			int year,
			char *spreadsheet_name,
			char *spreadsheet_filename )
{
	EDUCATION *education = education_calloc();

	education->semester =
		semester_new(
			season_name,
			year );

	education->paypal =
		paypal_new(
			spreadsheet_name,
			spreadsheet_filename );

	return education;
}

EDUCATION *education_fetch(
			char *season_name,
			int year,
			char *spreadsheet_name,
			char *spreadsheet_filename )
{
	EDUCATION *education = education_calloc();

	education->semester =
		semester_fetch(
			season_name,
			year,
			1 /* fetch_offering_list */,
			1 /* fetch_registration_list */ );

	education->paypal =
		paypal_fetch(
			spreadsheet_name,
			spreadsheet_filename );

	return education;
}

void education_deposit_list_insert(
			LIST *education_deposit_list )
{
if ( education_deposit_list ){}
}

LIST *education_deposit_list(
			char *season_name,
			int year,
			char *spreadsheet_filename,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *semester_registration_list )
{
	LIST *deposit_list = list_new();
	DEPOSIT *deposit;
	char input[ 65536 ];
	FILE *spreadsheet_file;
	/* ------------------------------------------ */
	/* Don't want to loose paypal_dataset pointer */
	/* ------------------------------------------ */
	PAYPAL_DATASET *dataset_return;

	if ( ! ( spreadsheet_file = fopen( spreadsheet_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot open [%s] for read.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
		spreadsheet_filename );

		return (LIST *)0;
	}

	while ( string_input( input, spreadsheet_file, 65536 ) )
	{
		if ( ( dataset_return =
				/* ---------------------- */
				/* Returns paypal_dataset */
				/* ---------------------- */
				education_paypal_dataset(
					input,
					spreadsheet,
					paypal_dataset ) ) )
		{
			if ( ( deposit =
				education_deposit(
					season_name,
					year,
					dataset_return
						/* paypal_dataset */,
					semester_offering_list,
					semester_registration_list ) ) )
		{
			list_set( deposit_list, deposit );
		}
	}
	fclose( spreadsheet_file );
	return deposit_list;
}

DEPOSIT *education_deposit(
			char *season_name,
			int year,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *semester_registration_list )
{
	DEPOSIT *deposit;
	char *payor_street_address;
	char deposit_date_time[ 128 ];

	if ( !paypal_dataset )
	{
		fprintf(stderr,
			"%s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	deposit = deposit_calloc();

	if ( ( payor_street_address =
		entity_street_address(
			paypal_dataset->full_name_D ) ) )
	{
		deposit->payor_entity =
			entity_new(
				paypal_dataset->full_name_D,
				payor_street_address );
	}
	else
	{
		deposit->payor_entity =
			entity_new(
				paypal_dataset->full_name_D,
				EDUCATION_DEFAULT_STREET_ADDRESS );
	}

	sprintf(deposit_date_time,
		"%s %s",
		/* --------------------------------- */
		/* Looks like: 1/2/2020 and 12:28:39 */
		/* --------------------------------- */
		date_convert_international_string(
			paypal_dataset->date_A ),
		paypal_dataset->time_B );
	
	deposit->deposit_date_time = strdup( deposit_date_time );

	deposit->semester =
		semester_new(
			season_name,
			year );

	deposit->deposit_payment_list =
		education_payment_list(
			season_name,
			year,
			input,
			/* ------------ */
			/* Stamp couple */
			/* ------------ */
			paypal_dataset,
			/* -------- */
			/* Set only */
			/* -------- */
			deposit );

	return deposit;
}

LIST *education_payment_list(
			char *season_name,
			int year,
			/* ------------ */
			/* Stamp couple */
			/* ------------ */
			PAYPAL_DATASET *paypal_dataset,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit )
{
	LIST *payment_list = list_new();
	PAYMENT *payment;
	int student_number;

	for (	student_number = 1;
		( payment =
			education_payment(
				season_name,
				year,
				paypal_dataset->item_title_P,
				deposit ) );
		student_number++ )
	{
		list_set( payment_list, payment );
	}

	return payment_list;
}

PAYMENT *education_payment(
			char *season_name,
			int year,
			char *item_title_P,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit )
{
	PAYMENT *payment;
	char *student_name;
	char *street_address;
	char *course_name;
	int student_number;

	for (	student_number = 1;
		( payment =
}

PAYPAL_DATASET *education_paypal_dataset(
			char *input,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset )
{
	return *paypal_dataset;
}

