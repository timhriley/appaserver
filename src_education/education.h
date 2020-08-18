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

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	SEMESTER *semester;
	LIST *education_spreadsheet_deposit_list;
} EDUCATION;

EDUCATION *education_new(
			char *season_name,
			int year );

LIST  *education_spreadsheet_deposit_list(
			LIST *semester_offering_list,
			LIST *semester_registration_list,
			ENTITY *education_financial_institution_entity,
			char *spreadsheet_filename );

void education_deposit_list_insert(
			LIST *education_spreadsheet_deposit_list,
			ENTITY *education_financial_institution_entity );

#endif

