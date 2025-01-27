/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/paypal_dataset.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PAYPAL_DATASET_H
#define PAYPAL_DATASET_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	char *date_A;
	char *time_B;
	char *timezone_C;
	char *full_name_D;
	char *transaction_type_E;
	char *transaction_status_F;
	char *currency_G;
	char *gross_revenue_H;
	char *transaction_fee_I;
	char *net_revenue_J;
	char *from_email_address_K;
	char *to_email_address_L;
	char *transaction_ID_M;
	char *shipping_address_N;
	char *address_status_O;
	char *item_title_P;
	char *item_ID_Q;
	char *shipping_handling_amount_R;
	char *insurance_amount_S;
	char *sales_tax_T;
	char *option_1_name_U;
	char *option_1_value_V;
	char *option_2_name_W;
	char *opton_2_value_X;
	char *reference_txn_ID_Y;
	char *invoice_number_Z;
	char *custom_number_AA;
	char *quantity_AB;
	char *receipt_ID_AC;
	char *account_balance_AD;
	char *address_line_1_AE;
	char *address_2_district_neighborhood_AF;
	char *town_city_AG;
	char *state_province_AH;
	char *zip_code_AI;
	char *country_AJ;
	char *contact_phone_number_AK;
	char *subject_AL;
	char *note_AM;
	char *country_code_AN;
	char *balance_impact_AO;
} PAYPAL_DATASET;

/* Usage */
/* ----- */
PAYPAL_DATASET *paypal_dataset_parse(
		LIST *spreadsheet_column_list,
		char *input );

/* Process */
/* ------- */
PAYPAL_DATASET *paypal_dataset_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *paypal_dataset_date_A(
		LIST *paypal_spreadsheet_column_list,
		char *input,
		char *heading_label );

#endif
