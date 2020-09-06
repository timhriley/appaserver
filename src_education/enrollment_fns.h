/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/enrollment_fns.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef ENROLLMENT_FNS_H
#define ENROLLMENT_FNS_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"

/* Returns static memory */
/* --------------------- */
char *enrollment_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

TRANSACTION *enrollment_transaction(
			char *student_full_name,
			char *street_address,
			char *registration_date_time
				/* transaction_date_time */,
			char *program_name,
			double offering_course_price,
			char *account_receivable,
			ACCOUNT *offering_revenue_account );

char *enrollment_sys_string(
			char *where );

FILE *enrollment_update_open(
			void );

void enrollment_update(
			char *transaction_date_time,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

LIST *enrollment_system_list(
			char *sys_string,
			boolean fetch_payment_list );

LIST *enrollment_payment_list(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

#endif

