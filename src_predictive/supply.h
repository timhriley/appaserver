/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/supply.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef SUPPLY_H
#define SUPPLY_H

#include "list.h"
#include "boolean.h"

/* Constants */
/* --------- */
#define SUPPLY_TABLE		"supply"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *supply_name;
	char *account_name;
} SUPPLY;

/* Operations */
/* ---------- */
SUPPLY *supply_new(
			char *supply_name );

SUPPLY *supply_parse(
			char *input );

SUPPLY *supply_fetch(
			char *supply_name );

/* Returns static memory */
/* --------------------- */
char *supply_system_string(
			char *where );

/* Returns static memory */
/* --------------------- */
char *supply_name_escape(
			char *supply_name );

/* Returns static memory */
/* --------------------- */
char *supply_primary_where(
			char *supply_name );


#endif

