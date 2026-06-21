/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/fixed_asset_home.h			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef FIXED_ASSET_HOME_H
#define FIXED_ASSET_HOME_H

#include "list.h"
#include "boolean.h"
#include "account.h"
#include "subsidiary_transaction_state.h"
#include "subsidiary_transaction.h"

#define FIXED_ASSET_HOME_TABLE		"fixed_asset"

#define FIXED_ASSET_HOME_PRIMARY_KEY	"asset_name"

#define FIXED_ASSET_HOME_SELECT			\
	"full_name,"				\
	"purchase_date_time,"			\
	"fixed_asset_cost,"			\
	"asset_account"

typedef struct
{
	char *asset_name;
	char *full_name;
	char *purchase_date_time;
	double fixed_asset_cost;
	char *asset_account;
	ACCOUNT *debit_account;
	char *account_equity_string;
	ACCOUNT *credit_account;
	LIST *journal_binary_list;
	SUBSIDIARY_TRANSACTION_STATE *subsidiary_transaction_state;
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
} FIXED_ASSET_HOME;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_ASSET_HOME *fixed_asset_home_fetch(
		char *asset_name,
		char *state,
		char *preupdate_full_name,
		char *preupdate_purchase_date_time );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *fixed_asset_home_primary_where(
		const char *fixed_asset_home_primary_key,
		char *asset_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_ASSET_HOME *fixed_asset_home_parse(
		char *asset_name,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_ASSET_HOME *fixed_asset_home_new(
		char *asset_name );

/* Process */
/* ------- */
FIXED_ASSET_HOME *fixed_asset_home_calloc(
		void );

#endif
