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
	LIST *education_deposit_list;
} EDUCATION;

EDUCATION *education_new(
			char *season_name,
			int year,
			char *spreadsheet_name,
			char *spreadsheet_filename );

EDUCATION *education_calloc(
			void );

EDUCATION *education_fetch(
			char *season_name,
			int year,
			char *spreadsheet_name,
			char *spreadsheet_filename );

LIST *education_deposit_list(
			char *season_name,
			int year,
			char *spreadsheet_filename,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset );

DEPOSIT *education_deposit(
			char *season_name,
			int year,
			PAYPAL_DATASET *paypal_dataset );

PAYPAL_DATASET *education_dataset_parse(
			char *input,
			SPREADSHEET *spreadsheet,
			/* Return only */
			/* ----------- */
			PAYPAL_DATASET *paypal_dataset );

LIST *education_payment_list(
			char *season_name,
			int year,
			char *item_title_P,
			double gross_revenue_H,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit );

PAYMENT *education_payment(
			char *season_name,
			int year,
			char *item_title_P,
			double gross_revenue_H,
			int student_number,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit );

PAYPAL_DATASET *education_paypal_dataset(
			char *input_string,
			SPREADSHEET *spreadsheet,
			/* ---------------------- */
			/* Returns paypal_dataset */
			/* ---------------------- */
			PAYPAL_DATASET *paypal_dataset );

void education_deposit_list_insert(
			LIST *deposit_list );

void education_deposit_insert(
			LIST *deposit_list );

void education_payment_insert(
			LIST *deposit_list );

void education_enrollment_insert(
			LIST *deposit_list );

void education_registration_insert(
			LIST *deposit_list );

void education_offering_insert(
			LIST *deposit_list );

void education_course_insert(
			LIST *deposit_list );

#endif

