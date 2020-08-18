/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/education.c		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "boolean.h"
#include "list.h"
#include "transaction.h"
#include "entity.h"
#include "enrollment.h"
#include "offering.h"
#include "semester.h"
#include "education.h"

EDUCATION *education_new(
			char *season_name,
			int year )
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

	education->semester =
		semester_new(
			season_name,
			year );

	return education;
}

LIST  *education_spreadsheet_deposit_list(
			LIST *semester_offering_list,
			LIST *semester_registration_list,
			ENTITY *education_financial_institution_entity,
			char *spreadsheet_filename )
{
	return (LIST *)0;
}

void education_deposit_list_insert(
			LIST *education_spreadsheet_deposit_list,
			ENTITY *education_financial_institution_entity )
{
}

