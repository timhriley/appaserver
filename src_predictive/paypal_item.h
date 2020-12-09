/* ---------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/paypal_item.h	*/
/* ---------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#ifndef PAYPAL_ITEM
#define PAYPAL_ITEM

#include "boolean.h"
#include "list.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *course_name;
	char *student_full_name;
} ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT;

typedef struct
{
	/* Input */
	/* ----- */
	char *item_title_P;
	int student_number;

	/* Process */
	/* ------- */
	ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *item_title_enrollment;
	ENTITY *item_title_tuition_payment_entity;
	char *item_title_tuition_payment_course_name;

} ITEM_TITLE_TUITION_PAYMENT;

/* Prototypes */
/* ---------- */
ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *
	item_title_tuition_payment_enrollment_calloc(
			void );

ITEM_TITLE_TUITION_PAYMENT *item_title_tuition_payment_calloc(
			void );

ITEM_TITLE_TUITION_PAYMENT *item_title_tuition_payment_new(
			char *item_title_P,
			int student_number );

ENTITY *item_title_tuition_payment_entity(
			ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT * );

/* ----------------------------------------------------- */
/* Note: COURSE.course_name can't have colons or commas. */
/* ----------------------------------------------------- */
/* Returns heap memory or null */
/* --------------------------- */
char *item_title_tuition_payment_course_name(
			ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT * );

ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *
	item_title_tuition_payment_enrollment_new(
			char *item_title_P,
			int student_number );

ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *
		item_title_tuition_payment_enrollment_block_A(
			char *item_title_P,
			int student_number );

ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *
		item_title_tuition_payment_enrollment_block_B(
			char *item_title_P,
			int student_number );

/* Returns heap memory or null */
/* --------------------------- */
char *item_title_program_payemnt_name(
			char *item_title_P,
			char *transaction_type_E,
			int program_number,
			LIST *program_list );

/* Returns heap memory or null */
/* --------------------------- */
char *item_title_product_payment_name(
			char *item_title_P,
			int program_number,
			LIST *product_list );

/* Returns heap memory or null */
/* --------------------------- */
char *item_title_program_payment_block(
			char *item_title_P,
			char *transaction_type_E,
			int program_number );

char *item_title_program_payment_name(
			char *item_title_P,
			char *transaction_type_E,
			int program_number,
			LIST *program_list );

#endif
