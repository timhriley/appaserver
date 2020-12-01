/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/education.c		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "boolean.h"
#include "float.h"
#include "list.h"
#include "date_convert.h"
#include "transaction.h"
#include "entity.h"
#include "enrollment.h"
#include "offering.h"
#include "offering_fns.h"
#include "semester.h"
#include "deposit.h"
#include "spreadsheet.h"
#include "paypal_dataset.h"
#include "program.h"
#include "product.h"
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

EDUCATION *education_fetch(
			char *season_name,
			int year,
			char *spreadsheet_filename,
			char *date_label )
{
	EDUCATION *education = education_calloc();

	education->semester =
		semester_fetch(
			strdup( season_name ),
			year,
			1 /* not fetch_offering_list */ );

	education->paypal =
		paypal_fetch(
			spreadsheet_filename,
			date_label );

	return education;
}

LIST *education_deposit_list(
			LIST *not_exists_course_name_list,
			char *season_name,
			int year,
			char *spreadsheet_filename,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *education_program_list,
			LIST *education_product_list )
{
	LIST *deposit_list = list_new();
	char input_string[ 65536 ];
	FILE *spreadsheet_file;
	/* ------------------------------------------ */
	/* Don't want to loose paypal_dataset pointer */
	/* ------------------------------------------ */
	PAYPAL_DATASET *dataset_return;
	int row_number = 0;

	if ( !spreadsheet )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty spreadsheet.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

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

	while ( string_input( input_string, spreadsheet_file, 65536 ) )
	{
		if ( ! ( dataset_return =
				/* ---------------------- */
				/* Returns paypal_dataset */
				/* ---------------------- */
				education_paypal_dataset(
					input_string,
					spreadsheet->spreadsheet_column_list,
					paypal_dataset ) ) )
		{
			continue;
		}

		if ( !dataset_return->date_A
		||   !*dataset_return->date_A
		||   !isdigit( *dataset_return->date_A ) )
		{
			continue;
		}

		list_set(
			deposit_list,
			education_deposit(
				not_exists_course_name_list,
				season_name,
				year,
				semester_offering_list,
				education_program_list,
				education_product_list,
				dataset_return
					/* paypal_dataset */,
				++row_number ) );
	}

	fclose( spreadsheet_file );
	return deposit_list;
}

DEPOSIT *education_deposit(
			LIST *not_exists_course_name_list,
			char *season_name,
			int year,
			LIST *semester_offering_list,
			LIST *education_program_list,
			LIST *education_product_list,
			PAYPAL_DATASET *paypal_dataset,
			int row_number )
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

	deposit->row_number = row_number;

	deposit->semester =
		semester_new(
			season_name,
			year );

	/* Columns A and B */
	/* --------------- */
	sprintf(deposit_date_time,
		"%s %s",
		/* --------------------------------- */
		/* Looks like: 1/2/2020 and 12:28:39 */
		/* --------------------------------- */
		date_convert_international_string(
			paypal_dataset->date_A ),
		paypal_dataset->time_B );
	
	deposit->deposit_date_time = strdup( deposit_date_time );

	/* Column D */
	/* -------- */
	if ( !paypal_dataset->full_name_D
	||  !*paypal_dataset->full_name_D )
	{
		return (DEPOSIT *)0;
	}

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
				ENTITY_STREET_ADDRESS_UNKNOWN );
	}

	/* Column H */
	/* -------- */
	deposit->deposit_amount = atof( paypal_dataset->gross_revenue_H );

	/* Column I */
	/* -------- */
	deposit->transaction_fee = 
		float_abs(
			atof( paypal_dataset->transaction_fee_I ) );

	/* Column J */
	/* -------- */
	deposit->net_revenue = atof( paypal_dataset->net_revenue_J );

	/* Column K */
	/* -------- */
	deposit->from_email_address =
	deposit->payor_entity->email_address =
		strdup( paypal_dataset->from_email_address_K );

	/* Column M */
	/* -------- */
	deposit->transaction_ID = paypal_dataset->transaction_ID_M;

	/* Column Z */
	/* -------- */
	deposit->invoice_number = paypal_dataset->invoice_number_Z;

	/* Column AD */
	/* --------- */
	deposit->account_balance = atof( paypal_dataset->account_balance_AD );

	if ( deposit->deposit_amount > 0.0 )
	{
		deposit->deposit_tuition_payment_list =
			deposit_tuition_payment_list(
				not_exists_course_name_list,
				season_name,
				year,
				paypal_dataset->item_title_P,
				semester_offering_list,
				/* -------- */
				/* Set only */
				/* -------- */
				deposit );
	
		deposit->deposit_program_payment_list =
			deposit_program_payment_list(
				paypal_dataset->item_title_P,
				education_program_list,
				/* -------- */
				/* Set only */
				/* -------- */
				deposit );

		if ( !list_length( deposit->deposit_program_payment_list ) )
		{
			deposit->deposit_product_payment_list =
				deposit_product_payment_list(
					paypal_dataset->item_title_P,
					education_product_list,
					/* -------- */
					/* Set only */
					/* -------- */
					deposit );
		}
	}
	else
	{
		deposit->deposit_tuition_refund_list =
			deposit_tuition_refund_list(
				season_name,
				year,
				paypal_dataset->item_title_P,
				/* -------- */
				/* Set only */
				/* -------- */
				deposit );
	}

	if ( list_length( deposit->deposit_tuition_payment_list )
	||   list_length( deposit->deposit_program_payment_list )
	||   list_length( deposit->deposit_product_payment_list )
	||   list_length( deposit->deposit_tuition_refund_list ) )
	{
		return deposit;
	}
	else
	{
		return (DEPOSIT *)0;
	}
}

LIST *education_deposit_list_insert(
			LIST *education_deposit_list )
{
	education_deposit_insert( education_deposit_list );
	education_registration_insert( education_deposit_list );
	education_enrollment_insert( education_deposit_list );
	education_tuition_payment_insert( education_deposit_list );
	education_program_payment_insert( education_deposit_list );
	education_product_payment_insert( education_deposit_list );
	education_tuition_refund_insert( education_deposit_list );
	education_student_insert( education_deposit_list );
	education_student_entity_insert( education_deposit_list );
	education_payor_entity_insert( education_deposit_list );

	return education_deposit_list;
}

void education_student_insert( LIST *deposit_list )
{
	deposit_list_student_insert( deposit_list );
}

void education_student_entity_insert( LIST *deposit_list )
{
	deposit_list_student_entity_insert( deposit_list );
}

void education_payor_entity_insert( LIST *deposit_list )
{
	deposit_list_payor_entity_insert( deposit_list );
}

void education_deposit_insert( LIST *deposit_list )
{
	deposit_list_insert( deposit_list );
}

void education_tuition_payment_insert( LIST *deposit_list )
{
	deposit_list_tuition_payment_insert( deposit_list );
}

void education_program_payment_insert( LIST *deposit_list )
{
	deposit_list_program_payment_insert( deposit_list );
}

void education_product_payment_insert( LIST *deposit_list )
{
	deposit_list_product_payment_insert( deposit_list );
}

void education_tuition_refund_insert( LIST *deposit_list )
{
	deposit_list_tuition_refund_insert( deposit_list );
}

void education_enrollment_insert( LIST *deposit_list )
{
	deposit_list_enrollment_insert( deposit_list );
}

void education_registration_insert( LIST *deposit_list )
{
	deposit_list_registration_insert( deposit_list );
}

PAYPAL_DATASET *education_paypal_dataset(
			char *input_string,
			LIST *spreadsheet_column_list,
			PAYPAL_DATASET *paypal_dataset )
{
	if ( !paypal_dataset )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty paypal_dataset\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !input_string || !*input_string )
		return (PAYPAL_DATASET *)0;

	if ( ! ( paypal_dataset->date_A =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Date" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse date_A.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->time_B =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Time" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse time_B.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->full_name_D =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Name" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse full_name_D.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->gross_revenue_H =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Gross" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse gross_revenue_H.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->transaction_fee_I =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Fee" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse transaction_fee_I.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->net_revenue_J =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Net" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse net_revenue_J.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->from_email_address_K =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"From Email Address" ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: can't parse from_email_address_K.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->transaction_ID_M =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Transaction ID" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse transaction_ID_M.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->item_title_P =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Item Title" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse item_title_P.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->invoice_number_Z =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Invoice Number" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse invoice_number_Z.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->account_balance_AD =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Balance" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse balance_AD.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	return paypal_dataset;
}

LIST *education_program_list( void )
{
	return program_list( 1 /* fetch_alias_list */ );
}

LIST *education_product_list( void )
{
	return product_list();
}

