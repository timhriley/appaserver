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
			char *offering_revenue_account,
			int seconds_to_add );

char *enrollment_sys_string(
			char *where );

FILE *enrollment_update_open(
			void );

void enrollment_cancelled_date_update(
			FILE *update_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *cancelled_date );

LIST *enrollment_system_list(
			char *sys_string,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list,
			boolean fetch_offering,
			boolean fetch_registration );

LIST *enrollment_tuition_payment_list(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

LIST *enrollment_tuition_refund_list(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

FILE *enrollment_insert_open(
			char *error_filename );

void enrollment_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *transaction_date_time );

LIST *enrollment_course_name_list(
			LIST *enrollment_list );

void enrollment_trigger(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

char *enrollment_memo(	char *program_name );

void enrollment_list_steady_state(
			int *transaction_seconds_to_add,
			LIST *registration_enrollment_list,
			double deposit_amount );

void enrollment_list_cancelled_update(
			LIST *enrollment_list,
			char *season_name,
			int year );

void enrollment_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *enrollment_list );

#endif

