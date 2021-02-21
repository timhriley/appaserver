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
#include "paypal_sweep.h"
#include "program_donation.h"
#include "tuition_payment.h"
#include "tuition_refund.h"
#include "product_sale.h"
#include "product_refund.h"
#include "ticket_sale.h"
#include "ticket_refund.h"
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

EDUCATION *education_spreadsheet_column_list_fetch(
			char *season_name,
			int year,
			char *spreadsheet_filename,
			char *date_label )
{
	EDUCATION *education = education_calloc();

	education->spreadsheet_filename = spreadsheet_filename;
	education->date_label = date_label;

	education->semester =
		semester_fetch(
			strdup( season_name ),
			year,
			1 /* fetch_offering_list */,
			1 /* fetch_event_list */ );

	education->paypal =
		paypal_column_list_fetch(
			spreadsheet_filename,
			date_label );

	education->program_list =
		program_list(
			1 /* fetch_alias_list */ );

	education->product_list = product_list();
	return education;
}

LIST *education_paypal_deposit_list(
			char *spreadsheet_filename,
			LIST *spreadsheet_column_list,
			PAYPAL_DATASET *paypal_dataset,
			char *season_name,
			int year,
			LIST *semester_offering_list,
			LIST *program_list,
			LIST *product_list,
			LIST *semester_event_list )
{
	LIST *paypal_deposit_list = list_new();
	char input_string[ 65536 ];
	FILE *spreadsheet_file;
	int row_number = 0;
	/* ------------------------------------------ */
	/* Don't want to loose paypal_dataset pointer */
	/* ------------------------------------------ */
	PAYPAL_DATASET *dataset_return;

	if ( !list_length( spreadsheet_column_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty spreadsheet_column_list.\n",
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
		row_number++;

		if ( ! ( dataset_return =
				/* ---------------------- */
				/* Returns paypal_dataset */
				/* ---------------------- */
				paypal_dataset_parse(
					input_string,
					spreadsheet_column_list,
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

		if ( string_strcmp(
			dataset_return->transaction_type_E,
			"General Withdrawal" ) == 0 )
		{
			list_set(
				paypal_deposit_list,
				paypal_deposit_sweep(
					dataset_return
						/* paypal_dataset */,
					row_number ) );
		}
		else
		{
			list_set(
				paypal_deposit_list,
				paypal_deposit_education(
					season_name,
					year,
					semester_offering_list,
					program_list,
					product_list,
					semester_event_list,
					dataset_return
						/* paypal_dataset */,
					row_number,
					paypal_deposit_list ) );
		}
	}

	fclose( spreadsheet_file );
	return paypal_deposit_list;
}

LIST *education_paypal_allowed_list(
			LIST *offering_name_list,
			LIST *program_name_list,
			LIST *program_alias_name_list,
			LIST *product_name_list )
{
	static LIST *allowed_list = {0};

	if ( allowed_list ) return allowed_list;

	allowed_list = list_new();

	list_unique_list( allowed_list, offering_name_list );
	list_unique_list( allowed_list, program_name_list );
	list_unique_list( allowed_list, program_alias_name_list );
	list_unique_list( allowed_list, product_name_list );

	return allowed_list;
}

double education_net_payment_amount(
			double payment_amount,
			double merchant_fees_expense )
{
	return	payment_amount +
		merchant_fees_expense;
}

double education_net_refund_amount(
			double refund_amount,
			double merchant_fees_expense )
{
	return	refund_amount +
		merchant_fees_expense;
}

LIST *education_existing_tuition_payment_list(
			char *spreadsheet_minimum_date )
{
	char where[ 128 ];

	sprintf(where,
		"payment_date_time >= '%s'",
		spreadsheet_minimum_date );

	return tuition_payment_list( where );
}

LIST *education_existing_tuition_refund_list(
			char *spreadsheet_minimum_date )
{
	char where[ 128 ];

	sprintf(where,
		"refund_date_time >= '%s'",
		spreadsheet_minimum_date );

	return tuition_refund_list( where );
}

LIST *education_existing_program_donation_list(
			char *spreadsheet_minimum_date )
{
	char where[ 128 ];

	sprintf(where,
		"payment_date_time >= '%s'",
		spreadsheet_minimum_date );

	return program_donation_list( where );
}

LIST *education_existing_product_sale_list(
			char *spreadsheet_minimum_date )
{
	char where[ 128 ];

	sprintf(where,
		"sale_date_time >= '%s'",
		spreadsheet_minimum_date );

	return product_sale_list( where );
}

LIST *education_existing_product_refund_list(
			char *spreadsheet_minimum_date )
{
	char where[ 128 ];

	sprintf(where,
		"refund_date_time >= '%s'",
		spreadsheet_minimum_date );

	return product_refund_list( where );
}

LIST *education_existing_ticket_sale_list(
			char *spreadsheet_minimum_date )
{
	char where[ 128 ];

	sprintf(where,
		"sale_date_time >= '%s'",
		spreadsheet_minimum_date );

	return ticket_sale_list(
			where,
			0 /* not fetch_event */ );
}

LIST *education_existing_ticket_refund_list(
			char *spreadsheet_minimum_date )
{
	char where[ 128 ];

	sprintf(where,
		"refund_date_time >= '%s'",
		spreadsheet_minimum_date );

	return ticket_refund_list(
			where,
			0 /* not fetch_sale */ );
}

LIST *education_existing_paypal_sweep_list(
			char *spreadsheet_minimum_date )
{
	char where[ 128 ];

	sprintf(where,
		"paypal_date_time >= '%s'",
		spreadsheet_minimum_date );

	return paypal_sweep_list( where );
}

LIST *education_paypal_exclude_existing_transaction_set(
			LIST *paypal_deposit_list,
			LIST *existing_program_donation_list,
			LIST *existing_tuition_payment_list,
			LIST *existing_tuition_refund_list,
			LIST *existing_product_sale_list,
			LIST *existing_product_refund_list,
			LIST *existing_ticket_sale_list,
			LIST *existing_ticket_refund_list,
			LIST *existing_paypal_sweep_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) )
		return paypal_deposit_list;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		paypal_deposit->exclude_existing_transaction =
			paypal_deposit_exclude_existing_transaction(
				paypal_deposit,
				existing_program_donation_list,
				existing_tuition_payment_list,
				existing_tuition_refund_list,
				existing_product_sale_list,
				existing_product_refund_list,
				existing_ticket_sale_list,
				existing_ticket_refund_list,
				existing_paypal_sweep_list );

	} while ( list_next( paypal_deposit_list ) );

	return paypal_deposit_list;
}

