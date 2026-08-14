/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/supply.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define SUPPLY_SELECT		"supply_name,"	\
				"account"

#define SUPPLY_TABLE		"supply"

typedef struct
{
	char *supply_name;
	char *account_name;
} SUPPLY;

/* Usage */
/* ----- */
SUPPLY *supply_fetch(
		char *supply_name );

/* Usage */
/* ----- */
SUPPLY *supply_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SUPPLY *supply_new(
		char *supply_name );

/* Process */
/* ------- */
SUPPLY *supply_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *supply_primary_where(
		char *supply_name );

