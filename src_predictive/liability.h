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
#include "entity_self.h"
#include "receivable.h"
#include "transaction.h"
#include "appaserver_link_file.h"

#define LIABILITY_PROMPT		"Press here to view the check."

typedef struct
{
	char *account_name;
	double credit_amount;
} LIABILITY_ACCOUNT;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
LIABILITY_ACCOUNT *liability_account_getset(
			LIST *list,
			char *account_name );

/* Process */
/* ------- */
LIABILITY_ACCOUNT *liability_account_new(
			char *account_name );

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
	char *timlib_in_clause;
	char *entity_where;
	LIABILITY_ACCOUNT_LIST *liability_account_list;
} LIABILITY;

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

/* Public */
/* ------ */
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
LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek(
			char *account_name,
			LIST *liability_account_entity_list );

/* Always succeeds */
/* --------------- */
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
LIST *liability_entity_list_account(
			LIST *liability_account_entity_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *liability_entity_list_entity(
			LIST *account_current_liability_name_list,
			LIST *journal_account_distinct_entity_list,
			LIST *account_receivable_name_list,
			ENTITY_SELF *entity_self );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIABILITY_ENTITY *liability_entity_account_name_new(
			char *account_name,
			ENTITY *entity );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIABILITY_ENTITY *liability_entity_account_list_new(
			LIST *account_current_liability_name_list,
			LIST *account_receivable_name_list,
			ENTITY *entity );

/* Process */
/* ------- */

/* Public */
/* ------ */
LIABILITY_ENTITY *liability_entity_calloc(
			void );

double liability_entity_amount_due(
			LIABILITY *liability,
			RECEIVABLE *receivable );

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

/* Process */
/* ------- */

/* Usage */
/* ----- */
void liability_transaction_list_html_display(
			LIABILITY_TRANSACTION_LIST *
				liability_transaction_list );

/* Process */
/* ------- */

/* Public */
/* ------ */
LIST *liability_transaction_list_extract(
			LIST *list );

#define LIABILITY_TRANSACTION_MEMO	"Liability Payment"

typedef struct
{
	LIABILITY_JOURNAL_LIST *liability_journal_list;
	double journal_list_transaction_amount;
	TRANSACTION *transaction;
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

#define LIABILITY_CHECK_DATE_COMMAND	"now.sh pretty | column.e 0"

typedef struct
{
	double amount;
	char *dollar_text;
	char *escape_payable_to;
	char *move_down;
	char *date_display;
	char *amount_due_display;
	char *vendor_name_amount_due_display;
	char *amount_due_stub_display;
	char *dollar_text_display;
	char *memo_display;
	char *number_display;
	char *newpage;
	char *output_string;
} LIABILITY_CHECK;

/* Usage */
/* ----- */
LIABILITY_CHECK *liability_check_new(
			double dialog_box_payment_amount,
			int check_number,
			char *transaction_memo,
			LIABILITY_ENTITY *liability_entity );

/* Process */
/* ------- */
LIABILITY_CHECK *liability_check_calloc(
			void );

double liability_check_amount(
			double dialog_box_payment_amount,
			double liability_entity_amount_due );
			
/* Returns static memory */
/* --------------------- */
char *liability_check_dollar_text(
			double liability_check_amount );

/* Returns static memory */
/* --------------------- */
char *liability_check_escape_payable_to(
			char *full_name );

/* Returns program memory */
/* ---------------------- */
char *liability_check_move_down(
			void );

/* Returns static memory */
/* --------------------- */
char *liability_check_date_display(
			char *check_date );

/* Returns amount_due_display which is static memory */
/* ------------------------------------------------- */
char *liability_check_amount_due_display(
			char *amount_due_display );

/* Returns static memory */
/* --------------------- */
char *liability_check_vendor_name_amount_due_display(
			char *liability_check_escape_payable_to,
			char *liability_check_amount_due_display );

/* Returns static memory */
/* --------------------- */
char *liability_check_amount_due_stub_display(
			char *liability_check_amount_due_display );

/* Returns static memory */
/* --------------------- */
char *liability_check_dollar_text_display(
			char *liability_check_dollar_text );

/* Returns static memory */
/* --------------------- */
char *liability_check_memo_display(
			char *transaction_memo );

/* Returns static memory */
/* --------------------- */
char *liability_check_number_display(
			int check_number );

/* Returns program memory */
/* ---------------------- */
char *liability_check_newpage(
			void );

/* Returns heap memory */
/* ------------------- */
char *liability_check_output_string(
			char *liability_check_move_down,
			char *liability_check_date_display,
			char *liability_check_vendor_name_amount_due_display,
			char *liability_check_amount_due_stub_display,
			char *liability_check_dollar_text_display,
			char *liability_check_memo_display,
			char *liability_check_number_display,
			char *liability_check_newpage );

typedef struct
{
	APPASERVER_LINK_FILE *appaserver_link_file;
	char *output_filename;
	char *pdf_output_filename;
	char *ftp_filename;
	char *document_root_filename;
} LIABILITY_CHECK_APPASERVER_LINK;

/* Usage */
/* ----- */
LIABILITY_CHECK_APPASERVER_LINK *
	liability_check_appaserver_link_new(
			char *application_name,
			char *document_root_directory,
			char *process_name,
			char *session_key );

/* Process */
/* ------- */
LIABILITY_CHECK_APPASERVER_LINK *
	liability_check_appaserver_link_calloc(
			void );

typedef struct
{
	LIST *list;
	char *documentclass;
	char *usepackage;
	char *pagenumbering_gobble;
	char *begin_document;
	char *heading;
	char *end_document;
	LIABILITY_CHECK_APPASERVER_LINK *liability_check_appaserver_link;
	FILE *output_file;
} LIABILITY_CHECK_LIST;

/* Usage */
/* ----- */
LIABILITY_CHECK_LIST *liability_check_list_new(
			char *application_name,
			double dialog_box_payment_amount,
			int starting_check_number,
			char *transaction_memo,
			char *document_root_directory,
			char *process_name,
			char *session_key,
			LIST *liability_payment_entity_list );

/* Process */
/* ------- */
LIABILITY_CHECK_LIST *liability_check_list_calloc(
			void );

/* Returns program memory */
/* ---------------------- */
char *liability_check_list_documentclass(
			void );

/* Returns program memory */
/* ---------------------- */
char *liability_check_list_usepackage(
			void );

/* Returns program memory */
/* ---------------------- */
char *liability_check_list_pagenumbering_gobble(
			void );

/* Returns program memory */
/* ---------------------- */
char *liability_check_list_begin_document(
			void );

/* Returns heap memory */
/* ------------------- */
char *liability_check_list_heading(
			char *liability_check_list_documentclass,
			char *liability_check_list_usepackage,
			char *liability_check_list_pagenumbering_gobble,
			char *liability_check_list_begin_document );

/* Returns program memory */
/* ---------------------- */
char *liability_check_list_end_document(
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
	LIABILITY_CHECK_LIST *liability_check_list;
	char *credit_account_name;
	LIABILITY_TRANSACTION_LIST *liability_transaction_list;
	char *error_message;
} LIABILITY_PAYMENT;

/* Usage */
/* ----- */
LIABILITY_PAYMENT *liability_payment_new(
			char *application_name,
			double dialog_box_payment_amount,
			int starting_check_number,
			char *dialog_box_memo,
			char *document_root_directory,
			char *process_name,
			char *session_key,
			LIST *entity_full_street_list );

/* Process */
/* ------- */
LIABILITY_PAYMENT *liability_payment_calloc(
			void );

char *liability_payment_credit_account_name(
			int starting_check_number,
			char *account_cash,
			char *account_uncleared_checks );

/* Usage */
/* ----- */
LIST *liability_payment_entity_list(
			LIST *entity_full_street_list,
			LIST *liability_entity_list );

/* Process */
/* ------- */

/* Public */
/* ------ */

/* Returns message */
/* --------------- */
char *liability_payment_error_message(
			char *message );

#endif
