/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/enrollment.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include "list.h"
#include "boolean.h"
#include "offering.h"
#include "registration.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define ENROLLMENT_TABLE		"enrollment"

#define ENROLLMENT_PRIMARY_KEY		\
	"full_name,street_address,course_name,season_name,year"

#define ENROLLMENT_MEMO			"Enrollment"

#define ENROLLMENT_INSERT_COLUMNS	"full_name,"		\
					"street_address,"	\
					"course_name,"		\
					"season_name,"		\
					"year,"			\
					"transaction_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	OFFERING *offering;
	REGISTRATION *registration;
	TRANSACTION *enrollment_transaction;
	char *transaction_date_time;
	LIST *enrollment_tuition_payment_list;
	LIST *enrollment_tuition_refund_list;
} ENROLLMENT;

ENROLLMENT *enrollment_new(
			char *full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list,
			boolean fetch_offering,
			boolean fetch_registration );

ENROLLMENT *enrollment_parse(
			char *input,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list,
			boolean fetch_offering,
			boolean fetch_registration );

ENROLLMENT *enrollment_steady_state(
			ENROLLMENT *enrollment );

/* ----------------------------------- */
/* Place functions in enrollment_fns.h */
/* ----------------------------------- */
#endif

