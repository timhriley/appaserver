/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/prepaid_asset.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PREPAID_ASSET_H
#define PREPAID_ASSET_H

#include "list.h"
#include "boolean.h"
#include "entity.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *asset_name;
	char *asset_account;
	char *expense_account;
} PREPAID_ASSET;

/* Operations */
/* ---------- */
PREPAID_ASSET *prepaid_asset_new(
			char *asset_name );

#endif
