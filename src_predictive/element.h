/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/element.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef ELEMENT_H
#define ELEMENT_H

#include "list.h"
#include "boolean.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *element_name;
	boolean accumulate_debit;

	/* -------------------------- */
	/* if !omit_subclassification */
	/* -------------------------- */
	LIST *subclassification_list;

	/* ------------------------- */
	/* if omit_subclassification */
	/* ------------------------- */
	LIST *account_list;

	double element_total;
} ELEMENT;

/* Operations */
/* ---------- */

/* Returns static memory */
/* --------------------- */
ELEMENT *element_parse(	char *input );

ELEMENT *element_new(	char *element_name ); 

ELEMENT *element_account_name_fetch(
			char *account_name );

char *element_select(	void );

boolean element_accumulate_debit(
			char *element_name );

ELEMENT *element_fetch(	char *element_name );

#endif

