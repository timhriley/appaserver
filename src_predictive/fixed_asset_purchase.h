/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset_purchase.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FIXED_ASSET_PURCHASE_H
#define FIXED_ASSET_PURCHASE_H

#include "list.h"
#include "boolean.h"
#include "entity.h"
#include "fixed_asset.h"
#include "recovery.h"
#include "depreciation.h"

#define FIXED_ASSET_PURCHASE_TABLE	"fixed_asset_purchase"

#define FIXED_ASSET_PURCHASE_PRIMARY_KEY \
					"asset_name,"		\
					"serial_label"

typedef struct
{
	char *asset_name;
	char *serial_label;
	ENTITY *vendor_entity;
	char *purchase_date_time;
	char *service_placement_date;
	double fixed_asset_cost;
	int units_produced_so_far;
	char *disposal_date;
	char *recovery_class_year_string;
	char *recovery_method;
	char *recovery_convention;
	char *recovery_system;
	enum depreciation_method depreciation_method;
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int estimated_residual_value;
	double declining_balance_n;
	double cost_basis;
	double finance_accumulated_depreciation;
	double tax_adjusted_basis;
	FIXED_ASSET *fixed_asset;
	double fixed_asset_purchase_list_total;
	DEPRECIATION *last_depreciation;
	DEPRECIATION *depreciation;
	RECOVERY *last_recovery;
	RECOVERY *recovery;
	TRANSACTION *depreciation_transaction;
} FIXED_ASSET_PURCHASE;

/* Usage */
/* ----- */
LIST *fixed_asset_purchase_list_fetch(
		char *fixed_asset_purchase_depreciation_where,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery );

/* Usage */
/* ----- */
LIST *fixed_asset_purchase_system_list(
		char *fixed_asset_purchase_system_string,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery );

/* Process */
/* ------- */
FILE *fixed_asset_purchase_input_pipe(
		char *fixed_asset_purchase_system_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_fetch(
		char *asset_name,
		char *serial_label,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery );

/* Usage */
/* ----- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse(
		char *input,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_new(
		char *asset_name,
		char *serial_label );

/* Process */
/* ------- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *fixed_asset_purchase_primary_where(
			char *asset_name,
			char *serial_label );

/* Usage */
/* ----- */

/* ---------------------------------------- */
/* Returns fixed_asset_purchase_list_fetch. */
/* ---------------------------------------- */
LIST *fixed_asset_purchase_depreciation_list(
		LIST *fixed_asset_purchase_list_fetch,
		char *depreciation_date );

/* Process */
/* ------- */

/* Returns last_depreciation->depreciation_date or null */
/* ---------------------------------------------------- */
char *fixed_asset_purchase_prior_depreciation_date(
		DEPRECIATION *last_depreciation );

/* Usage */
/* ----- */
void fixed_asset_purchase_list_update(
		LIST *fixed_asset_purchase_list );

/* Usage */
/* ----- */
void fixed_asset_purchase_update(
		FIXED_ASSET_PURCHASE *fixed_asset_purchase );

/* Process */
/* ------- */
FILE *fixed_asset_purchase_update_pipe(
		char *fixed_asset_purchase_table,
		char *fixed_asset_purchase_primary_key );

void fixed_asset_purchase_update_execute(
		FILE *fixed_asset_purchase_update_pipe,
		double cost_basis,
		double finance_accumulated_depreciation,
		double tax_adjusted_basis,
		char *asset_name,
		char *serial_label );

/* Usage */
/* ----- */
LIST *fixed_asset_purchase_transaction_list_extract(
		LIST *fixed_asset_purchase_depreciation_list );

/* Usage */
/* ----- */
LIST *fixed_asset_purchase_depreciation_list_extract(
		LIST *fixed_asset_purchase_deprecation_list );

/* Usage */
/* ----- */
void fixed_asset_purchase_transaction_list_insert(
		/* May reset depreciation_transaction->
			transaction_date_time */
		LIST *fixed_asset_purchase_transaction_list_extract );

/* Usage */
/* ----- */
void fixed_asset_purchase_depreciation_list_insert(
		LIST *fixed_asset_purchase_depreciation_list_extract );

/* Driver */
/* ------ */
void fixed_asset_purchase_depreciation_display(
		LIST *fixed_asset_purchase_depreciation_list );

/* Driver */
/* ------ */
void fixed_asset_purchase_depreciation_insert(
		LIST *fixed_asset_purchase_depreciation_list );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *fixed_asset_purchase_depreciation_where(
		char *depreciation_date );


/* Returns heap memory */
/* ------------------- */
char *fixed_asset_purchase_depreciation_date(
		void );

double fixed_asset_purchase_cost_basis(
		double fixed_asset_cost );

double fixed_asset_purchase_tax_adjusted_basis(
		double fixed_asset_cost );

/* Returns heap memory */
/* ------------------- */
char *fixed_asset_purchase_system_string(
		char *fixed_asset_purchase_table,
		char *where,
		char *order );

double fixed_asset_purchase_total(
		LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_depreciation_display(
		LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_recovery_display(
		LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_finance_fetch_update(
		char *asset_name,
		char *serial_label );

void fixed_asset_purchase_cost_fetch_update(
		char *asset_name,
		char *serial_label );

LIST *fixed_asset_purchase_list_cost_recover(
		LIST *fixed_asset_purchase_list,
		int tax_year );

LIST *fixed_asset_purchase_cost_recovery_list(
		LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_list_add_depreciation_amount(
		LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_list_subtract_recovery_amount(
		LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_subtract_recovery_amount(
		FIXED_ASSET_PURCHASE *fixed_asset_purchase );

void fixed_asset_purchase_negate_depreciation_amount(
		LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_negate_recovery_amount(
		LIST *fixed_asset_purchase_list );

#endif

