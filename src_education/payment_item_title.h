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

ENTITY *payment_item_title_entity(
			char *item_title_P,
			int student_number );

char *payment_item_title_year(
			char *item_title_P,
			int student_number );

#endif
