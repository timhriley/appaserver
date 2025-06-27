/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/paypal_dataset.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "list.h"
#include "boolean.h"
#include "paypal_spreadsheet.h"
#include "paypal_dataset.h"

PAYPAL_DATASET *paypal_dataset_calloc( void )
{
	PAYPAL_DATASET *paypal_dataset;

	if ( ! ( paypal_dataset = calloc( 1, sizeof( PAYPAL_DATASET ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return paypal_dataset;
}

char *paypal_dataset_date_A(
		LIST *paypal_spreadsheet_column_list,
		char *input,
		char *heading_label )
{
	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	paypal_spreadsheet_heading_data(
		paypal_spreadsheet_column_list,
		input,
		heading_label );
}

PAYPAL_DATASET *paypal_dataset_parse(
		LIST *paypal_spreadsheet_column_list,
		char *input )
{
	PAYPAL_DATASET *paypal_dataset;

	if ( !input || !*input ) return NULL;

	paypal_dataset = paypal_dataset_calloc();

	if ( ! ( paypal_dataset->date_A =
			paypal_dataset_date_A(
				paypal_spreadsheet_column_list,
				input,
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
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
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
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
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
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
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
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
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
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
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
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
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
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
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
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
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
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
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
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
				"Invoice Number" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse invoice_number_Z.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->quantity_AB =
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
				"Quantity" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't parse quantity_AB.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PAYPAL_DATASET *)0;
	}

	if ( ! ( paypal_dataset->account_balance_AD =
			paypal_spreadsheet_heading_data(
				paypal_spreadsheet_column_list,
				input,
				STATEMENT_BALANCE_HEADING ) ) )
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

