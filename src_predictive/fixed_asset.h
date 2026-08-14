/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define FIXED_ASSET_SELECT	"fixed_asset,"		\
				"asset_account,"	\
				"credit_account"

#define FIXED_ASSET_TABLE	"fixed_asset"

typedef struct
{
	char *asset_name;
	char *asset_account_name;
	char *credit_account_name;
/*
	char *cost_recovery_period_string;
	char *cost_recovery_method;
	char *cost_recovery_conversion;
	double activity_energy_kilowatt_draw;
	double activity_depreciation_per_hour;
*/
} FIXED_ASSET;

/* Usage */
/* ----- */
FIXED_ASSET *fixed_asset_fetch(
		char *asset_name );

/* Usage */
/* ----- */
FIXED_ASSET *fixed_asset_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
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

