/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/import_predict.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef IMPORT_PREDICT_H
#define IMPORT_PREDICT_H

#include "list.h"
#include "boolean.h"
#include "application.h"
#include "frameset.h"
#include "entity_self.h"
#include "transaction.h"
#include "export_table.h"
#include "export_process.h"

#define IMPORT_PREDICT_SQL			"home_edition.sql"
#define IMPORT_PREDICT_SQLGZ			"home_edition.sql.gz"
#define IMPORT_PREDICT_PROCESS_NAME		"import_predictbooks"

#define IMPORT_PREDICT_CASH_ACCOUNT		"checking"
#define IMPORT_PREDICT_EQUITY_ACCOUNT		"net_asset"

#define IMPORT_PREDICT_CARD_FEEDER_PHRASE	"Credit Card Bill Payment"
#define IMPORT_PREDICT_CASH_FEEDER_PHRASE	"PAYMENT - THANK YOU"

#define IMPORT_PREDICT_TRIAL_BALANCE_EXECUTABLE		\
						"trial_balance_output"

#define IMPORT_PREDICT_FINANCIAL_POSITION_EXECUTABLE	\
						"financial_position"

#define IMPORT_PREDICT_DATE_MISSING_MESSAGE	\
	"<p style=\"margin: 3\%;\">"		\
	"Please enter a valid begin date."

#define IMPORT_PREDICT_BALANCE_ZERO_MESSAGE	\
	"<p style=\"margin: 3\%;\">"		\
	"Please enter a begin balance."

#define IMPORT_PREDICT_BANK_MISSING_MESSAGE	\
	"<p style=\"margin: 3\%;\">"		\
	"Please either select a financial "	\
	"institution or enter in a valid "	\
	"bank name."

#define IMPORT_PREDICT_EXISTS_MESSAGE					  \
	"<p style=\"margin: 3\%;\">"					  \
	"Warning: PredictBooks is already installed and will be replaced."

#define IMPORT_PREDICT_ENTITY_SELF_MESSAGE				  \
	"<p style=\"margin: 3\%;\">"					  \
	"ERROR: your name must be inserted into the table Self. "	  \
	"The Supervisor role path is Insert --> Entity --> Self"

#define IMPORT_PREDICT_ONCE_MESSAGE					  \
	"<p style=\"margin: 3\%;\">"					  \
	"This process should only be executed again "			  \
	"if you want to delete PredictBooks and start over."

#define IMPORT_PREDICT_RECENT_MESSAGE					  \
	"<p style=\"margin: 3\%;\">"					  \
	"Warning: the Checking Begin Date is recent. Your bank should "	  \
	"store 18 months of transactions."

#define IMPORT_PREDICT_OPENING_MESSAGE 					  \
	"<p style=\"margin: 3\%;\">"					  \
	"PredictBooks implements "					  \
	"<a href=https://en.wikipedia.org/wiki/Double-entry_bookkeeping " \
	"target=_new>double-entry bookkeeping</a>. Each transaction is "  \
	"timestamped to the second. This opening entry is timestamped "   \
	"to midnight. No two transactions can share the "   		  \
	"same second. Here is your opening entry transaction:"

#define IMPORT_PREDICT_TEMPLATE_MESSAGE					\
	"<p style=\"margin: 3\%;\">"					\
	"Warning: You are currently connected to the template "		\
	"database. The template database is meant to spawn your "	\
	"multiple databases. Whereas you can import PredictBooks "	\
	"here, we recommend that you first execute the Alter process "	\
	"called Create Empty Application. This process is available "	\
	"to the System role."

#define IMPORT_PREDICT_SHORTCUT_MESSAGE 				\
	"<p style=\"margin: 3\%;\">"					\
	"If you are not yet familiar with double-entry bookkeeping, "	\
	"the skill will develop over time. PredictBooks has "		\
	"available a shortcut process called Insert Cash Expense "	\
	"Transaction. This process will prompt you for the necessary "	\
	"information and generate the correct double-entry "		\
	"transaction.\n"						\
	"<p style=\"margin: 3\%;\">"					\
	"However, you will only have to insert a small number of "	\
	"transactions because most of them will be generated "		\
	"automatically. After you install PredictBooks, become "	\
	"familiar with the Feeder Phrase table. Each of your bank's "	\
	"transactions is described with a phrase. Insert these "	\
	"phrases into the Feeder Phrase table. PredictBooks will "	\
	"seek out the phrases to generate your transactions "		\
	"automatically."

#define IMPORT_PREDICT_REFRESH_MESSAGE 					\
	"<p style=\"margin: 3\%;\">"					\
	"This process changed the database that is not yet reflected "	\
	"in the menu above. To update the menu above, click this "	\
	"link: "

#define IMPORT_PREDICT_TRIAL_BALANCE_MESSAGE				\
	"<p style=\"margin: 3\%;\">"					\
	"The Trial Balance is the first financial statement to "	\
	"execute.\n"							\
	"<br />Journal entries with a <i>debit</i> amount "		\
	"(the left column) usually <i>increase</i> your Checking "	\
	"balance. The corresponding <i>credit</i> amount (the right "	\
        "column) will probably be a revenue (income) account.\n"	\
	"<br />Journal entries with a <i>credit</i> amount (the right "	\
	"column) usually <i>decrease</i> your Checking balance. The "	\
	"corresponding <i>debit</i> amount (the left column) will "	\
	"probably be an expense account.\n"				\
	"<br />Bookkeeping rule #1: Total Debit = Total Credit"

#define IMPORT_PREDICT_FINANCIAL_POSITION_MESSAGE			\
	"<p style=\"margin: 3\%;\">"					\
	"The Financial Position is the third financial statement to "	\
	"execute. <br />Bookkeeping rule #2: "				\
	"Asset Element = Liability Plus Equity\n"

#define IMPORT_PREDICT_NEXT_STEP_MESSAGE				\
	"<p style=\"margin: 3\%;\">"					\
	"The second financial statement to execute is the Statement "	\
	"of Activities. It is omitted here because it would have been "	\
	"blank. Activities are mainly revenues (income) and expenses.\n"\
	"<br />Notice above the label \"Change In Net Assets\".\n"	\
	"The amount reported is currently 0.00.\n"			\
	"<br />The next step is to:\n"					\
	"<ol>\n"							\
	"<li>Download all of your checking transactions from your "	\
	"bank.\n"							\
	"<li>Execute: Reconcile --> yourBank Checking Upload.\n"	\
	"</ol>\n"

#define IMPORT_PREDICT_MAP_PROCESS_NAME \
	"generic_checking_upload"

#define IMPORT_PREDICT_INPUT_DAYS_AGO 180

#define IMPORT_PREDICT_PASSTHRU_ACCOUNT	\
	"credit_card_passthru"

typedef struct
{
	char *card_insert_statement;
	char *cash_insert_statement;
	LIST *feeder_phrase_insert_statement_list;
} IMPORT_PREDICT_PASSTHRU;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
IMPORT_PREDICT_PASSTHRU *import_predict_passthru_new(
		char *entity_self_full_name,
		char *entity_self_street_address );

/* Process */
/* ------- */
IMPORT_PREDICT_PASSTHRU *import_pedict_passthru_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *import_predict_passthru_insert_statement(
		const char *feeder_phrase_table,
		const char *import_predict_passthru_account,
		const char *feeder_phrase,
		char *entity_self_full_name,
		char *entity_self_street_address );

typedef struct
{
	LIST *import_predict_map_list;
	char *import_predict_map_process_name;
	char *insert_system_string;
} IMPORT_PREDICT_PROCESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
IMPORT_PREDICT_PROCESS *import_predict_process_new(
		const char *role_process_table,
		const char *role_supervisor,
		char *import_predict_bank_name );

/* Process */
/* ------- */
IMPORT_PREDICT_PROCESS *import_pedict_process_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *import_predict_process_insert_system_string(
		const char *role_process_table,
		const char *role_supervisor,
		char *import_predict_map_process_name );

typedef struct
{
	char *financial_institution_full_name;
	char *upload_process_name;
} IMPORT_PREDICT_MAP;

/* Usage */
/* ----- */
LIST *import_predict_map_list(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
IMPORT_PREDICT_MAP *import_predict_map_new(
		char *financial_institution_full_name,
		char *upload_process_name );

/* Process */
/* ------- */
IMPORT_PREDICT_MAP *import_pedict_map_calloc(
		void );

/* Usage */
/* ----- */

/* Returns component of import_predict_map_list */
/* -------------------------------------------- */
char *import_predict_map_process_name(
		char *import_predict_bank_name,
		LIST *import_predict_map_list );

/* Usage */
/* ----- */
IMPORT_PREDICT_MAP *import_predict_map_seek(
		LIST *import_predict_map_list,
		char *import_predict_bank_name );

typedef struct
{
	char *cash_account_name;
	char *equity_account_name;
	char *date_time;
	TRANSACTION *transaction;
} IMPORT_PREDICT_TRANSACTION;

/* Usage */
/* ----- */
IMPORT_PREDICT_TRANSACTION *
	import_predict_transaction_new(
		const char *transaction_begin_time,
		char *checking_begin_date,
		double checking_begin_balance,
		char *entity_self_full_name,
		char *entity_self_street_address );

/* Process */
/* ------- */
IMPORT_PREDICT_TRANSACTION *
	import_predict_transaction_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *import_predict_transaction_date_time(
		const char *transaction_begin_time,
		char *checking_begin_date );

/* Usage */
/* ----- */
LIST *import_predict_transaction_journal_list(
		const char *import_predict_cash_account,
		const char *import_predict_equity_account,
		double checking_begin_balance,
		boolean fetch_account_boolean );

typedef struct
{
	boolean template_boolean;
	boolean exists_boolean;
	char *bank_name;
	boolean bank_missing_boolean;
	boolean date_missing_boolean;
	boolean date_recent_boolean;
	boolean balance_zero_boolean;
	ENTITY_SELF *entity_self;
	char *filespecification;
	char *appaserver_error_filename;
	boolean application_menu_horizontal_boolean;
} IMPORT_PREDICT_INPUT;

/* Safely returns */
/* -------------- */
IMPORT_PREDICT_INPUT *import_predict_input_new(
		const char *import_predict_sqlgz,
		char *application_name,
		char *financial_institution_full_name,
		char *name_of_bank,
		char *checking_begin_date,
		double checking_begin_balance,
		char *mount_point );

/* Process */
/* ------- */
IMPORT_PREDICT_INPUT *import_predict_input_calloc(
		void );

boolean import_predict_input_template_boolean(
		const char *application_create_template_name,
		char *application_name );

/* Returns either parameter or null */
/* -------------------------------- */
char *import_predict_input_bank_name(
		char *financial_institution_full_name,
		char *name_of_bank );

boolean import_predict_input_bank_missing_boolean(
		const char *security_forbid_character_string,
		char *import_predict_input_bank_name );

boolean import_predict_input_date_missing_boolean(
		const char *security_forbid_character_string,
		char *checking_begin_date );

boolean import_predict_input_date_recent_boolean(
		const int import_predict_input_days_ago,
		char *checking_begin_date,
		char *date_now_yyyy_mm_dd );

boolean import_predict_input_balance_zero_boolean(
		double checking_begin_balance );

/* Returns heap memory */
/* ------------------- */
char *import_predict_input_filespecification(
		const char *import_predict_sqlgz,
		char *mount_point );

/* Usage */
/* ----- */
boolean import_predict_input_exists_boolean(
		const char *transaction_table );

typedef struct
{
	IMPORT_PREDICT_INPUT *import_predict_input;
	IMPORT_PREDICT_PROCESS *import_predict_process;
	char *system_string;
	char *delete_role_system_string;
	char *entity_system_string;
	IMPORT_PREDICT_TRANSACTION *import_predict_transaction;
	IMPORT_PREDICT_PASSTHRU *import_predict_passthru;
	char *menu_href_string;
	char *menu_anchor_tag;
	char *trial_balance_system_string;
	char *financial_position_system_string;
} IMPORT_PREDICT;

/* Safely returns */
/* -------------- */
IMPORT_PREDICT *import_predict_new(
		const char *import_predict_sqlgz,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *financial_institution_full_name,
		char *name_of_bank,
		char *checking_begin_date,
		double checking_begin_balance,
		char *mount_point );

/* Process */
/* ------- */
IMPORT_PREDICT *import_predict_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *import_predict_delete_role_system_string(
		const char *import_predict_process_name,
		const char *role_process_table,
		const char *role_supervisor );

/* Returns static memory or null */
/* ----------------------------- */
char *import_predict_menu_href_string(
		const char *menu_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		boolean application_menu_horizontal_boolean );

/* Returns static memory or null */
/* ----------------------------- */
char *import_predict_menu_anchor_tag(
		const char *frameset_prompt_frame,
		char *import_predict_menu_href_string );

/* Returns static memory */
/* --------------------- */
char *import_predict_trial_balance_system_string(
		const char *import_predict_trial_balance_executable,
		char *session_key,
		char *login_name,
		char *role_name );

/* Returns static memory */
/* --------------------- */
char *import_predict_financial_position_system_string(
		const char *import_predict_financial_position_executable,
		char *session_key,
		char *login_name,
		char *role_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *import_predict_system_string(
		char *import_predict_filename );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *import_predict_entity_system_string(
		const char *entity_table,
		const char *entity_financial_institution_table,
		const char *entity_street_address_unknown,
		char *name_of_bank,
		char *import_predict_bank_name );

#endif
