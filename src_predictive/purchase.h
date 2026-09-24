/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "purchase_transaction.h"
#include "purchase_calculate.h"
#include "purchase_fetch.h"
#include "purchase_update.h"

#define PURCHASE_SELECT			"full_name,"			\
					"purchase_date_time,"		\
					"sales_tax,"			\
					"freight_in,"			\
					"fixed_asset_total,"		\
					"supply_total,"			\
					"service_total,"		\
					"invoice_amount,"		\
					"transaction_date_time"

#define PURCHASE_TABLE			"purchase"
#define PURCHASE_DATE_TIME_COLUMN	"purchase_date_time"
#define PURCHASE_ASSET_COLUMN		"asset_name"
#define PURCHASE_SUPPLY_COLUMN		"supply_name"
#define PURCHASE_MEMO			"Purchase Order"

typedef struct
{
	PURCHASE_FETCH *purchase_fetch;
	PURCHASE_CALCULATE *purchase_calculate;
	PURCHASE_TRANSACTION *purchase_transaction;
	PURCHASE_UPDATE *purchase_update;
} PURCHASE;

/* Usage */
/* ----- */
PURCHASE *purchase_trigger_new(
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *state );

/* Process */
/* ------- */
PURCHASE *purchase_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *purchase_primary_where(
		const char *purchase_date_time_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

