/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/paypal_dataset.c		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

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
#include "spreadsheet.h"
#include "paypal_dataset.h"

PAYPAL_DATASET *paypal_dataset_calloc( void )
{
	PAYPAL_DATASET *a;

	if ( ! ( a = (PAYPAL_DATASET *)
			calloc( 1, sizeof( PAYPAL_DATASET ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}
	return a;
}

PAYPAL_DATASET *paypal_dataset_parse(
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

	if ( ! ( paypal_dataset->quantity_AB =
			spreadsheet_heading_data(
				spreadsheet_column_list,
				input_string,
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

