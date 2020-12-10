/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/entity.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ENTITY_H
#define ENTITY_H

#include "boolean.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */
enum title_passage_rule{	title_passage_rule_null,
				FOB_shipping,
				FOB_destination };

enum payroll_pay_period{	pay_period_not_set,
				pay_period_weekly,
				pay_period_biweekly,
				pay_period_semimonthly,
				pay_period_monthly };

/* Constants */
/* --------- */
#define ENTITY_TABLE		"entity"

#define ENTITY_INSERT_COLUMNS	"full_name,"		\
				"street_address,"	\
				"email_address"

/* #define REFRESH_COMPLETELY	1 */

#define TITLE_PASSAGE_RULE_SHIPPED_DATE "FOB_shipping"
#define TITLE_PASSAGE_RULE_ARRIVED_DATE "FOB_destination"
#define TITLE_PASSAGE_RULE_NULL		""
#define ENTITY_STREET_ADDRESS_UNKNOWN	"unknown"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *city;
	char *state_code;
	char *zip_code;
	char *phone_number;
	char *email_address;
	LIST *depreciable_fixed_asset_purchase_list;
	double depreciation_amount;

	double dialog_box_payment_amount;
	char *liability_entity_debit_account_name;
	LIST *liability_entity_journal_list;
	double liability_entity_amount_due;
	double liability_entity_payment_amount;
	double liability_entity_loss_amount;
	int check_number;
} ENTITY;

/* Operations */
/* ---------- */
ENTITY *entity_calloc(			void );

ENTITY *entity_new(			char *full_name,
					char *street_address );

enum title_passage_rule
	entity_get_title_passage_rule_resolve(
			char *title_passage_rule_string );

char *entity_get_title_passage_rule_string(
			enum title_passage_rule title_passage_rule );

ENTITY *entity_sales_tax_payable_entity(
			void );

char *entity_payroll_pay_period_string(
				enum payroll_pay_period );

enum payroll_pay_period entity_payroll_pay_period_resolve(
				char *payroll_pay_period_string );

ENTITY *entity_getset(	LIST *entity_list,
			char *full_name,
			char *street_address,
			boolean with_strdup );

ENTITY *entity_seek(	char *full_name,
			char *street_address,
			LIST *entity_list );

char *entity_list_display(
			LIST *entity_list );

boolean entity_list_exists(
			char *full_name,
			char *street_address,
			LIST *entity_list );

ENTITY *entity_parse(	char *input );

ENTITY *entity_fetch(	char *full_name,
			char *street_address );

/* ---------------------- */
/* Returns program memory */
/* ---------------------- */
char *entity_select(	void );

/* Safely returns heap memory */
/* -------------------------- */
char *entity_primary_where(
			char *full_name,
			char *street_address );

/* Returns static memory */
/* --------------------- */
char *entity_escape_full_name(
			char *full_name );
char *entity_escape_name(
			char *full_name );

/* Returns heap memory or null */
/* --------------------------- */
char *entity_street_address(
			char *full_name );

FILE *entity_insert_open(
			char *error_filename );

void entity_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *email_address );

char *entity_name_display(
			char *full_name,
			char *street_address );

LIST *entity_system_list(
			char *sys_string );

char *entity_sys_string(
			char *where );

LIST *entity_full_street_list(
			LIST *full_name_list,
			LIST *street_address_list );

ENTITY *entity_full_name_seek(
			char *full_name,
			LIST *entity_list );

ENTITY *entity_full_name_entity(
			char *full_name );

#endif

