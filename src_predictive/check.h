/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/check.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CHECK_H
#define CHECK_H

#include "list.h"
#include "boolean.h"
#include "appaserver_link.h"

#define CHECK_PROMPT		"Press here to view the check(s)."
#define CHECK_DATE_COMMAND	"now.sh pretty | column.e 0"

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
} CHECK;

/* Usage */
/* ----- */
CHECK *check_new(
		double dialog_box_payment_amount,
		int check_number,
		char *check_list_memo,
		double liability_entity_amount_due,
		char *entity_full_name );

/* Process */
/* ------- */
CHECK *check_calloc(
		void );

double check_amount(
		double dialog_box_payment_amount,
		double liability_entity_amount_due );
			
/* Returns static memory */
/* --------------------- */
char *check_dollar_text(
		double check_amount );

/* Returns static memory */
/* --------------------- */
char *check_escape_payable_to(
		char *entity_full_name );

/* Returns program memory */
/* ---------------------- */
char *check_move_down(
		void );

/* Returns static memory */
/* --------------------- */
char *check_date_display(
		char *check_date );

/* Returns static memory */
/* --------------------- */
char *check_vendor_name_amount_due_display(
		char *check_escape_payable_to,
		char *check_amount_due_display );

/* Returns static memory */
/* --------------------- */
char *check_amount_due_stub_display(
		char *check_amount_due_display );

/* Returns static memory */
/* --------------------- */
char *check_dollar_text_display(
		char *check_dollar_text );

/* Returns static memory */
/* --------------------- */
char *check_number_display(
		int check_number );

/* Returns program memory */
/* ---------------------- */
char *check_newpage(
		void );

/* Returns heap memory */
/* ------------------- */
char *check_output_string(
		char *check_move_down,
		char *check_date_display,
		char *check_vendor_name_amount_due_display,
		char *check_amount_due_stub_display,
		char *check_dollar_text_display,
		char *check_memo_display,
		char *check_number_display,
		char *check_newpage );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *check_memo_display(
		char *transaction_memo );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *check_stub_memo(
		char *transaction_memo,
		int max_length );

typedef struct
{
	APPASERVER_LINK *appaserver_link;
	char *tex_filename;
	char *tex_anchor_html;
	char *pdf_anchor_html;
	char *appaserver_link_working_directory;
} CHECK_LINK;

/* Usage */
/* ----- */
CHECK_LINK *check_link_new(
		char *application_name,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *session_key );

/* Process */
/* ------- */
CHECK_LINK *check_link_calloc(
		void );

typedef struct
{
	char *documentclass;
	char *usepackage;
	char *pagenumbering_gobble;
	char *begin_document;
	char *heading;
	LIST *list;
	char *end_document;
	CHECK_LINK *check_link;
	FILE *appaserver_output_file;
} CHECK_LIST;

/* Usage */
/* ----- */
CHECK_LIST *check_list_new(
		char *application_name,
		double dialog_box_payment_amount,
		int starting_check_number,
		char *transaction_memo,
		char *dappaserver_parameter_data_directory,
		char *process_name,
		char *session_key,
		LIST *liability_payment_entity_list );

/* Process */
/* ------- */
CHECK_LIST *check_list_calloc(
		void );

/* Returns program memory */
/* ---------------------- */
char *check_list_documentclass(
		void );

/* Returns program memory */
/* ---------------------- */
char *check_list_usepackage(
		void );

/* Returns program memory */
/* ---------------------- */
char *check_list_pagenumbering_gobble(
		void );

/* Returns program memory */
/* ---------------------- */
char *check_list_begin_document(
		void );

/* Returns heap memory */
/* ------------------- */
char *check_list_heading(
		char *check_list_documentclass,
		char *check_list_usepackage,
		char *check_list_pagenumbering_gobble,
		char *check_list_begin_document );

/* Returns either parameter */
/* ------------------------ */
char *check_list_memo(
		char *transaction_memo,
		char *journal_list_last_memo );

/* Returns program memory */
/* ---------------------- */
char *check_list_end_document(
		void );
#endif
