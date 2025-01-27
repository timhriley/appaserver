/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/entity.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ENTITY_H
#define ENTITY_H

#include "boolean.h"

enum payroll_pay_period{	pay_period_not_set,
				pay_period_weekly,
				pay_period_biweekly,
				pay_period_semimonthly,
				pay_period_monthly };

#define ENTITY_TABLE		"entity"

#define ENTITY_SELECT		"full_name,"		\
				"street_address,"	\
				"city,"			\
				"state_code,"		\
				"zip_code,"		\
				"phone_number,"		\
				"email_address"

#define ENTITY_INSERT_COLUMNS	"full_name,"		\
				"street_address,"	\
				"email_address"

/* #define REFRESH_COMPLETELY	1 */

#define ENTITY_STREET_ADDRESS_UNKNOWN	"unknown"

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
	LIST *balance_zero_account_list;
	LIST *entity_liability_due_account_list;
	double liability_amount_due;
	double liability_payment_amount;
	double liability_additional_payment_amount;
	int check_number;
} ENTITY;

/* Usage */
/* ----- */
LIST *entity_system_list(
			char *system_string );

/* Process */
/* ------- */
FILE *entity_input_pipe(
			char *system_string );

/* Usage */
/* ----- */
ENTITY *entity_fetch(	char *full_name,
			char *street_address );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *entity_primary_where(
			char *full_name,
			char *street_address );

/* Returns heap memory */
/* ------------------- */
char *entity_system_string(
			char *entity_select,
			char *entity_table,
			char *entity_primary_where );

ENTITY *entity_parse(	char *input );


ENTITY *entity_new(	char *full_name,
			char *street_address );

ENTITY *entity_calloc(	void );

/* Public */
/* ------ */
LIST *entity_full_street_list(
			LIST *full_name_list,
			LIST *street_address_list );

/* Returns static memory */
/* --------------------- */
char *entity_escape_full_name(
			char *full_name );
char *entity_escape_name(
			char *full_name );
char *entity_name_escape(
			char *full_name );

/* Returns heap memory or null */
/* --------------------------- */
char *entity_street_address(
			char *full_name );

ENTITY *entity_full_name_seek(
			char *full_name,
			LIST *entity_list );

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

ENTITY *entity_full_name_entity(
			char *full_name );

#endif

