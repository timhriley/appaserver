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
#include "semester.h"
#include "offering.h"
#include "offering_fns.h"
#include "tuition_refund.h"
#include "tuition_refund_fns.h"
#include "tuition_payment.h"
#include "program_payment.h"
#include "program_payment_fns.h"
#include "product_payment.h"
#include "product_payment_fns.h"
#include "product_refund.h"
#include "product_refund_fns.h"
#include "paypal_deposit.h"
#include "spreadsheet.h"
#include "paypal.h"
#include "paypal_upload.h"
#include "paypal_dataset.h"
#include "paypal_item.h"
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

LIST *education_paypal_deposit_list(
			char *season_name,
			int year,
			char *spreadsheet_filename,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *education_program_list,
			LIST *education_product_list )
{
	LIST *paypal_list = list_new();
	char input_string[ 65536 ];
	FILE *spreadsheet_file;
	LIST *not_exists_course_name_list = list_new();

	/* ------------------------------------------ */
	/* Don't want to loose paypal_dataset pointer */
	/* ------------------------------------------ */
	PAYPAL_DATASET *dataset_return;

	/* -------------------------------------------- */
	/* The first line, the header, is already read. */
	/* -------------------------------------------- */
	int row_number = 1;

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
			paypal_list,
			education_paypal_deposit(
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
	return paypal_list;
}

PAYPAL_DEPOSIT *education_paypal_deposit(
			LIST *not_exists_course_name_list,
			char *season_name,
			int year,
			LIST *semester_offering_list,
			LIST *education_program_list,
			LIST *education_product_list,
			PAYPAL_DATASET *paypal_dataset,
			int row_number )
{
	PAYPAL_DEPOSIT *paypal_deposit;
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

	paypal_deposit = paypal_deposit_calloc();

	paypal_deposit->row_number = row_number;

	paypal_deposit->semester =
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
	
	paypal_deposit->deposit_date_time = strdup( deposit_date_time );

	/* Column D */
	/* -------- */
	if ( !paypal_dataset->full_name_D
	||  !*paypal_dataset->full_name_D )
	{
		paypal_dataset->full_name_D = PAYPAL_UPLOAD_DEFAULT_FULL_NAME;
	}

	paypal_deposit->payor_entity =
		entity_full_name_entity(
			paypal_dataset->full_name_D );

	/* Column H */
	/* -------- */
	paypal_deposit->deposit_amount =
		atof( paypal_dataset->gross_revenue_H );

	/* Column I */
	/* -------- */
	paypal_deposit->transaction_fee = 
		float_abs(
			atof( paypal_dataset->transaction_fee_I ) );

	/* Column J */
	/* -------- */
	paypal_deposit->net_revenue = atof( paypal_dataset->net_revenue_J );

	/* Column K */
	/* -------- */
	paypal_deposit->from_email_address =
	paypal_deposit->payor_entity->email_address =
		strdup( paypal_dataset->from_email_address_K );

	/* Column M */
	/* -------- */
	paypal_deposit->transaction_ID = paypal_dataset->transaction_ID_M;

	/* Column Z */
	/* -------- */
	paypal_deposit->invoice_number = paypal_dataset->invoice_number_Z;

	/* Column AD */
	/* --------- */
	paypal_deposit->account_balance =
		atof( paypal_dataset->account_balance_AD );

	/* Steady state */
	/* ------------ */
	paypal_deposit->paypal_item_list =
		paypal_item_list(
			not_exists_course_name_list
				/* not_exists_revenue_item_list */,
			paypal_item_entity_delimit(
				paypal_dataset->item_title_P ),
			paypal_dataset->transaction_type_E,
			education_paypal_allowed_list(
				offering_name_list(
					semester_offering_list ),
				program_name_list(
					education_program_list ),
				program_alias_name_list(
					education_program_list ),
				product_name_list(
					education_product_list ) ) );

	if ( !list_length( paypal_deposit->paypal_item_list ) )
	{
		return (PAYPAL_DEPOSIT *)0;
	}

	paypal_deposit_set_paypal_item_expected_revenue(
		paypal_deposit->paypal_item_list,
		semester_offering_list );

	paypal_deposit->paypal_item_expected_revenue_total =
		paypal_item_expected_revenue_total(
			paypal_deposit->paypal_item_list );

	paypal_deposit->paypal_item_nonexpected_revenue_length =
		paypal_item_nonexpected_revenue_length(
			paypal_deposit->paypal_item_list );

	paypal_deposit->paypal_item_expected_revenue_length =
		paypal_item_expected_revenue_length(
			paypal_deposit->paypal_item_list );

	paypal_deposit->paypal_item_steady_state_list =
		paypal_item_steady_state_list(
			paypal_deposit->paypal_item_list,
			paypal_deposit->deposit_amount,
			paypal_deposit->transaction_fee,
			paypal_deposit->paypal_item_expected_revenue_total,
			paypal_deposit->paypal_item_nonexpected_revenue_length,
			paypal_deposit->paypal_item_expected_revenue_length );

	/* Columns E and P */
	/* --------------- */
	if ( paypal_deposit->deposit_amount > 0.0 )
	{
		paypal_deposit->tuition_payment_list =
			tuition_payment_list(
				season_name,
				year,
				paypal_deposit->paypal_item_steady_state_list,
				semester_offering_list,
				/* -------- */
				/* Set only */
				/* -------- */
				paypal_deposit );
	
		paypal_deposit->program_payment_list =
			program_payment_list(
				paypal_deposit->paypal_item_steady_state_list,
				education_program_list,
				/* -------- */
				/* Set only */
				/* -------- */
				paypal_deposit );

		paypal_deposit->product_payment_list =
			product_payment_list(
				paypal_deposit->paypal_item_steady_state_list,
				education_product_list,
				/* -------- */
				/* Set only */
				/* -------- */
				paypal_deposit );
	}
	else
	{
		paypal_deposit->tuition_refund_list =
			tuition_refund_list(
				season_name,
				year,
				paypal_deposit->paypal_item_steady_state_list,
				semester_offering_list,
				/* -------- */
				/* Set only */
				/* -------- */
				paypal_deposit );

		paypal_deposit->product_refund_list =
			product_refund_list(
				paypal_deposit->paypal_item_steady_state_list,
				education_product_list,
				/* -------- */
				/* Set only */
				/* -------- */
				paypal_deposit );
	}

	if ( list_length( paypal_deposit->tuition_payment_list )
	||   list_length( paypal_deposit->program_payment_list )
	||   list_length( paypal_deposit->product_payment_list )
	||   list_length( paypal_deposit->tuition_refund_list )
	||   list_length( paypal_deposit->product_refund_list ) )
	{
		return paypal_deposit;
	}
	else
	{
		return (PAYPAL_DEPOSIT *)0;
	}
}

LIST *education_paypal_deposit_list_insert(
			LIST *education_paypal_list )
{
	paypal_deposit_list_insert( education_paypal_list );
	paypal_list_registration_insert( education_paypal_list );
	paypal_list_enrollment_insert( education_paypal_list );
	paypal_list_tuition_payment_insert( education_paypal_list );
	paypal_list_program_payment_insert( education_paypal_list );
	paypal_list_product_payment_insert( education_paypal_list );
	paypal_list_tuition_refund_insert( education_paypal_list );
	paypal_list_student_insert( education_paypal_list );
	paypal_list_student_entity_insert( education_paypal_list );
	paypal_list_payor_entity_insert( education_paypal_list );

	return education_paypal_list;
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

	if ( ! ( paypal_dataset->transaction_type_E =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
				"Type" ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: can't parse transaction_type_E.\n",
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

LIST *education_paypal_allowed_list(
			LIST *offering_name_list,
			LIST *program_name_list,
			LIST *program_alias_name_list,
			LIST *product_name_list )
{
	LIST *allowed_list = list_new();

	list_append_list( allowed_list, offering_name_list );
	list_append_list( allowed_list, program_name_list );
	list_append_list( allowed_list, program_alias_name_list );
	list_append_list( allowed_list, product_name_list );

	return allowed_list;
}

double education_net_payment_amount(
			double payment_amount,
			double merchant_fees_expense )
{
	return	payment_amount -
		merchant_fees_expense;
}

double education_net_refund_amount(
			double refund_amount,
			double merchant_fees_expense )
{
	return	refund_amount +
		merchant_fees_expense;
}

