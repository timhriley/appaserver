/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_purchase.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INVENTORY_PURCHASE_H
#define INVENTORY_PURCHASE_H

#include "list.h"
#include "hash_table.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define INVENTORY_PURCHASE_TABLE	"inventory_purchase"

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *vendor_entity;
	char *purchase_date_time;
	char *inventory_name;
	char *arrived_date_time;
	int ordered_quantity;
	double unit_cost;
	double extended_cost;
	double capitalized_unit_cost;
	int missing_quantity;
	int arrived_quantity;
	int quantity_on_hand;
	int layer_consumed_quantity;
	double average_unit_cost;
	char *inventory_account_name;
	LIST *inventory_purchase_return_list;
} INVENTORY_PURCHASE;

/* Operations */
/* ---------- */
INVENTORY_PURCHASE *inventory_purchase_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name );

double inventory_purchase_total(
			LIST *inventory_purchase_list );

/*
double inventory_purchase_sum_extended_cost(
			LIST *inventory_purchase_list );

FILE *inventory_purchase_update_pipe(
			void );

void inventory_purchase_list_update(
			LIST *inventory_purchase_list );

LIST *inventory_purchase_entity_list(
			char *full_name,
			char *street_address,
			char *inventory_name,
			char *purchase_date_time );

LIST *inventory_purchase_date_list(
			char *inventory_name,
			char *purchase_date_time );

LIST *inventory_purchase_arrived_date_list(
			char *inventory_name,
			char *earliest_arrived_date_time,
			char *latest_arrived_date_time );

void inventory_purchase_reset_quantity_on_hand(
			LIST *purchase_list );

char *inventory_earliest_quantity_on_hand_purchase_date_time(
			char *inventory_name );

void inventory_purchase_fifo_decrement_quantity_on_hand(
			LIST *purchase_list,
			int quantity );

void inventory_purchase_lifo_decrement_quantity_on_hand(
			LIST *purchase_list,
			int quantity );

char *inventory_purchase_latest_date_time(
			char *inventory_name );

char *inventory_purchase_list_display(
			LIST *purchase_list );

INVENTORY_PURCHASE *inventory_purchase_list_seek(
			LIST *inventory_purchase_list,
			char *inventory_name );

char *inventory_purchase_prior_purchase_date_time(
			char *inventory_name,
			char *purchase_date_time );

void inventory_purchase_list_reset_quantity_on_hand(
			LIST *inventory_purchase_list );

INVENTORY_PURCHASE *inventory_prior_inventory_purchase(
			char *inventory_name,
			char *purchase_date_time );

char *inventory_purchase_join_where(
			void );

INVENTORY_PURCHASE *inventory_purchase_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name );

void inventory_purchase_arrived_pipe_update(
			FILE *update_pipe,
			char *inventory_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			int arrived_quantity );

void inventory_purchase_pipe_update(
			FILE *update_pipe,
			char *inventory_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			int arrived_quantity,
			int quantity_on_hand,
			double extended_cost,
			double capitalized_unit_cost,
			double average_unit_cost );

void inventory_purchase_update(
			char *inventory_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			int arrived_quantity,
			int quantity_on_hand,
			double extended_cost,
			double capitalized_unit_cost,
			double average_unit_cost );

char *inventory_max_arrived_purchase_date_time(
			char *inventory_name );

INVENTORY_PURCHASE *inventory_last_inventory_purchase(
			char *inventory_name );

char *inventory_purchase_max_arrived_date_time(
			char *inventory_name );

char *inventory_purchase_max_completed_date_time(
			char *inventory_name );

double inventory_purchase_total_cost_balance(
			int *quantity_on_hand,
			double *average_unit_cost,
			double total_cost_balance,
			double capitalized_unit_cost,
			int ordered_minus_returned_quantity );

void inventory_purchase_list_delete(
			LIST *inventory_purchase_list,
			char *purchase_date_time,
			char *inventory_name );

char *inventory_purchase_hash_table_key(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name );

char *inventory_purchase_select(
			void );

void inventory_purchase_parse(
			char **full_name,
			char **street_address,
			char **purchase_date_time,
			char **inventory_name,
			int *ordered_quantity,
			double *unit_cost,
			double *extended_cost,
			double *capitalized_unit_cost,
			int *arrived_quantity,
			int *missing_quantity,
			int *quantity_on_hand,
			double *average_unit_cost,
			char **arrived_date_time,
			char *input_buffer );

HASH_TABLE *inventory_arrived_inventory_purchase_hash_table(
			LIST **inventory_purchase_list,
			LIST **purchase_inventory_name_list,
			char *inventory_name );

LIST *inventory_inventory_purchase_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

double inventory_purchase_list_set_extended_cost(
			LIST *inventory_purchase_list );

INVENTORY_PURCHASE *inventory_seek_inventory_purchase(
			LIST *inventory_purchase_list,
			char *inventory_name );

void inventory_purchase_arrived_quantity_update(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			int arrived_quantity );

void inventory_purchase_layers_propagate(
			LIST *inventory_purchase_list,
			boolean force_not_latest );

void inventory_purchase_arrived_quantity_update_with_propagate(
			LIST *inventory_purchase_list,
			boolean force_not_latest );

double inventory_purchase_extended_cost(
			int ordered_quantity,
			double unit_cost );

int inventory_purchase_quantity_on_hand(
			int arrived_quantity,
			int missing_quantity,
			int returned_quantity );

void inventory_purchase_list_set_capitalized_unit_cost(
			LIST *inventory_purchase_list,
			double sum_inventory_extended_cost,
			double sales_tax,
			double freight_in );

LIST *inventory_latest_inventory_purchase_list(
			char *inventory_name );

int inventory_purchase_returned_quantity(
			LIST *inventory_purchase_return_list );

int inventory_purchase_quantity_minus_returned(
			int quantity,
			LIST *inventory_purchase_return_list );
*/
#endif

