/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/paypal.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef PAYPAL_H
#define PAYPAL_H

#include "list.h"
#include "boolean.h"
#include "spreadsheet.h"
#include "paypal_data.h"

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
	PAYPAL_DATA *paypal_data;
} PAYPAL;

/* Operations */
/* ---------- */
PAYPAL *paypal_calloc( void );

PAYPAL *paypal_fetch(
			char *spreadsheet_name,
			char *spreadsheet_filename );

#endif
