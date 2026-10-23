/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/import_predict.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "application.h"
#include "frameset.h"
#include "entity_self.h"
#include "transaction.h"
#include "export_table.h"
#include "export_process.h"

#define IMPORT_PREDICT_SQLGZ			"home_edition.sql.gz"
#define IMPORT_PREDICT_PROCESS_NAME		"import_predictbooks"

#define IMPORT_PREDICT_EXISTS_MESSAGE					\
	"<p style=\"margin: 3\%;\">"					\
	"Warning: PredictBooks is already installed and will be "	\
	"replaced."

#define IMPORT_PREDICT_ENTITY_SELF_MESSAGE				\
	"<p style=\"margin: 3\%;\">"					\
	"ERROR: your name must be inserted into the table Self. "	\
	"The Supervisor role path is Insert --> Entity --> Self"

#define IMPORT_PREDICT_ONCE_MESSAGE					\
	"<p style=\"margin: 3\%;\">"					\
	"This process should only be executed again "			\
	"if you want to delete PredictBooks and start over. "		\
	"Also, this process will only be available to the System role."

#define IMPORT_PREDICT_TEMPLATE_MESSAGE					\
	"<p style=\"margin: 3\%;\">"					\
	"Warning: You are currently connected to the template "		\
	"database. The template database is meant to spawn your "	\
	"multiple databases. Whereas you can import PredictBooks "	\
	"here, we recommend that you first execute the Alter process "	\
	"called Create Empty Application. This process is available "	\
	"to the System role."

#define IMPORT_PREDICT_SHORTCUT_MESSAGE 				      \
	"<p style=\"margin: 3\%;\">"					      \
	"If you are not yet familiar with "				      \
	"<a href=\"https://en.wikipedia.org/wiki/Double-entry_bookkeeping\""  \
	"target=_new>"							      \
	"double-entry bookkeeping</a>, "				      \
	"the skill will develop over time. PredictBooks has "		      \
	"available a shortcut process called Insert Expense "	      	\
	"Transaction. This process will prompt you for the necessary "	      \
	"information and generate the correct double-entry "		      \
	"transaction.\n"						      \
	"<br />"							      \
	"<br />"							      \
	"However, you will only have to insert a small number of "	      \
	"transactions, because most of them will be generated "		      \
	"automatically. After you install PredictBooks, become "	      \
	"familiar with the Feeder Phrase table. Each of your bank's "	      \
	"transactions contain the company's name. PredictBooks calls them "   \
	"feeder phrases. Insert these feeder phrases into the Feeder Phrase " \
	"table. PredictBooks will seek out the phrases to generate your "     \
	"transactions automatically."

#define IMPORT_PREDICT_REFRESH_MESSAGE 					\
	"<p style=\"margin: 3\%;\">"					\
	"This process changed the database that is not yet reflected "	\
	"in the menu above. To update the menu above, click this "	\
	"link: "

#define IMPORT_PREDICT_NEXT_STEP_MESSAGE				     \
	"<p style=\"margin: 3\%;\">"					     \
	"The next step is to execute Initialize Feeder Account. "	     \
	"Feeder accounts are either checking or credit card accounts "	     \
	"from financial institutions that feed transactions into "	     \
	"PredictBooks. Note: Bank of America does not support the standard " \
	"feeder account format. Therefore, special processes are available " \
	"for Bank of America's file format of Excel.\n"			     \
	"<br />"							     \
	"<br />"							     \
	"Log into your bank's website and download all "		     \
	"of your checking transactions that are available. "		     \
	"The bank's download dialog box will probably give you choice "	     \
	"of format options. PredictBooks recognizes two "		     \
	"format options. The most current format option is "	     	     \
	"called "							     \
	"<a href=\"https://en.wikipedia.org/wiki/Open_Financial_Exchange\""  \
	"target=_new>"							     \
	"Open Financial eXchange</a>. However, your bank may call it by "    \
	"the name of other bookkeeping software products. "		     \
	"Nonetheless, the downloaded filename should end with either "	     \
	"'FX' or 'fx'."							     \
	"<br />"							     \
	"<br />"							     \
	"The next step is to:\n"					     \
	"<ol>\n"							     \
	"<li>Download all of your checking transactions from your "	     \
	"bank.\n"							     \
	"<li>Execute: Feeder --> Initialize Feeder Account.\n"	     	     \
	"</ol>\n"

enum import_predict_input_financial_enum
{
	predict_financial_bank_of_america,
	predict_financial_other_bank_name,
	predict_financial_no_update
};

typedef struct
{
	boolean template_boolean;
	boolean exists_boolean;
	ENTITY_SELF *entity_self;
	char *filespecification;
	boolean application_menu_horizontal_boolean;
	enum import_predict_input_financial_enum
		financial_enum;
	boolean appaserver_user_boolean;
} IMPORT_PREDICT_INPUT;

/* Safely returns */
/* -------------- */
IMPORT_PREDICT_INPUT *import_predict_input_new(
		const char *import_predict_sqlgz,
		char *application_name,
		boolean bank_of_america_boolean,
		char *mount_point );

/* Process */
/* ------- */
IMPORT_PREDICT_INPUT *import_predict_input_calloc(
		void );

boolean import_predict_input_template_boolean(
		const char *application_create_template_name,
		char *application_name );

boolean import_predict_input_appaserver_user_boolean(
		const char *application_clone_name_placeholder,
		char *entity_self_full_name );

/* Returns heap memory */
/* ------------------- */
char *import_predict_input_filespecification(
		const char *import_predict_sqlgz,
		char *mount_point );

/* Usage */
/* ----- */
boolean import_predict_input_exists_boolean(
		const char *transaction_table );

/* Usage */
/* ----- */
enum import_predict_input_financial_enum import_predict_input_financial_enum(
		const char *entity_financial_institution_table,
		boolean bank_of_america_boolean );

typedef struct
{
	IMPORT_PREDICT_INPUT *import_predict_input;
	char *system_string;
	char *delete_role_system_string;
	char *menu_href_string;
	char *menu_anchor_tag;
	char *bank_update_name;
	char *entity_bank_financial_update_statement;
	char *entity_bank_update_statement;
	char *entity_vendor_update_statement;
	char *entity_feeder_phrase_update_statement;
	char *entity_user_update_statement;
	char *entity_self_user_update_statement;
	char *appaserver_user_update_statement;
	char *session_update_statement;
	char *role_appaserver_user_update_statement;
	char *login_default_role_update_statement;
} IMPORT_PREDICT;

/* Safely returns */
/* -------------- */
IMPORT_PREDICT *import_predict_new(
		const char *import_predict_sqlgz,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		boolean bank_of_america_boolean,
		char *bank_full_name,
		char *your_full_name,
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

/* Returns either parameter or null */
/* -------------------------------- */
char *import_predict_bank_update_name(
		const char *feeder_install_bank_of_america,
		char *bank_full_name,
		enum import_predict_input_financial_enum financial_enum );

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
char *import_predict_update_statement(
		const char *table_name,
		const char *placeholder,
		char *update_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *import_predict_appaserver_user_update_statement(
		const char *table_name,
		const char *placeholder,
		char *update_name );

