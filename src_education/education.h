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
	SEMESTER *semester;
	PAYPAL *paypal;
	LIST *program_list;
	LIST *product_list;
	LIST *event_list;
	LIST *education_paypal_deposit_list;
} EDUCATION;

EDUCATION *education_calloc(
			void );

EDUCATION *education_fetch(
			char *season_name,
			int year,
			char *spreadsheet_filename,
			char *date_label );

LIST *education_paypal_deposit_list(
			char *season_name,
			int year,
			char *spreadsheet_filename,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *program_list,
			LIST *product_list,
			LIST *event_list );

PAYPAL_DATASET *education_dataset_parse(
			char *input_string,
			LIST *spreadsheet_column_list,
			/* ----------- */
			/* Returns paypal_dataset */
			/* ---------------------- */
			PAYPAL_DATASET *paypal_dataset );

PAYPAL_DATASET *education_paypal_dataset(
			char *input_string,
			LIST *spreadsheet_column_list,
			/* ---------------------- */
			/* Returns paypal_dataset */
			/* ---------------------- */
			PAYPAL_DATASET *paypal_dataset );

void education_paypal_deposit_list_insert(
			LIST *education_paypal_deposit_list );

void education_product_payment_insert(
			LIST *education_paypal_deposit_list );

LIST *education_paypal_allowed_list(
			LIST *offering_name_list,
			LIST *program_name_list,
			LIST *program_alias_name_list,
			LIST *product_name_list,
			LIST *event_name_list );

double education_net_payment_amount(
			double payment_amount,
			double merchant_fees_expense );

double education_net_refund_amount(
			double refund_amount,
			double merchant_fees_expense );

#endif

