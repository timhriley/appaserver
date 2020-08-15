/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INVENTORY_H
#define INVENTORY_H

#include "list.h"
#include "hash_table.h"
#include "inventory_purchase_return.h"

/* Enumerated types */
/* ---------------- */
enum inventory_cost_method{	inventory_not_set,
				inventory_fifo,
				inventory_average,
				inventory_lifo };


/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *account_name;
	double cost_of_goods_sold;
	double database_cost_of_goods_sold;
} INVENTORY_COST_ACCOUNT;

typedef struct
{
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *inventory_name;
	char *completed_date_time;
	int sold_quantity;
	double retail_price;
	double discount_amount;
	double extension;
	double database_extension;
	double cost_of_goods_sold;
	double database_cost_of_goods_sold;
	char *inventory_account_name;
	char *cost_of_goods_sold_account_name;
	LIST *inventory_sale_return_list;
	LIST *layer_inventory_purchase_list;
} INVENTORY_SALE;

typedef struct
{
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *inventory_name;
	char *arrived_date_time;
	int ordered_quantity;
	double unit_cost;
	double extension;
	double database_extension;
	double capitalized_unit_cost;
	double database_capitalized_unit_cost;
	int missing_quantity;
	int arrived_quantity;
	int database_arrived_quantity;
	int quantity_on_hand;
	int database_quantity_on_hand;
	int layer_consumed_quantity;
	double average_unit_cost;
	double database_average_unit_cost;
	char *inventory_account_name;
	LIST *inventory_purchase_return_list;
} INVENTORY_PURCHASE;

typedef struct
{
	INVENTORY_PURCHASE *inventory_purchase;
	INVENTORY_SALE *inventory_sale;
	double total_cost_balance;
	double average_unit_cost;
	int reorder_quantity;
	int quantity_on_hand;
} INVENTORY_BALANCE;

typedef struct
{
	char *inventory_name;
	char *inventory_account_name;
	char *cost_of_goods_sold_account_name;
	double retail_price;
	int reorder_quantity;
	LIST *inventory_purchase_list;
	LIST *inventory_sale_list;
	LIST *inventory_balance_list;
	LIST *inventory_cost_list;
	INVENTORY_BALANCE *last_inventory_balance;
} INVENTORY;

/* Operations */
/* ---------- */
enum inventory_cost_method inventory_get_cost_method(
					char *application_name );

double inventory_purchase_get_sum_extension(
					LIST *inventory_purchase_list );

double inventory_sale_get_sum_extension(
					LIST *inventory_sale_list );

double inventory_sale_get_sum_cost_of_goods_sold(
					LIST *inventory_sale_list );

INVENTORY *inventory_new(		char *inventory_name );

INVENTORY_PURCHASE *inventory_purchase_new(
					void );

INVENTORY *inventory_load_new(		char *application_name,
					char *inventory_name );

void inventory_load(			char **inventory_account_name,
					char **cost_of_goods_sold_account_name,
					double *retail_price,
					int *reorder_quantity,
					int *quantity_on_hand,
					double *average_unit_cost,
					double *total_cost_balance,
					char *application_name,
					char *inventory_name );

FILE *inventory_sale_get_update_pipe(
					char *application_name );

FILE *inventory_purchase_get_update_pipe(
					char *application_name );

void inventory_purchase_list_update(
				char *application_name,
				LIST *inventory_purchase_list );

LIST *inventory_purchase_entity_get_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *inventory_name,
					char *purchase_date_time );

LIST *inventory_purchase_date_get_list(
					char *application_name,
					char *inventory_name,
					char *purchase_date_time );

LIST *inventory_purchase_arrived_date_get_list(
					char *application_name,
					char *inventory_name,
					char *earliest_arrived_date_time,
					char *latest_arrived_date_time );

LIST *inventory_sale_completed_date_get_list(
					char *application_name,
					char *inventory_name,
					char *earliest_completed_date_time,
					char *latest_completed_date_time );

void inventory_purchase_reset_quantity_on_hand(
					LIST *purchase_list );

INVENTORY_SALE *inventory_sale_new(
					void );

LIST *inventory_sale_get_list(		char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time,
					char *inventory_name );

char *inventory_get_earliest_quantity_on_hand_purchase_date_time(
				char *application_name,
				char *inventory_name );

void inventory_sale_list_update(
				LIST *inventory_sale_list,
				char *application_name );

void inventory_update_quantity_on_hand_CGS_on_hand_variables(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *sale_date_time,
				char *inventory_name,
				char *state,
				enum inventory_cost_method );

void inventory_purchase_fifo_decrement_quantity_on_hand(
				LIST *purchase_list,
				int quantity );

double inventory_get_lifo_cost_of_goods_sold(
				LIST *purchase_list,
				int quantity );

void inventory_purchase_lifo_decrement_quantity_on_hand(
				LIST *purchase_list,
				int quantity );

char *inventory_sale_get_latest_date_time(
				char *application_name,
				char *inventory_name );

char *inventory_purchase_get_latest_date_time(
				char *application_name,
				char *inventory_name );

char *inventory_purchase_list_display(
				LIST *purchase_list );

char *inventory_sale_list_display(
				LIST *sale_list );

char *inventory_sale_get_latest_date_time(
				char *application_name,
				char *inventory_name );

INVENTORY_SALE *inventory_sale_update_quantity_on_hand_CGS_latest(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				enum inventory_cost_method );

boolean inventory_exists_latest_zero_cost_of_goods_sold(
				char *application_name,
				char *inventory_name,
				char *latest_inventory_sale_date_time );

void inventory_update_quantity_on_hand_CGS_last_inventory_balance(
				char *application_name,
				char *inventory_name,
				enum inventory_cost_method );

LIST *inventory_sort_inventory_balance_list(
				LIST *inventory_purchase_list,
				LIST *inventory_sale_list );

LIST *inventory_get_average_cost_inventory_balance_list(
				LIST *inventory_purchase_list,
				LIST *inventory_sale_list );

LIST *inventory_get_fifo_inventory_balance_list(
				LIST *inventory_purchase_list,
				LIST *inventory_sale_list );

INVENTORY_BALANCE *inventory_balance_new(
				void );

INVENTORY *inventory_list_seek(
				LIST *inventory_list,
				char *inventory_name );

INVENTORY_PURCHASE *inventory_purchase_list_seek(
				LIST *inventory_purchase_list,
				char *inventory_name );

INVENTORY_SALE *inventory_sale_list_seek(
				LIST *inventory_sale_list,
				char *inventory_name );

char *inventory_sale_latest_date_time(
				char *application_name,
				char *inventory_name,
				char *before_sale_date_time );

void inventory_list_update(
				LIST *inventory_list,
				char *application_name );

void inventory_list_cost_of_goods_sold_update(
				LIST *inventory_list,
				char *application_name );

char *inventory_get_latest_zero_quantity_on_hand_arrived_date_time(
				char *application_name,
				char *inventory_name );

char *inventory_get_non_zero_quantity_on_hand_arrived_date_time(
				char *application_name,
				char *inventory_name,
				char *function );

char *inventory_sale_get_prior_purchase_date_time(
				char *application_name,
				char *inventory_name,
				char *completed_date_time );

char *inventory_purchase_get_prior_purchase_date_time(
				char *application_name,
				char *inventory_name,
				char *purchase_date_time );

void inventory_last_inventory_balance_update(
				int quantity_on_hand,
				double average_unit_cost,
				double total_cost_balance,
				char *inventory_name,
				char *application_name );

void inventory_purchase_list_reset_quantity_on_hand(
				LIST *inventory_purchase_list );

void inventory_set_average_inventory_balance_list(
				LIST *inventory_list,
				char *inventory_name,
				char *application_name );

void inventory_set_average_inventory_balance(
				INVENTORY *inventory,
				char *application_name );

int inventory_get_sale_quantity(
				char *application_name,
				char *inventory_name,
				char *sale_full_name,
				char *sale_street_address,
				char *sale_date_time );

INVENTORY_PURCHASE *inventory_get_prior_inventory_purchase(
				char *application_name,
				char *inventory_name,
				char *purchase_date_time );

char *inventory_get_inventory_sale_join_where(
				void );

char *inventory_get_inventory_purchase_join_where(
				void );

INVENTORY_SALE *inventory_sale_fetch(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name );

INVENTORY_PURCHASE *inventory_purchase_fetch(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name );

void inventory_sale_pipe_update(
				FILE *update_pipe,
				char *inventory_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				double extension,
				double database_extension,
				double cost_of_goods_sold,
				double database_cost_of_goods_sold );

void inventory_purchase_pipe_update(
				FILE *update_pipe,
				char *inventory_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				int arrived_quantity,
				int database_arrived_quantity,
				int quantity_on_hand,
				int database_quantity_on_hand,
				double extension,
				double database_extension,
				double capitalized_unit_cost,
				double database_capitalized_unit_cost,
				double average_unit_cost,
				double database_average_unit_cost );

void inventory_purchase_update(
				char *application_name,
				char *inventory_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				int arrived_quantity,
				int database_arrived_quantity,
				int quantity_on_hand,
				int database_quantity_on_hand,
				double extension,
				double database_extension,
				double capitalized_unit_cost,
				double database_capitalized_unit_cost,
				double average_unit_cost,
				double database_average_unit_cost );

char *inventory_get_max_arrived_purchase_date_time(
				char *application_name,
				char *inventory_name );

INVENTORY_PURCHASE *inventory_get_last_inventory_purchase(
				char *application_name,
				char *inventory_name );

char *inventory_get_max_sale_date_time(
				char *application_name,
				char *inventory_name );

INVENTORY_SALE *inventory_get_last_inventory_sale(
				char *application_name,
				char *inventory_name );

char *inventory_get_max_arrived_date_time(
				char *application_name,
				char *inventory_name );

char *inventory_get_max_completed_date_time(
				char *application_name,
				char *inventory_name );

boolean inventory_is_latest_purchase(
				char *last_inventory_purchase_date_time,
				INVENTORY_SALE *last_inventory_sale,
				char *last_inventory_arrived_date_time,
				char *purchase_date_time );

boolean inventory_is_latest_sale(
				char *last_inventory_sale_date_time,
				char *last_inventory_completed_date_time,
				char *last_inventory_arrived_date_time,
				char *sale_date_time );

void inventory_sale_list_cost_of_goods_sold_update(
				char *application_name,
				LIST *inventory_sale_list,
				char *inventory_name );

void inventory_sale_update(	char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				double extension,
				double database_extension,
				double cost_of_goods_sold,
				double database_cost_of_goods_sold );

double inventory_purchase_get_total_cost_balance(
				int *quantity_on_hand,
				double *average_unit_cost,
				double total_cost_balance,
				double capitalized_unit_cost,
				int ordered_minus_returned_quantity );

double inventory_sale_get_average_cost_of_goods_sold(
				double *total_cost_balance,
				int *quantity_on_hand,
				double average_unit_cost,
				int quantity_sold );

void inventory_sale_list_extension_update(
				char *application_name,
				LIST *sale_list,
				char *inventory_name );

void inventory_sale_list_delete(
				LIST *inventory_sale_list,
				char *sale_date_time,
				char *inventory_name );

void inventory_purchase_list_delete(
				LIST *inventory_purchase_list,
				char *purchase_date_time,
				char *inventory_name );

char *inventory_get_subquery(	char *inventory_name,
				char *one2m_folder_name,
				char *mto1_folder_name,
				char *foreign_attribute_name );

double inventory_decrement_quantity_on_hand_get_CGS_fifo(
				LIST *inventory_purchase_list,
				int quantity );

double inventory_decrement_quantity_on_hand_get_CGS_lifo(
				LIST *inventory_purchase_list,
				int quantity,
				char *completed_date_time );

void inventory_set_quantity_on_hand_CGS_fifo(
				LIST *inventory_sale_list,
				LIST *inventory_purchase_list );

void inventory_set_quantity_on_hand_CGS_lifo(
				LIST *inventory_sale_list,
				LIST *inventory_purchase_list );

void inventory_list_delete(	LIST *inventory_list,
				char *inventory_name );

char *inventory_purchase_get_hash_table_key(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name );

char *inventory_purchase_get_select(
				void );

void inventory_purchase_parse(
				char **full_name,
				char **street_address,
				char **purchase_date_time,
				char **inventory_name,
				int *ordered_quantity,
				double *unit_cost,
				double *extension,
				double *database_extension,
				double *capitalized_unit_cost,
				double *database_capitalized_unit_cost,
				int *arrived_quantity,
				int *database_arrived_quantity,
				int *missing_quantity,
				int *quantity_on_hand,
				int *database_quantity_on_hand,
				double *average_unit_cost,
				double *database_average_unit_cost,
				char **arrived_date_time,
				char *input_buffer );

HASH_TABLE *inventory_get_arrived_inventory_purchase_hash_table(
				LIST **inventory_purchase_list,
				LIST **purchase_inventory_name_list,
				char *application_name,
				char *inventory_name );

char *inventory_sale_get_select(
				void );

void inventory_sale_parse(
				char **full_name,
				char **street_address,
				char **sale_date_time,
				char **inventory_name,
				int *sold_quantity,
				double *retail_price,
				double *discount_amount,
				double *extension,
				double *database_extension,
				double *cost_of_goods_sold,
				double *database_cost_of_goods_sold,
				char **completed_date_time,
				char **inventory_account_name,
				char **cost_of_goods_sold_account_name,
				char *input_buffer );

HASH_TABLE *inventory_get_completed_inventory_sale_hash_table(
				LIST **inventory_sale_list,
				LIST **inventory_sale_name_list,
				char *application_name,
				char *inventory_name );

char *inventory_sale_get_hash_table_key(
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name );

LIST *inventory_get_inventory_purchase_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

LIST *inventory_get_specific_inventory_purchase_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

LIST *inventory_get_inventory_sale_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

double inventory_purchase_list_set_extension(
				LIST *inventory_purchase_list );

void inventory_sale_list_set_extension(
				LIST *inventory_sale_list );

double inventory_sale_get_extension(
				double retail_price,
				int quantity,
				double discount_amount );

INVENTORY_PURCHASE *inventory_get_inventory_purchase(
				LIST *inventory_purchase_list,
				char *inventory_name );

void inventory_purchase_arrived_quantity_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name,
				int arrived_quantity,
				int database_arrived_quantity );

void inventory_purchase_layers_propagate(
				char *application_name,
				LIST *inventory_purchase_list,
				boolean force_not_latest );

void inventory_purchase_arrived_quantity_update_with_propagate(
				LIST *inventory_purchase_list,
				char *application_name,
				boolean force_not_latest );

INVENTORY *inventory_sale_set_cost_of_goods_sold(
				INVENTORY_SALE *inventory_sale,
				char *application_name,
				char *inventory_name,
				enum inventory_cost_method );

double inventory_sale_get_cost_of_goods_sold(
				LIST *inventory_purchase_list,
				double *total_cost_balance,
				int *quantity_on_hand,
				double average_unit_cost,
				int sale_quantity,
				enum inventory_cost_method,
				char *completed_date_time );

double inventory_purchase_get_extension(
				int ordered_quantity,
				double unit_cost );

int inventory_purchase_get_quantity_on_hand(
				int arrived_quantity,
				int missing_quantity,
				int returned_quantity );

void inventory_set_quantity_on_hand_fifo(
				LIST *inventory_sale_list,
				LIST *inventory_purchase_list );

void inventory_purchase_list_set_capitalized_unit_cost(
				LIST *inventory_purchase_list,
				double sum_inventory_extension,
				double sales_tax,
				double freight_in );

char *inventory_balance_list_display(
				LIST *inventory_balance_list );

void inventory_balance_list_average_table_display(
				FILE *output_pipe,
				LIST *inventory_balance_list );

/* Use this for fifo and lifo */
/* -------------------------- */
void inventory_balance_list_table_display(
				FILE *output_pipe,
				LIST *inventory_balance_list );

LIST *inventory_get_latest_inventory_purchase_list(
				char *application_name,
				char *inventory_name );

INVENTORY_COST_ACCOUNT *inventory_cost_account_new(
				char *account_name );

INVENTORY_COST_ACCOUNT *inventory_get_or_set_cost_account(
				LIST *inventory_cost_account_list,
				char *account_name );

char *inventory_get_inventory_account_name(
				char *application_name,
				char *inventory_name );

void inventory_folder_table_display(
				FILE *output_pipe,
				char *application_name,
				char *inventory_name,
				char *heading );

char *inventory_get_where(	char *inventory_name );

int inventory_purchase_get_returned_quantity(
				LIST *inventory_purchase_return_list );

int inventory_purchase_get_quantity_minus_returned(
				int quantity,
				LIST *inventory_purchase_return_list );

int inventory_sale_get_returned_quantity(
				LIST *inventory_sale_return_list );

int inventory_sale_get_quantity_minus_returned(
				int quantity,
				LIST *inventory_sale_return_list );

/* ---------------------------------------------------- */
/* Sets inventory_purchase.quantity_on_hand,		*/
/*      inventory_purchase.layer_consumed_quantity,	*/
/*      inventory_sale.layer_inventory_purchase_list,	*/
/*      inventory_sale.cost_of_goods_sold.		*/
/* ---------------------------------------------------- */
void inventory_set_fifo_layer_inventory_purchase_list(
				LIST *inventory_purchase_list,
				LIST *inventory_sale_list );

/* ---------------------------------------------------- */
/* Sets inventory_purchase.quantity_on_hand,		*/
/*      inventory_purchase.layer_consumed_quantity,	*/
/*      inventory_sale.layer_inventory_purchase_list,	*/
/*      inventory_sale.cost_of_goods_sold.		*/
/* ---------------------------------------------------- */
void inventory_set_layer_inventory_purchase_list(
				LIST *inventory_purchase_list,
				LIST *inventory_sale_list,
				boolean is_fifo );

/* ---------------------------------------------------- */
/* Sets inventory_purchase.quantity_on_hand,		*/
/*      inventory_purchase.layer_consumed_quantity	*/
/* ---------------------------------------------------- */
LIST *inventory_get_layer_inventory_purchase_list(
			double *cost_of_goods_sold,
			LIST *inventory_purchase_list,
			int sold_quantity,
			boolean is_fifo );

enum inventory_cost_method inventory_cost_method_resolve(
			char *inventory_cost_method_string );

char *inventory_select(	void );

INVENTORY *inventory_parse(
			char *input );

#endif

