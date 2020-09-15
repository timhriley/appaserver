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
			FILE *spreadsheet_file,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *semester_registration_list )
{
	LIST *deposit_list = list_new();
	DEPOSIT *deposit;
	char input[ 65536 ];

	while ( string_input( input, spreadsheet_file, 65536 ) )
	{
		if ( ( deposit =
				education_deposit(
					season_name,
					year,
					input,
					spreadsheet,
					paypal_dataset,
					semester_offering_list,
					semester_registration_list ) ) )
		{
			list_set( deposit_list, deposit );
		}
	}
	return deposit_list;
}

DEPOSIT *education_deposit(
			char *season_name,
			int year,
			char *input,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *semester_registration_list )
{
	paypal_dataset =
		education_dataset_parse(
			input,
			spreadsheet,
			/* Return only */
			/* ----------- */
			paypal_dataset );

	return education_deposit_parse(
			season_name,
			year,
			paypal_dataset,
			semester_offering_list,
			semester_registration_list );
}

PAYPAL_DATASET *education_dataset_parse(
			char *input,
			SPREADSHEET *spreadsheet,
			/* Return only */
			/* ----------- */
			PAYPAL_DATASET *paypal_dataset )
{
	return paypal_dataset;
}

DEPOSIT *education_deposit_parse(
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
			deposit->payor_entity->full_name,
			deposit->payor_entity->street_address,
			season_name,
			year,
			deposit->deposit_date_time,
			/* ------------ */
			/* Stamp couple */
			/* ------------ */
			paypal_dataset );

	return deposit;
}

LIST *education_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			/* ------------ */
			/* Stamp couple */
			/* ------------ */
			PAYPAL_DATASET *paypal_dataset )
{
	LIST *payment_list = list_new();
	PAYMENT *payment;

	return payment_list;
}
