/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/program_payment_item_title.h	*/
/* -----------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

#ifndef PROGRAM_PAYMENT_ITEM_TITLE_H
#define PROGRAM_PAYMENT_ITEM_TITLE_H

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
	/* Input */
	/* ----- */
	char *item_title_P;
	int program_number;

	/* Process */
	/* ------- */
	char *program_payment_item_title_name;

} PROGRAM_PAYMENT_ITEM_TITLE;

/* Prototypes */
/* ---------- */
PROGRAM_PAYMENT_ITEM_TITLE *
	program_payment_item_title_new(
			char *item_title_P,
			int program_number );

/* Returns heap memory or null */
/* --------------------------- */
char *program_payment_item_title_name(
			char *item_title_P,
			int program_number );

/* Returns static memory or null */
/* ----------------------------- */
char *program_payment_item_title_block(
			char *item_title_P,
			int program_number );

#endif