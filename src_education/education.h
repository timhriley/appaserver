/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/education.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef EDUCATION_H
#define EDUCATION_H

#include "boolean.h"
#include "list.h"
#include "entity.h"
#include "semester.h"
#include "paypal.h"
#include "paypal_dataset.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *spreadsheet_filename;
	char *date_label;
	SEMESTER *semester;
	PAYPAL *paypal;
	LIST *program_list;
	LIST *product_list;
	char *spreadsheet_minimum_date;
	char *spreadsheet_maximum_date;
	int spreadsheet_row_count;
	LIST *paypal_deposit_list;
	LIST *existing_program_donation_list;
	LIST *existing_tuition_payment_list;
	LIST *existing_tuition_refund_list;
	LIST *existing_product_sale_list;
	LIST *existing_product_refund_list;
	LIST *existing_ticket_sale_list;
	LIST *existing_ticket_refund_list;
	LIST *existing_paypal_sweep_list;
} EDUCATION;

EDUCATION *education_calloc(
			void );

EDUCATION *education_spreadsheet_column_list_fetch(
			char *season_name,
			int year,
			char *spreadsheet_filename,
			char *date_label );

LIST *education_paypal_deposit_list(
			char *spreadsheet_filename,
			LIST *spreadsheet_column_list,
			PAYPAL_DATASET *paypal_dataset,
			char *season_name,
			int year,
			LIST *semester_offering_list,
			LIST *program_list,
			LIST *product_list,
			LIST *semester_event_list );

void education_product_payment_insert(
			LIST *education_paypal_deposit_list );

LIST *education_paypal_allowed_list(
			LIST *offering_name_list,
			LIST *program_name_list,
			LIST *program_alias_name_list,
			LIST *product_name_list );

double education_net_payment_amount(
			double payment_amount,
			double merchant_fees_expense );

double education_net_refund_amount(
			double refund_amount,
			double merchant_fees_expense );

LIST *education_existing_program_donation_list(
			char *spreadsheet_minimum_date );

LIST *education_existing_tuition_payment_list(
			char *spreadsheet_minimum_date );

LIST *education_existing_tuition_refund_list(
			char *spreadsheet_minimum_date );

LIST *education_existing_product_sale_list(
			char *spreadsheet_minimum_date );

LIST *education_existing_product_refund_list(
			char *spreadsheet_minimum_date );

LIST *education_existing_ticket_sale_list(
			char *spreadsheet_minimum_date );

LIST *education_existing_ticket_refund_list(
			char *spreadsheet_minimum_date );

LIST *education_existing_paypal_sweep_list(
			char *spreadsheet_minimum_date );

/* Return paypal_deposit_list */
/* -------------------------- */
LIST *education_paypal_existing_transaction_set(
			LIST *paypal_deposit_list,
			LIST *education_existing_program_donation_list,
			LIST *education_existing_tuition_payment_list,
			LIST *education_existing_tuition_refund_list,
			LIST *education_existing_product_sale_list,
			LIST *education_existing_product_refund_list,
			LIST *education_existing_ticket_sale_list,
			LIST *education_existing_ticket_refund_list,
			LIST *education_existing_paypal_sweep_list );

#endif

