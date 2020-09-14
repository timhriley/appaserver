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

void education_deposit_list_insert(
			LIST *education_deposit_list );

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
			FILE *spreadsheet_file,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *semester_registration_list );

DEPOSIT *education_deposit(
			char *season_name,
			int year,
			char *input,
			SPREADSHEET *spreadsheet,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *semester_registration_list );

PAYPAL_DATASET *education_dataset_parse(
			char *input,
			SPREADSHEET *spreadsheet,
			/* Return only */
			/* ----------- */
			PAYPAL_DATASET *paypal_dataset );

DEPOSIT *education_deposit_parse(
			char *season_name,
			int year,
			PAYPAL_DATASET *paypal_dataset,
			LIST *semester_offering_list,
			LIST *semester_registration_list );

#endif

