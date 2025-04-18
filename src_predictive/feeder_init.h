/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_init.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_INIT_H
#define FEEDER_INIT_H

#include "list.h"
#include "boolean.h"
#include "entity_self.h"
#include "journal.h"
#include "transaction.h"

#define FEEDER_INIT_CASH_FEEDER_PHRASE	\
	"card bill payment|credit card|card online payment"

#define FEEDER_INIT_CARD_FEEDER_PHRASE	\
	"payment - thank you|payment, thank you"

#define FEEDER_INIT_INPUT_DAYS_AGO	180

#define FEEDER_INIT_TRIAL_EXECUTABLE	\
					"trial_balance_output"

#define FEEDER_INIT_ACTIVITY_EXECUTABLE	\
					"statement_of_activities"

#define FEEDER_INIT_POSITION_EXECUTABLE	\
					"financial_position"

#define FEEDER_INIT_INSTITUTION_MISSING_MESSAGE	\
	"<h3>Please select a financial institution.</h3>"

#define FEEDER_INIT_ACCOUNT_EXIST_MESSAGE				\
	"<p style=\"margin: 3\%;\">"					\
	"ERROR: This feeder account already exists. "			\
	"See Lookup --> Feeder --> Feeder Account"

#define FEEDER_INIT_ENTITY_SELF_MESSAGE				  	\
	"<p style=\"margin: 3\%;\">"					\
	"ERROR: your name must be inserted into the table Self. "	\
	"See Insert --> Entity --> Self"

#define FEEDER_INIT_RECENT_MESSAGE					\
	"<p style=\"margin: 3\%;\">"					\
	"Warning: the earliest transaction is recent. "			\
	"Your financial institution "  					\
	"should store 18 months of transactions."

#define FEEDER_INIT_PASSTHRU_EXIST_MESSAGE				\
	"<p style=\"margin: 3\%;\">"					\
	"Warning: the passthru account's feeder phrase already "	\
	"exists. "  							\
	"You will need to adjust the passthru feeder phrases manually."

#define FEEDER_INIT_OPENING_MESSAGE 					  \
	"<h3>Opening message</h3>\n"					  \
	"<p style=\"margin: 3\%;\">"					  \
	"PredictBooks implements "					  \
	"<a href=https://en.wikipedia.org/wiki/Double-entry_bookkeeping " \
	"target=_new>double-entry bookkeeping</a>. Each transaction is "  \
	"timestamped to the second. This opening entry is timestamped "   \
	"to midnight. No two transactions can share the "   		  \
	"same second. Here is your opening entry transaction:"

#define FEEDER_INIT_TRIAL_BALANCE_MESSAGE				\
	"<h3>Trial Balance</h3>\n"					\
	"<p style=\"margin: 3\%;\">"					\
	"The Trial Balance is the first financial statement to "	\
	"generate.\n"							\
	"<br />"							\
	"<br />Journal entries with a Checking account <i>debit</i> "	\
	"amount (the left column) will <i>increase</i> the Checking "	\
	"balance. The corresponding <i>credit</i> amount (the right "	\
        "column) will probably be a revenue (income) account.\n"	\
	"<br />"							\
	"<br />Journal entries with a Checking account <i>credit</i> "	\
	"amount (the right column) will <i>decrease</i> the Checking "	\
	"balance. The corresponding <i>debit</i> amount "		\
	"(the left column) will probably be an expense account.\n"	\
	"<br />"							\
	"<br />If the journal entry above is for a Checking account, "	\
	"the <i>credit</i> amount is posted to the Net Asset account. "	\
	"Net Asset is an Equity element. Equity elements are rarely "	\
	"used to record normal activities. However, this opening "	\
	"entry is not a normal activity.\n"				\
	"<br />"							\
	"<br /><big>Become familiar with the seven elements</big>. "	\
	"See Lookup --> Transaction --> Element\n"			\
	"<br />"							\
	"<br />The only normal activity that will post to an Equity "	\
	"element is when you transfer money from your checking account "\
	"to pay down your credit card account. The transfer activity "	\
	"generates two transactions. "					\
	"One transaction posts the decrease in checking cash. "		\
	"The other transaction posts the decrease in "			\
	"credit card liability. The common account in both "		\
	"transactions is called Credit Card Passthru. It is an Equity "	\
	"account.\n"							\
	"<br />"							\
	"<br />Your Trial Balance follows. It contains Bookkeeping "	\
	"rule #1.\n"							\
	"<br />"							\
	"<br />Bookkeeping rule #1: Total Debit = Total Credit\n"	\
	"<br />"							\
	"<br />If Total Debit != Total Credit, then <i>stop</i> "	\
	"generating financial statements. "				\
	"Instead, execute Audit --> Ledger Debit Credit Audit"

#define FEEDER_INIT_ACTIVITY_MESSAGE					\
	"<h3>Statement of Activities</h3>\n"				\
	"<p style=\"margin: 3\%;\">"					\
	"The second financial statement to generate is the Statement "	\
	"of Activities. It will be blank if there are no activities. "	\
	"Common activities are revenue (income) and expense "		\
	"transactions. "						\
	"Uncommon actitites are gain and loss transactions.\n"		\
	"<br />"							\
	"<br />A note about bookkeeping jargon: "			\
	"The elements of revenue, expense, gain, and loss are known "	\
	"as nominal elements. Therefore, the accounts assigned to "	\
	"these elements are known as nominal accounts. "		\
	"Nominal accounts have the property that they are closed "	\
	"(reset to zero) at year-end.\n"				\
	"<br />See Alter --> Close Nominal Accounts"

#define FEEDER_INIT_POSITION_MESSAGE					\
	"<h3>Financial Position</h3>\n"					\
	"<p style=\"margin: 3\%;\">"					\
	"The Financial Position is the third financial statement to "	\
	"generate.\n"							\
	"<br />Bookkeeping rule #2: "					\
	"Asset Element = Liability Element + Equity Element\n"

#define FEEDER_INIT_UPLOAD_MESSAGE 					\
	"<h3>Next Step</h3>\n"						\
	"<p style=\"margin: 3\%;\">"					\
	"The next step is to upload this feeder file to generate "	\
	"journal entries for all of these activity transactions. "	\
	"The process to execute is called Feeder Upload. "		\
	"However, it is likely that only a few transactions get "	\
	"generated the first time.\n"					\
	"<br />"							\
	"<br />In this feeder file, each transaction is described with "\
	"some text that contains the vendor's name. "			\
	"PredictBooks calls the vendor's name and any descriptive "	\
	"words a feeder phrase.\n"					\
	"<br />"							\
	"<br />Execute the Feeder Upload process with "			\
	"Execute yn set to 'No' or left as 'Select'. "			\
	"A Feeder Error Table will display containing a row for each "	\
	"activity that will not generate a journal entry. "		\
	"Each row contains the vendor's name. "				\
	"Copy and paste the vendor's name and any adjoining "		\
	"descriptive words into the following path:\n"			\
	"<br />"							\
	"<br />Feeder --> Insert Feeder Phrase\n"			\
	"<br />"							\
	"<br />Hint: [Right Click] your browser's tab and select "	\
	"Duplicate Tab. You will then have two instances of "		\
	"PredictBooks open. One tab is for the Feeder Upload process. "	\
	"The other tab is for Feeder --> Insert Feeder Phrase.\n"

#define FEEDER_INIT_MESSAGES_AVAILABLE_MESSAGE 				\
	"<h3>Messages Available</h3>\n"					\
	"<p style=\"margin: 3\%;\">"					\
	"These messages are available anytime by executing Feeder --> "	\
	"Initialize Feeder Account. Leave at least one choice blank. "	\
	"Set Execute yn to 'Yes' for the extended messages."

typedef struct
{
	boolean institution_missing_boolean;
	char *account_name;
	boolean account_exist_boolean;
	char *date_now_yyyy_mm_dd;
	boolean date_recent_boolean;
	ENTITY_SELF *entity_self;
	char *appaserver_error_filespecification;
} FEEDER_INIT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_INIT_INPUT *feeder_init_input_new(
		char *application_name,
		char *financial_institution_full_name,
		boolean checking_boolean,
		char *exchange_minimum_date_string );

/* Process */
/* ------- */
FEEDER_INIT_INPUT *feeder_init_input_calloc(
		void );

boolean feeder_init_input_institution_missing_boolean(
		const char *security_forbid_character_string,
		char *financial_institution_full_name );

boolean feeder_init_input_account_exist_boolean(
		const char *account_table,
		char *feeder_init_input_account_name );

boolean feeder_init_input_date_recent_boolean(
		const int feeder_init_input_days_ago,
		char *exchange_minimum_date_string,
		char *date_now_yyyy_mm_dd );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *feeder_init_input_account_name(
		char *financial_institution_full_name,
		boolean checking_boolean );

typedef struct
{
	char *date_time;
	TRANSACTION *transaction;
} FEEDER_INIT_TRANSACTION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_INIT_TRANSACTION *feeder_init_transaction_new(
		const char *transaction_begin_time,
		double exchange_journal_begin_amount,
		char *exchange_minimum_date_string,
		char *entity_self_full_name,
		char *entity_self_street_address,
		JOURNAL *debit_journal,
		JOURNAL *credit_journal );

/* Process */
/* ------- */
FEEDER_INIT_TRANSACTION *feeder_init_transaction_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *feeder_init_transaction_date_time(
		const char *transaction_begin_time,
		char *exchange_minimum_date_string );

/* Usage */
/* ----- */

/* Returns heap memory from static list */
/* ------------------------------------ */
char *feeder_init_transaction_equity_account_name(
		const char *account_equity_key );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
JOURNAL *feeder_init_transaction_journal(
		double exchange_journal_begin_amount,
		char *account_name,
		boolean debit_boolean,
		boolean fetch_account_boolean );

/* Usage */
/* ----- */
void feeder_init_transaction_insert(
		FEEDER_INIT_TRANSACTION *feeder_init_transaction );

typedef struct
{
	char *feeder_init_transaction_equity_account_name;
	JOURNAL *debit_journal;
	JOURNAL *credit_journal;
	FEEDER_INIT_TRANSACTION *feeder_init_transaction;
} FEEDER_INIT_CHECKING;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_INIT_CHECKING *feeder_init_checking_new(
		boolean execute_boolean,
		double exchange_journal_begin_amount,
		char *exchange_minimum_date_string,
		char *account_name,
		char *entity_self_full_name,
		char *entity_self_street_address );

/* Process */
/* ------- */
FEEDER_INIT_CHECKING *feeder_init_checking_calloc(
		void );

typedef struct
{
	char *feeder_init_transaction_equity_account_name;
	JOURNAL *debit_journal;
	JOURNAL *credit_journal;
	FEEDER_INIT_TRANSACTION *feeder_init_transaction;
} FEEDER_INIT_CREDIT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_INIT_CREDIT *feeder_init_credit_new(
		boolean execute_boolean,
		double negate_exchange_journal_begin_amount,
		char *exchange_minimum_date_string,
		char *account_name,
		char *entity_self_full_name,
		char *entity_self_street_address );

/* Process */
/* ------- */
FEEDER_INIT_CREDIT *feeder_init_credit_calloc(
		void );

typedef struct
{
	char *account_name;
	char *feeder_phrase;
	char *exist_system_string;
	boolean exist_boolean;
	char *insert_sql;
} FEEDER_INIT_PASSTHRU;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_INIT_PASSTHRU *feeder_init_passthru_new(
		boolean checking_boolean,
		char *entity_self_full_name,
		char *entity_self_street_address );

/* Process */
/* ------- */
FEEDER_INIT_PASSTHRU *feeder_init_passthru_calloc(
		void );

/* Returns either parameter */
/* ------------------------ */
const char *feeder_init_passthru_feeder_phrase(
		const char *feeder_init_cash_feeder_phrase,
		const char *feeder_init_card_feeder_phrase,
		boolean checking_boolean );

boolean feeder_init_passthru_exist_boolean(
		char *feeder_init_passthru_exist_system_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_init_passthru_insert_sql(
		const char *feeder_phrase_table,
		char *account_name,
		char *entity_self_full_name,
		char *entity_self_street_address,
		char *feeder_init_passthru_feeder_phrase );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_init_passthru_exist_system_string(
		const char *feeder_phrase_table,
		char *feeder_init_passthru_feeder_phrase );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_init_passthru_account_name(
		const char *account_passthru_key );

typedef struct
{
	FEEDER_INIT_INPUT *feeder_init_input;
	FEEDER_INIT_CHECKING *feeder_init_checking;
	FEEDER_INIT_CREDIT *feeder_init_credit;
	FEEDER_INIT_PASSTHRU *feeder_init_passthru;
	char *account_insert_sql;
	char *feeder_account_insert_sql;
	LIST *insert_sql_list;
	char *trial_balance_system_string;
	char *activity_system_string;
	char *position_system_string;
} FEEDER_INIT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_INIT *feeder_init_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *financial_institution_full_name,
		boolean checking_boolean,
		double exchange_journal_begin_amount,
		char *exchange_minimum_date_string );

/* Process */
/* ------- */
FEEDER_INIT *feeder_init_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *feeder_init_feeder_account_insert_sql(
		const char *feeder_account_table,
		char *account_name );

LIST *feeder_init_insert_sql_list(
		char *passthru_insert_sql,
		char *account_insert_sql,
		char *feeder_account_insert_sql );

/* Returns static memory */
/* --------------------- */
char *feeder_init_trial_balance_system_string(
		const char *feeder_init_trial_executable,
		char *session_key,
		char *login_name,
		char *role_name );

/* Returns static memory */
/* --------------------- */
char *feeder_init_activity_system_string(
		const char *feeder_init_activity_executable,
		char *session_key,
		char *login_name,
		char *role_name );

/* Returns static memory */
/* --------------------- */
char *feeder_init_position_system_string(
		const char *feeder_init_position_executable,
		char *session_key,
		char *login_name,
		char *role_name );

/* Usage */
/* ----- */
void feeder_init_transaction_html_display(
		FEEDER_INIT_CHECKING *feeder_init_checking,
		FEEDER_INIT_CREDIT *feeder_init_credit );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_init_account_insert_sql(
		const char *account_table,
		const char *subclassification_cash,
		const char *subclassification_current_liability,
		boolean checking_boolean,
		char *account_name );

/* Process */
/* ------- */

/* Returns either parameter */
/* ------------------------ */
const char *feeder_init_subclassification(
		const char *subclassification_cash,
		const char *subclassification_current_liability,
		boolean checking_boolean );

#endif
