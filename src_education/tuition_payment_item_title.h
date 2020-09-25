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
	/* Input */
	/* ----- */
	char *item_title_P;
	int student_number;

	/* Process */
	/* ------- */
	ENTITY *tuition_payment_item_title_entity;
	char *tuition_payment_item_title_course_name;

} TUITION_PAYMENT_ITEM_TITLE;

/* Prototypes */
/* ---------- */
TUITION_PAYMENT_ITEM_TITLE *tuition_payment_item_title_new(
			char *item_title_P,
			int student_number );

/* Note: COURSE.course_name can't have colons or commas. */
/* ----------------------------------------------------- */
ENTITY *tuition_payment_item_title_entity(
			char *item_title_P,
			int student_number );

/* Returns heap memory or null */
/* --------------------------- */
char *tuition_payment_item_title_course_name(
			char *item_title_P,
			int student_number );

/* Returns static memory or null */
/* ----------------------------- */
char *tuition_payment_item_title_enrollment_block(
			char *item_title_P,
			int student_number );
#endif
