/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FIXED_ASSET_H
#define FIXED_ASSET_H

#include "list.h"
#include "boolean.h"

/* Constants */
/* --------- */
#define FIXED_ASSET_TABLE	"fixed_asset"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *asset_name;
	char *account_name;
	char *cost_recovery_period_string;
	char *cost_recovery_method;
	char *cost_recovery_conversion;
	double activity_energy_kilowatt_draw;
	double activity_depreciation_per_hour;
} FIXED_ASSET;

/* Operations */
/* ---------- */
FIXED_ASSET *fixed_asset_new(
			char *asset_name );

FIXED_ASSET *fixed_asset_parse(
			char *input );

FIXED_ASSET *fixed_asset_fetch(
			char *asset_name );

/* Returns static memory */
/* --------------------- */
char *fixed_asset_primary_where(
			char *asset_name );

/* Returns static memory */
/* --------------------- */
char *fixed_asset_system_string(
			char *where );

/* Returns static memory */
/* --------------------- */
char *fixed_asset_name_escape(
			char *asset_name );

#endif

