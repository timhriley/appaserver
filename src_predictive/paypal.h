/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/paypal.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef PAYPAL_H
#define PAYPAL_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"
#include "spreadsheet.h"
#include "paypal_datasheet.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	SPREADSHEET *spreadsheet;

	char *spreadsheet_filename;

	/* Process */
	/* ------- */
	PAYPAL_DATASHEET *paypal_datasheet;
} PAYPAL;

/* Operations */
/* ---------- */
PAYPAL *paypal_calloc( void );

PAYPAL *paypal_fetch(
			char *spreadsheet_name,
			char *spreadsheet_filename );

PAYPAL *paypal_new(
			char *spreadsheet_name,
			char *spreadsheet_filename );

#endif
