/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/payment_item_title.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PAYMENT_ITEM_TITLE_H
#define PAYMENT_ITEM_TITLE_H

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
	ENTITY *payment_item_title_entity;
	char *payment_item_title_course_name;

} PAYMENT_ITEM_TITLE;

/* Prototypes */
/* ---------- */
PAYMENT_ITEM_TITLE *payment_item_title_new(
			char *item_title_P,
			int student_number );

/* Note: COURSE.course_name can't have colons or commas. */
/* ----------------------------------------------------- */
ENTITY *payment_item_title_entity(
			char *item_title_P,
			int student_number );

/* Returns heap memory or null */
/* --------------------------- */
char *payment_item_title_course_name(
			char *item_title_P,
			int student_number );

/* Returns static memory or null */
/* ----------------------------- */
char *payment_item_title_enrollment_block(
			char *item_title_P,
			int student_number );
#endif