/* --------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/liability.h	 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef LIABILITY_H
#define LIABILITY_H

#include "list.h"
#include "entity.h"
#include "receivable.h"
#include "transaction.h"
#include "appaserver_link_file.h"

#define LIABILITY_MEMO		"Liability Payment"
#define LIABILITY_PROMPT	"Press here to view check."

typedef struct
{
	char *timlib_in_clause;
	char *liability_entity_where;
	char *liability_account_where;
	LIST *journal_system_list;
	double journal_credit_debit_difference_sum;
} LIABILITY;

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
			char *timlib_in_clause );

/* Usage */
/* ----- */
LIABILITY *liability_account_fetch(
			char *liability_account_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *liability_account_where(
			char *liability_account_name );

/* Private */
/* ------- */
LIABILITY *liability_calloc(
			void );

#define LIABILITY_ACCOUNT_ENTITY_TABLE		"liability_account_entity"

#define LIABILITY_ACCOUNT_ENTITY_SELECT		"account,"	\
						"full_name,"	\
						"street_address"

typedef struct
{
	char *account_name;
	ENTITY *entity;
} LIABILITY_ACCOUNT_ENTITY;

/* Usage */
/* ----- */
LIST *liability_account_entity_list(
			void );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *liability_account_entity_system_string(
			char *liability_account_entity_select,
			char *liability_account_entity_table );

LIST *liability_account_entity_system_list(
			char *liability_account_entity_system_string );

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_parse(
			char *string_input );

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_new(
			char *account_name );

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_calloc(
			void );

/* Public */
/* ------ */
LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek_account(
			char *account_name,
			LIST *liability_account_entity_list );

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek_entity(
			char *full_name,
			char *street_address,
			LIST *liability_account_entity_list );


typedef struct
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;
	LIABILITY *liability;
	RECEIVABLE *receivable;
	double amount_due;
} LIABILITY_ENTITY;

/* Usage */
/* ----- */
LIABILITY_ENTITY *liability_entity_new(
			double dialog_box_payment_amount,
			LIST *liability_account_entity_list,
			LIST *account_current_liability_name_list,
			LIST *account_receivable_name_list,
			ENTITY *entity );

/* Process */
/* ------- */
LIABILITY_ENTITY *liability_entity_calloc(
			void );

double liability_entity_amount_due(
			double dialog_box_payment_amount,
			LIABILITY *liability,
			RECEIVABLE *receivable );

typedef struct
{
	char *output_string;
} LIABILITY_CHECK;

/* Usage */
/* ----- */
LIABILITY_CHECK *liability_check_new(
			int check_number,
			LIABILITY_ENTITY *liability_entity );

/* Process */
/* ------- */
LIABILITY_CHECK *liability_check_calloc(
			void );

typedef struct
{
	LIST *check_list;
	APPASERVER_LINK_FILE *appaserver_link_file;
	char *output_filename;
	char *pdf_output_filename;
	char *ftp_filename;
	char *document_root_filename;
	FILE *output_file;
} LIABILITY_CHECK_LIST;

/* Usage */
/* ----- */
LIABILITY_CHECK_LIST *liability_check_list_new(
			char *application_name,
			int starting_check_number,
			char *document_root_directory,
			char *process_name,
			char *session_key,
			LIST *liability_entity_list );

/* Process */
/* ------- */
LIABILITY_CHECK_LIST *liability_check_list_calloc(
			void );


typedef struct
{
	TRANSACTION *transaction;
} LIABILITY_TRANSACTION;

/* Usage */
/* ----- */
LIABILITY_TRANSACTION *liability_transaction_new(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double payment_amount,
			char *liability_debit_account_name,
			char *liability_credit_account_name,
			char *liability_memo,
			int check_number );

/* Process */
/* ------- */
LIABILITY_TRANSACTION *liability_transaction_calloc(
			void );

typedef struct
{
	LIST *liability_account_entity_list;
	LIST *account_current_liability_name_list;
	LIST *account_receivable_name_list;
	LIST *liability_entity_list;
	LIABILITY_CHECK_LIST *liability_check_list;
	LIST *liability_transaction_list;
} LIABILITY_PAYMENT;

/* Usage */
/* ----- */
LIABILITY_PAYMENT *liability_payment_new(
			char *application_name,
			double dialog_box_payment_amount,
			int starting_check_number,
			char *document_root_directory,
			char *process_name,
			char *session_key,
			LIST *entity_full_street_list );

/* Process */
/* ------- */
LIABILITY_PAYMENT *liability_payment_calloc(
			void );

#endif
