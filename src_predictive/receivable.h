/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/receivable.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef RECEIVABLE_H
#define RECEIVABLE_H

#include "list.h"
#include "boolean.h"
#include "account.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *full_name;
	char *street_address;

	/* Process */
	/* ------- */
	LIST *receivable_entity_account_list;
	double receivable_expecting;
} RECEIVABLE;

/* Operations */
/* ---------- */
RECEIVABLE *receivable_new(
			char *full_name,
			char *street_address );

/* Sets receivable_expecting */
/* ------------------------- */
RECEIVABLE *receivable_steady_state(
			RECEIVABLE *receivable );

/* Sets account->account_receivable_due */
/* ------------------------------------ */
LIST *receivable_entity_account_list(
			char *full_name,
			char *street_address );

double receivable_expecting(
	LIST *receivable_entity_account_list );

#endif
