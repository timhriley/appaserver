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
#include "deposit.h"
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
	LIST *education_program_list;
	LIST *education_product_list;
	LIST *education_deposit_list;
} EDUCATION;

EDUCATION *education_calloc(
			void );

EDUCATION *education_fetch(
			char *season_name,
			int year,
			char *spreadsheet_filename,
			char *date_label );

LIST *education_deposit_list(
			LIST *not_found_item_list,
			char *season_name,
			int year,
			char *spreadsheet_filename,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *education_program_list,
			LIST *education_product_list );

DEPOSIT *education_deposit(
			LIST *not_found_item_list,
			char *season_name,
			int year,
			LIST *semester_offering_list,
			LIST *education_program_list,
			LIST *education_product_list,
			PAYPAL_DATASET *paypal_dataset,
			int row_number );

PAYPAL_DATASET *education_dataset_parse(
			char *input,
			LIST *spreadsheet_column_list,
			/* Return only */
			/* ----------- */
			PAYPAL_DATASET *paypal_dataset );

PAYPAL_DATASET *education_paypal_dataset(
			char *input_string,
			LIST *spreadsheet_column_list,
			/* ---------------------- */
			/* Returns paypal_dataset */
			/* ---------------------- */
			PAYPAL_DATASET *paypal_dataset );

LIST *education_deposit_list_insert(
			LIST *education_deposit_list );

void education_deposit_insert(
			LIST *deposit_list );

void education_tuition_payment_insert(
			LIST *deposit_list );

void education_program_payment_insert(
			LIST *deposit_list );

void education_tuition_refund_insert(
			LIST *deposit_list );

void education_enrollment_insert(
			LIST *deposit_list );

void education_registration_insert(
			LIST *deposit_list );

void education_student_insert(
			LIST *deposit_list );

void education_student_entity_insert(
			LIST *deposit_list );

void education_payor_entity_insert(
			LIST *deposit_list );

LIST *education_program_list(
			void );

LIST *education_product_list(
			void );

void education_product_payment_insert(
			LIST *deposit_list );

LIST *education_paypal_allowed_list(
			LIST *offering_name_list,
			LIST *program_name_list,
			LIST *program_alias_name_list,
			LIST *product_name_list );

#endif

