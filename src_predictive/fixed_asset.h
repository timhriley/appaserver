/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FIXED_ASSET_H
#define FIXED_ASSET_H

#include "list.h"
#include "boolean.h"

#define FIXED_ASSET_TABLE	"fixed_asset"

typedef struct
{
	char *asset_name;
	char *account_name;
	double activity_energy_kilowatt_draw;
	double activity_depreciation_per_hour;
} FIXED_ASSET;

/* Usage */
/* ----- */
FIXED_ASSET *fixed_asset_fetch(
		char *asset_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *fixed_asset_system_string(
		char *fixed_asset_table,
		char *fixed_asset_primary_where );

/* Usage */
/* ----- */
FIXED_ASSET *fixed_asset_parse(
		char *input );

/* Usage */
/* ----- */
FIXED_ASSET *fixed_asset_new(
		char *asset_name );

/* Process */
/* ------- */
FIXED_ASSET *fixed_asset_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *fixed_asset_primary_where(
		char *asset_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *fixed_asset_name_escape(
		char *asset_name );

#endif

