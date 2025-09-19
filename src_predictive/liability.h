/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/liability.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef LIABILITY_H
#define LIABILITY_H

#include "list.h"
#include "boolean.h"
#include "entity.h"
#include "entity_self.h"
#include "receivable.h"
#include "transaction.h"
#include "check.h"
#include "appaserver_link.h"

#define LIABILITY_ACCOUNT_ENTITY_TABLE		"liability_account_entity"

#define LIABILITY_ACCOUNT_ENTITY_SELECT		"account,"	\
						"full_name,"	\
						"street_address"

#define LIABILITY_TRANSACTION_MEMO	"Liability Payment"

typedef struct
{
	char *account_name;
	double credit_amount;
} LIABILITY_ACCOUNT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIABILITY_ACCOUNT *liability_account_getset(
		LIST *list,
		char *account_name );

/* Usage */
/* ----- */
LIABILITY_ACCOUNT *liability_account_new(
		char *account_name );

/* Process */
/* ------- */
LIABILITY_ACCOUNT *liability_account_calloc(
		void );

typedef struct
{
	LIST *list;
} LIABILITY_ACCOUNT_LIST;

/* Usage */
/* ----- */
LIABILITY_ACCOUNT_LIST *
	liability_account_list_new(
		LIST *journal_system_list );

/* Process */
/* ------- */
LIABILITY_ACCOUNT_LIST *
	liability_account_list_calloc(
		void );

typedef struct
{
	char *account_where;
	LIST *journal_system_list;
	double journal_credit_debit_difference_sum;
	char *string_in_clause;
	char *entity_where;
	LIABILITY_ACCOUNT_LIST *liability_account_list;
	char *journal_list_last_memo;
} LIABILITY;

/* Usage */
/* ----- */
LIABILITY *liability_account_fetch(
		char *liability_account_name );

/* Process */
/* ------- */
LIABILITY *liability_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *liability_account_where(
		char *liability_account_name );

/* Usage */
/* ----- */
LIABILITY *liability_entity_fetch(
		char *full_name,
		char *street_address,
		LIST *account_current_liability_name_list );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *liability_entity_where(
		char *full_name,
		char *street_address,
		char *string_in_clause );

typedef struct
{
	char *account_name;
	ENTITY *entity;
} LIABILITY_ACCOUNT_ENTITY;

/* Usage */
/* ----- */
LIST *liability_account_entity_list(
		const char *liability_account_entity_select,
		const char *liability_account_entity_table );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *liability_account_entity_system_string(
		const char *liability_account_entity_select,
		const char *liability_account_entity_table );

/* Usage */
/* ----- */
LIST *liability_account_entity_system_list(
		char *liability_account_entity_system_string );

/* Usage */
/* ----- */
LIABILITY_ACCOUNT_ENTITY *liability_account_entity_parse(
		char *string_input );

/* Usage */
/* ----- */
LIABILITY_ACCOUNT_ENTITY *liability_account_entity_new(
		char *account_name );

/* Process */
/* ------- */
LIABILITY_ACCOUNT_ENTITY *liability_account_entity_calloc(
		void );

/* Usage */
/* ----- */
LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek(
		char *account_name,
		LIST *liability_account_entity_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *liability_account_entity_account_name_list(
		LIST *liability_account_entity_list );

typedef struct
{
	ENTITY *entity;
	LIABILITY *liability;
	RECEIVABLE *receivable;
	double amount_due;
} LIABILITY_ENTITY;

/* Usage */
/* ----- */
LIST *liability_entity_account_list(
		LIST *liability_account_entity_list );

/* Usage */
/* ----- */
LIST *liability_entity_distinct_entity_list(
		LIST *account_current_liability_name_list,
		LIST *journal_account_distinct_entity_list,
		LIST *account_receivable_name_list,
		ENTITY_SELF *entity_self );

/* Usage */
/* ----- */
LIABILITY_ENTITY *liability_entity_account_name_new(
		char *account_name,
		ENTITY *entity );

/* Usage */
/* ----- */
LIABILITY_ENTITY *liability_entity_account_list_new(
		LIST *account_current_liability_name_list,
		LIST *account_receivable_name_list,
		ENTITY *entity );

/* Process */
/* ------- */
LIABILITY_ENTITY *liability_entity_calloc(
		void );

/* Usage */
/* ----- */
double liability_entity_amount_due(
		LIABILITY *liability,
		RECEIVABLE *receivable );

/* Usage */
/* ----- */
LIABILITY_ENTITY *liability_entity_seek(
		char *full_name,
		char *street_address,
		LIST *liability_entity_list );

/* Driver */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *liability_entity_display(
		LIABILITY_ENTITY *liability_entity );

typedef struct
{
	LIST *journal_list;
	double transaction_amount;
} LIABILITY_JOURNAL_LIST;

/* Usage */
/* ----- */
LIABILITY_JOURNAL_LIST *
	liability_journal_list_new(
		double dialog_box_payment_amount,
		char *liability_payment_credit_account_name,
		LIABILITY_ENTITY *liability_entity );

/* Process */
/* ------- */
LIABILITY_JOURNAL_LIST *liability_journal_list_calloc(
		void );

double liability_journal_list_transaction_amount(
		double dialog_box_payment_amount,
		double liability_entity_amount_due );

typedef struct
{
	LIST *list;
	DATE *transaction_date_time;
} LIABILITY_TRANSACTION_LIST;

/* Usage */
/* ----- */
LIABILITY_TRANSACTION_LIST *
	liability_transaction_list_new(
		double dialog_box_payment_amount,
		int starting_check_number,
		char *transaction_memo,
		LIST *liability_payment_entity_list,
		char *liability_payment_credit_account_name );

/* Process */
/* ------- */
LIABILITY_TRANSACTION_LIST *
	liability_transaction_list_calloc(
		void );

/* Usage */
/* ----- */

/* May reset transaction_date_time */
/* ------------------------------- */
void liability_transaction_list_insert(
		LIABILITY_TRANSACTION_LIST *
			liability_transaction_list );

/* Usage */
/* ----- */
void liability_transaction_list_html_display(
		LIABILITY_TRANSACTION_LIST *
			liability_transaction_list );

/* Usage */
/* ----- */
LIST *liability_transaction_list_extract(
		LIST *list );

typedef struct
{
	LIABILITY_JOURNAL_LIST *liability_journal_list;
	TRANSACTION *transaction;

	/* Set externally */
	/* -------------- */
	/* double journal_transaction_amount; */
} LIABILITY_TRANSACTION;

/* Usage */
/* ----- */
LIABILITY_TRANSACTION *liability_transaction_new(
		double dialog_box_payment_amount,
		int check_number,
		char *transaction_memo,
		char *liability_payment_credit_account_name,
		LIABILITY_ENTITY *liability_entity,
		DATE *transaction_date_time );

/* Process */
/* ------- */
LIABILITY_TRANSACTION *liability_transaction_calloc(
		void );

typedef struct
{
	LIST *liability_account_entity_list;
	LIST *exclude_account_name_list;
	LIST *account_current_liability_name_list;
	LIST *journal_account_distinct_entity_list;
	LIST *account_receivable_name_list;
	ENTITY_SELF *entity_self;
	LIST *liability_entity_list;
} LIABILITY_CALCULATE;

/* Usage */
/* ----- */
LIABILITY_CALCULATE *liability_calculate_new(
		char *application_name );

LIABILITY_CALCULATE *liability_calculate_calloc(
		void );

/* Driver */
/* ------ */
void liability_calculate_stdout(
		LIST *liability_entity_list );

typedef struct
{
	LIABILITY_CALCULATE *liability_calculate;
	LIST *entity_list;
	char *transaction_memo;
	CHECK_LIST *check_list;
	char *credit_account_name;
	LIABILITY_TRANSACTION_LIST *liability_transaction_list;
	char *error_message;
} LIABILITY_PAYMENT;

/* Usage */
/* ----- */
LIABILITY_PAYMENT *liability_payment_new(
		char *application_name,
		char *cash_account_name,
		double dialog_box_payment_amount,
		int starting_check_number,
		char *dialog_box_memo,
		char *appaserver_parameter_data_directory,
		char *process_name,
		char *session_key,
		LIST *entity_full_street_list );

/* Process */
/* ------- */
LIABILITY_PAYMENT *liability_payment_calloc(
		void );

char *liability_payment_credit_account_name(
		int starting_check_number,
		char *cash_account_name,
		char *account_uncleared_checks );

/* Usage */
/* ----- */
LIST *liability_payment_entity_list(
		LIST *entity_full_street_list,
		LIST *liability_entity_list );

/* Usage */
/* ----- */

/* Returns message */
/* --------------- */
char *liability_payment_error_message(
		const char *message );

#endif
