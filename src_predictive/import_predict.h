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

/* #define IMPORT_PREDICT_SQL			"home_edition.sql" */
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
	"transactions is described with a phrase. Insert these "	      \
	"phrases into the Feeder Phrase table. PredictBooks will "	      \
	"seek out the phrases to generate your transactions automatically."

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

typedef struct
{
	boolean template_boolean;
	boolean exists_boolean;
	ENTITY_SELF *entity_self;
	char *filespecification;
	boolean application_menu_horizontal_boolean;
} IMPORT_PREDICT_INPUT;

/* Safely returns */
/* -------------- */
IMPORT_PREDICT_INPUT *import_predict_input_new(
		const char *import_predict_sqlgz,
		char *application_name,
		char *mount_point );

/* Process */
/* ------- */
IMPORT_PREDICT_INPUT *import_predict_input_calloc(
		void );

boolean import_predict_input_template_boolean(
		const char *application_create_template_name,
		char *application_name );

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
	char *system_string;
	char *delete_role_system_string;
	char *menu_href_string;
	char *menu_anchor_tag;
} IMPORT_PREDICT;

/* Safely returns */
/* -------------- */
IMPORT_PREDICT *import_predict_new(
		const char *import_predict_sqlgz,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
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

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *import_predict_system_string(
		char *import_predict_filename );

#endif
