/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/prior_fixed_asset.h			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef PRIOR_FIXED_ASSET_H
#define PRIOR_FIXED_ASSET_H

#include "list.h"
#include "boolean.h"
#include "account.h"
#include "subsidiary_transaction_state.h"
#include "subsidiary_transaction.h"

#define PRIOR_FIXED_ASSET_TABLE		"prior_fixed_asset"

#define PRIOR_FIXED_ASSET_PRIMARY_KEY	"asset_name"

#define PRIOR_FIXED_ASSET_SELECT		\
	"full_name,"				\
	"street_address,"			\
	"purchase_date_time,"			\
	"fixed_asset_cost,"			\
	"asset_account"

typedef struct
{
	char *asset_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	double fixed_asset_cost;
	char *asset_account;
	ACCOUNT *debit_account;
	char *account_equity;
	ACCOUNT *credit_account;
	LIST *journal_binary_list;
	SUBSIDIARY_TRANSACTION_STATE *subsidiary_transaction_state;
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
} PRIOR_FIXED_ASSET;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PRIOR_FIXED_ASSET *
	prior_fixed_asset_fetch(
		char *asset_name,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_purchase_date_time );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *prior_fixed_asset_primary_where(
		const char *prior_fixed_asset_primary_key,
		char *asset_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PRIOR_FIXED_ASSET *prior_fixed_asset_parse(
		char *asset_name,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PRIOR_FIXED_ASSET *prior_fixed_asset_new(
		char *asset_name );

/* Process */
/* ------- */
PRIOR_FIXED_ASSET *prior_fixed_asset_calloc(
		void );

#endif
