/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/tuition_payment_item_title.h	*/
/* -----------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

#ifndef TUITION_PAYMENT_ITEM_TITLE_H
#define TUITION_PAYMENT_ITEM_TITLE_H

#include "boolean.h"
#include "list.h"
#include "entity.h"

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
} TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT;

typedef struct
{
	/* Input */
	/* ----- */
	char *item_title_P;
	int student_number;

	/* Process */
	/* ------- */
	TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *item_title_enrollment;
	ENTITY *tuition_payment_item_title_entity;
	char *tuition_payment_item_title_course_name;

} TUITION_PAYMENT_ITEM_TITLE;

/* Prototypes */
/* ---------- */
TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *
	tuition_payment_item_title_enrollment_calloc(
			void );

TUITION_PAYMENT_ITEM_TITLE *tuition_payment_item_title_calloc(
			void );

TUITION_PAYMENT_ITEM_TITLE *tuition_payment_item_title_new(
			char *item_title_P,
			int student_number );

ENTITY *tuition_payment_item_title_entity(
			TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT * );

/* ----------------------------------------------------- */
/* Note: COURSE.course_name can't have colons or commas. */
/* ----------------------------------------------------- */
/* Returns heap memory or null */
/* --------------------------- */
char *tuition_payment_item_title_course_name(
			TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT * );

TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *
	tuition_payment_item_title_enrollment_new(
			char *item_title_P,
			int student_number );

TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *
		tuition_payment_item_title_enrollment_block_A(
			char *item_title_P,
			int student_number );

TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *
		tuition_payment_item_title_enrollment_block_B(
			char *item_title_P,
			int student_number );

#endif
