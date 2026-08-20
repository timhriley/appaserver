/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/prepaid_asset_purchase.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define PREPAID_ASSET_PURCHASE_TABLE	"prepaid_asset_purchase"

#define PREPAID_ASSET_PURCHASE_SELECT	"asset_name,"		\
					"extended_cost,"	\
					"accrual_period_years,"	\
					"accumulated_accrual"
typedef struct
{
	char *asset_name;
	double extended_cost;
	double accrual_period_years;
	double accumulated_accrual;
} PREPAID_ASSET_PURCHASE;

/* Usage */
/* ----- */
LIST *prepaid_asset_purchase_list(
		const char *prepaid_purchase_select,
		const char *prepaid_purchase_table,
		char *purchase_primary_where );

/* Usage */
/* ----- */
PREPAID_ASSET_PURCHASE *prepaid_asset_purchase_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PREPAID_ASSET_PURCHASE *prepaid_asset_purchase_new(
		char *asset_name );

/* Process */
/* ------- */
PREPAID_ASSET_PURCHASE *prepaid_asset_purchase_calloc(
		void );

/* Usage */
/* ----- */
double prepaid_asset_purchase_list_total(
		LIST *prepaid_asset_purchase_list );

