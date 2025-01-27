/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/prepaid_asset_purchase.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PREPAID_ASSET_PURCHASE_H
#define PREPAID_ASSET_PURCHASE_H

#include "list.h"
#include "boolean.h"
#include "entity.h"
#include "transaction.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *vendor_entity;
	char *asset_name;
	char *purchase_date_time;
	char *accrual_date;
	double accrual_amount;
	TRANSACTION *transaction;
} PREPAID_ASSET_ACCRUAL;

typedef struct
{
	ENTITY *vendor_entity;
	char *asset_name;
	char *purchase_date_time;
	char *asset_account_name;
	char *expense_account_name;
	double cost;
	double accrual_period_years;
	double accumulated_accrual;
	LIST *accrual_list;
} PREPAID_ASSET_PURCHASE;

/* Operations */
/* ---------- */
PREPAID_ASSET_PURCHASE *prepaid_asset_purchase_new(
			char *full_name,
			char *street_address,
			char *asset_name,
			char *purchase_date_time );

PREPAID_ASSET_ACCRUAL *prepaid_asset_accrual_new(
			char *full_name,
			char *street_address,
			char *asset_name,
			char *purchase_date_time,
			char *accrual_date );

double prepaid_asset_purchase_total(
			LIST *prepaid_asset_purchase_list );

#endif

