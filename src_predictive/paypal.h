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
#include "paypal_dataset.h"

/* Constants */
/* --------- */
#define PAYPAL_ITEM_DELIMITER		','
#define PAYPAL_ENTITY_DELIMITER		'^'

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	SPREADSHEET *spreadsheet;

	/* Process */
	/* ------- */
	PAYPAL_DATASET *paypal_dataset;
} PAYPAL;

/* Operations */
/* ---------- */
PAYPAL *paypal_calloc(	void );

PAYPAL *paypal_column_list_fetch(
			char *spreadsheet_filename,
			char *date_label );

#endif
