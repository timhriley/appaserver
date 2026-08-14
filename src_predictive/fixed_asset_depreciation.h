/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset_depreciation.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "fixed_asset.h"
#include "recovery.h"
#include "depreciation.h"

typedef struct
{
	char *asset_name;
	char *serial_label;
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
	double fixed_asset_depreciation_list_total;
	DEPRECIATION *last_depreciation;
	DEPRECIATION *depreciation;
	RECOVERY *last_recovery;
	RECOVERY *recovery;
	TRANSACTION *depreciation_transaction;
} FIXED_ASSET_DEPRECIATION;

/* Usage */
/* ----- */
LIST *fixed_asset_depreciation_list(
		const char *fixed_asset_depreciation_select,
		const char *fixed_asset_depreciation_table,
		char *purchase_primary_where,
		boolean entity_contact_key_boolean,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery );

/* Usage */
/* ----- */
LIST *fixed_asset_depreciation_system_list(
		char *fixed_asset_depreciation_system_string,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery );

/* Usage */
/* ----- */
FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation_fetch(
		char *asset_name,
		char *serial_label,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery );

/* Usage */
/* ----- */
FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation_parse(
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation_new(
		char *asset_name,
		char *serial_label );

/* Process */
/* ------- */
FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *fixed_asset_depreciation_primary_where(
			char *asset_name,
			char *serial_label );

/* Usage */
/* ----- */

/* ---------------------------------------- */
/* Returns fixed_asset_depreciation_list_fetch. */
/* ---------------------------------------- */
LIST *fixed_asset_depreciation_depreciation_list(
		LIST *fixed_asset_depreciation_list_fetch,
		char *depreciation_date );

/* Process */
/* ------- */

/* Returns last_depreciation->depreciation_date or null */
/* ---------------------------------------------------- */
char *fixed_asset_depreciation_prior_depreciation_date(
		DEPRECIATION *last_depreciation );

/* Usage */
/* ----- */
void fixed_asset_depreciation_list_update(
		LIST *fixed_asset_depreciation_list );

/* Usage */
/* ----- */
void fixed_asset_depreciation_update(
		FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation );

/* Process */
/* ------- */
FILE *fixed_asset_depreciation_update_pipe(
		char *fixed_asset_depreciation_table,
		char *fixed_asset_depreciation_primary_key );

void fixed_asset_depreciation_update_execute(
		FILE *fixed_asset_depreciation_update_pipe,
		double cost_basis,
		double finance_accumulated_depreciation,
		double tax_adjusted_basis,
		char *asset_name,
		char *serial_label );

/* Usage */
/* ----- */
LIST *fixed_asset_depreciation_transaction_list_extract(
		LIST *fixed_asset_depreciation_depreciation_list );

/* Usage */
/* ----- */
LIST *fixed_asset_depreciation_depreciation_list_extract(
		LIST *fixed_asset_depreciation_deprecation_list );

/* Usage */
/* ----- */
void fixed_asset_depreciation_transaction_list_insert(
		/* May reset depreciation_transaction->
			transaction_date_time */
		LIST *fixed_asset_depreciation_transaction_list_extract );

/* Usage */
/* ----- */
void fixed_asset_depreciation_depreciation_list_insert(
		LIST *fixed_asset_depreciation_depreciation_list_extract );

/* Driver */
/* ------ */
void fixed_asset_depreciation_depreciation_display(
		LIST *fixed_asset_depreciation_depreciation_list );

/* Driver */
/* ------ */
void fixed_asset_depreciation_depreciation_insert(
		LIST *fixed_asset_depreciation_depreciation_list );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *fixed_asset_depreciation_depreciation_where(
		char *depreciation_date );


/* Returns heap memory */
/* ------------------- */
char *fixed_asset_depreciation_depreciation_date(
		void );

double fixed_asset_depreciation_cost_basis(
		double fixed_asset_cost );

double fixed_asset_depreciation_tax_adjusted_basis(
		double fixed_asset_cost );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *fixed_asset_depreciation_system_string(
		char *select_string,
		const char *fixed_asset_depreciation_table,
		char *where,
		char *order );

/* Usage */
/* ----- */
double fixed_asset_depreciation_total(
		LIST *fixed_asset_depreciation_list );

/* Usage */
/* ----- */
void fixed_asset_depreciation_depreciation_display(
		LIST *fixed_asset_depreciation_list );

/* Usage */
/* ----- */
void fixed_asset_depreciation_recovery_display(
		LIST *fixed_asset_depreciation_list );

/* Usage */
/* ----- */
LIST *fixed_asset_depreciation_list_cost_recover(
		LIST *fixed_asset_depreciation_list,
		int tax_year );

/* Usage */
/* ----- */
LIST *fixed_asset_depreciation_cost_recovery_list(
		LIST *fixed_asset_depreciation_list );

/* Usage */
/* ----- */
void fixed_asset_depreciation_list_add_depreciation_amount(
		LIST *fixed_asset_depreciation_list );

/* Usage */
/* ----- */
void fixed_asset_depreciation_list_subtract_recovery_amount(
		LIST *fixed_asset_depreciation_list );

/* Usage */
/* ----- */
void fixed_asset_depreciation_subtract_recovery_amount(
		FIXED_ASSET_DEPRECIATION *fixed_asset_depreciation );

/* Usage */
/* ----- */
void fixed_asset_depreciation_negate_depreciation_amount(
		LIST *fixed_asset_depreciation_list );

/* Usage */
/* ----- */
void fixed_asset_depreciation_negate_recovery_amount(
		LIST *fixed_asset_depreciation_list );

