/* --------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/liability.c	 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "list.h"
#include "transaction.h"
#include "appaserver_library.h"
#include "subclassification.h"
#include "application.h"
#include "account.h"
#include "journal.h"
#include "latex.h"
#include "dollar.h"
#include "liability.h"

LIABILITY *liability_entity_fetch(
			char *full_name,
			char *street_address,
			LIST *account_current_liability_name_list )
{
	LIABILITY *liability = liability_calloc();

	if ( !full_name
	||   !street_address
	||   !list_length( account_current_liability_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability->timlib_in_clause =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		timlib_in_clause(
			account_current_liability_name_list );

	liability->entity_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_entity_where(
			full_name,
			street_address,
			liability->timlib_in_clause );

	liability->journal_system_list =
		journal_system_list(
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				liability->entity_where ),
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			0 /* not fetch_transaction */ );

	if ( !list_length( liability->journal_system_list ) )
	{
		free( liability );
		return (LIABILITY *)0;
	}

	liability->journal_credit_debit_difference_sum =
		journal_credit_debit_difference_sum(
			liability->journal_system_list );

	if ( money_virtually_same(
		liability->journal_credit_debit_difference_sum,
		0.0 )
	||	liability->journal_credit_debit_difference_sum < 0.0 )
	{
		list_free( liability->journal_system_list );
		free( liability );
		return (LIABILITY *)0;
	}

	liability->liability_account_list =
		liability_account_list_new(
			liability->journal_system_list );

	return liability;
}

char *liability_entity_where(
			char *full_name,
			char *street_address,
			char *timlib_in_clause )
{
	static char where[ 512 ];

	if ( !full_name
	||   !street_address
	||   !timlib_in_clause )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"full_name = '%s' and "
		"street_address = '%s' and "
		"account in (%s)",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name(
			full_name ),
			street_address,
			timlib_in_clause );

	return where;
}

LIABILITY *liability_account_fetch( char *liability_account_name )
{
	LIABILITY *liability;

	if ( !liability_account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: liability_account_name empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability = liability_calloc();

	liability->account_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_account_where(
			liability_account_name );

	liability->journal_system_list =
		journal_system_list(
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				liability->account_where ),
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			0 /* not fetch_transaction */ );

	if ( !list_length( liability->journal_system_list ) )
	{
		free( liability );
		return (LIABILITY *)0;
	}

	liability->journal_credit_debit_difference_sum =
		journal_credit_debit_difference_sum(
			liability->journal_system_list );

	if ( money_virtually_same(
		liability->journal_credit_debit_difference_sum,
		0.0 )
	||   liability->journal_credit_debit_difference_sum < 0.0 )
	{
		list_free( liability->journal_system_list );
		free( liability );
		return (LIABILITY *)0;
	}

	liability->liability_account_list =
		liability_account_list_new(
			liability->journal_system_list );

	return liability;
}

char *liability_account_where( char *liability_account_name )
{
	static char where[ 128 ];

	sprintf(where,
		"account = '%s'",
		liability_account_name );

	return where;
}

LIABILITY *liability_calloc( void )
{
	LIABILITY *liability;

	if ( ! ( liability = calloc( 1, sizeof( LIABILITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}
	return liability;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_calloc( void )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( ! ( liability_account_entity =
			calloc( 1, sizeof( LIABILITY_ACCOUNT_ENTITY ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return liability_account_entity;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_new(
			char *account_name )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity =
		liability_account_entity_calloc();

	liability_account_entity->account_name = account_name;

	return liability_account_entity;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_parse(
			char *string_input )
{
	char account_name[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( !string_input ) return (LIABILITY_ACCOUNT_ENTITY *)0;

	/* See LIABILITY_ACCOUNT_ENTITY_SELECT */
	/* ------------------------------------ */
	piece( account_name, SQL_DELIMITER, string_input, 0 );

	liability_account_entity =
		liability_account_entity_new(
			strdup( account_name ) );

	if ( piece( full_name, SQL_DELIMITER, string_input, 1 ) )
	{
		piece( street_address, SQL_DELIMITER, string_input, 2 );

		liability_account_entity->entity =
			entity_new(
				strdup( full_name ),
				strdup( street_address ) );
	}

	return liability_account_entity;
}

LIST *liability_account_entity_list( void )
{
	return
	liability_account_entity_system_list(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_account_entity_system_string(
			LIABILITY_ACCOUNT_ENTITY_SELECT,
			LIABILITY_ACCOUNT_ENTITY_TABLE ) );
}

char *liability_account_entity_system_string(
			char *select,
			char *table )
{
	static char system_string[ 128 ];

	sprintf(system_string,
		"select.sh \"%s\" %s",
		select,
		table );

	return system_string;
}

LIST *liability_account_entity_system_list( char *system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *liability_account_entity_list;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_account_entity_list = list_new();

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(	liability_account_entity_list,
				liability_account_entity_parse( input ) );
	}

	pclose( input_pipe );
	return liability_account_entity_list;
}

LIST *liability_account_entity_account_name_list(
			LIST *liability_account_entity_list )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;
	LIST *account_name_list;

	account_name_list = list_new();

	if ( !list_rewind( liability_account_entity_list ) )
	{
		return account_name_list;
	}

	do {
		liability_account_entity =
			list_get(
				liability_account_entity_list );

		if ( !liability_account_entity->account_name )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			account_name_list,
			liability_account_entity->account_name );

	} while ( list_next( liability_account_entity_list ) );

	return account_name_list;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek(
			char *account_name,
			LIST *liability_account_entity_list )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( liability_account_entity_list ) )
	{
		return (LIABILITY_ACCOUNT_ENTITY *)0;
	}

	do {
		liability_account_entity =
			list_get(
				liability_account_entity_list );

		if ( string_strcmp(
			liability_account_entity->account_name,
			account_name ) == 0 )
		{
			return liability_account_entity;
		}

	} while ( list_next( liability_account_entity_list ) );

	return (LIABILITY_ACCOUNT_ENTITY *)0;
}

ENTITY *liability_account_entity(
			LIST *liability_account_entity_list,
			char *account_name )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( !list_rewind( liability_account_entity_list ) )
		return (ENTITY *)0;

	do {
		liability_account_entity =
			list_get(
				liability_account_entity_list );

		if ( string_strcmp( 
			liability_account_entity->account_name,
			account_name ) == 0 )
		{
			return liability_account_entity->entity;
		}

	} while ( list_next( liability_account_entity_list ) );

	return (ENTITY *)0;
}

char *liability_payment_credit_account_name(
			int starting_check_number,
			char *account_cash,
			char *account_uncleared_checks )
{
	if ( starting_check_number )
		return account_uncleared_checks;
	else
		return account_cash;
}

LIABILITY_ENTITY *liability_entity_account_name_new(
			char *account_name,
			ENTITY *entity )
{
	LIABILITY_ENTITY *liability_entity;

	if ( !account_name
	||   !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_entity = liability_entity_calloc();
	liability_entity->entity = entity;

	if ( ! ( liability_entity->liability =
			liability_account_fetch(
				account_name ) ) )
	{
		free( liability_entity );
		return (LIABILITY_ENTITY *)0;
	}

	liability_entity->amount_due =
		liability_entity_amount_due(
			liability_entity->liability,
			(RECEIVABLE *)0 /* receivable */ );

	return liability_entity;
}

LIABILITY_ENTITY *liability_entity_account_list_new(
			LIST *account_current_liability_name_list,
			LIST *account_receivable_name_list,
			ENTITY *entity )
{
	LIABILITY_ENTITY *liability_entity;

	if ( !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: entity is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( account_current_liability_name_list ) )
		return (LIABILITY_ENTITY *)0;

	liability_entity = liability_entity_calloc();
	liability_entity->entity = entity;

	if ( ! ( liability_entity->liability =
			liability_entity_fetch(
				entity->full_name,
				entity->street_address,
				account_current_liability_name_list ) ) )
	{
		free( liability_entity );
		return (LIABILITY_ENTITY *)0;
	}

	if ( list_length( account_receivable_name_list ) )
	{
		liability_entity->receivable =
			receivable_fetch(
				entity->full_name,
				entity->street_address,
				account_receivable_name_list );
	}

	liability_entity->amount_due =
		liability_entity_amount_due(
			liability_entity->liability,
			liability_entity->receivable );

	return liability_entity;
}

double liability_entity_amount_due(
			LIABILITY *liability,
			RECEIVABLE *receivable )
{
	double amount_due;

	if ( !liability ) return 0.0;

	if ( ! ( amount_due = liability->journal_credit_debit_difference_sum ) )
	{
		return 0.0;
	}

	if ( receivable )
	{
		amount_due -= receivable->journal_debit_credit_difference_sum;
	}

	return amount_due;
}

LIABILITY_ENTITY *liability_entity_calloc( void )
{
	LIABILITY_ENTITY *liability_entity;

	if ( ! ( liability_entity = calloc( 1, sizeof( LIABILITY_ENTITY ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_entity;
}

LIABILITY_PAYMENT *liability_payment_new(
			char *application_name,
			double dialog_box_payment_amount,
			int starting_check_number,
			char *dialog_box_memo,
			char *document_root_directory,
			char *process_name,
			char *session_key,
			LIST *entity_full_street_list )
{
	LIABILITY_PAYMENT *liability_payment;

	if ( !application_name
	||   !document_root_directory
	||   !process_name
	||   !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_payment = liability_payment_calloc();

	if ( !list_length( entity_full_street_list ) )
	{
		liability_payment->error_message =
			liability_payment_error_message(
				"Please select an entity." );

		return liability_payment;
	}

	liability_payment->transaction_memo =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_memo( dialog_box_memo );

	if ( list_length( entity_full_street_list ) > 1
	&& (  dialog_box_payment_amount
	||    *liability_payment->transaction_memo ) )
	{
		liability_payment->error_message =
			liability_payment_error_message(
			"Please choose only one entity for this option." );

		return liability_payment;
	}

	liability_payment->liability_calculate =
		liability_calculate_new(
			application_name );

	if ( !liability_payment->liability_calculate )
	{
		liability_payment->error_message =
			liability_payment_error_message(
			"No liabilities due." );

		return liability_payment;
	}

	liability_payment->entity_list =
		liability_payment_entity_list(
			entity_full_street_list,
			liability_payment->
				liability_calculate->
				liability_entity_list );

	if ( !list_length( liability_payment->entity_list ) )
	{
		liability_payment->error_message =
			liability_payment_error_message(
				"Could not fulfill the liabilities." );

		return liability_payment;
	}

	if ( starting_check_number )
	{
		liability_payment->liability_check_list =
			liability_check_list_new(
				application_name,
				dialog_box_payment_amount,
				starting_check_number,
				liability_payment->transaction_memo,
				document_root_directory,
				process_name,
				session_key,
				liability_payment->entity_list );
	}

	liability_payment->credit_account_name =
		liability_payment_credit_account_name(
			starting_check_number,
			account_cash(
				ACCOUNT_CASH_KEY ),
			account_uncleared_checks(
				ACCOUNT_UNCLEARED_CHECKS_KEY ) );

	liability_payment->liability_transaction_list =
		liability_transaction_list_new(
			dialog_box_payment_amount,
			starting_check_number,
			liability_payment->transaction_memo,
			liability_payment->entity_list,
			liability_payment->credit_account_name );

	if ( !liability_payment->liability_transaction_list )
	{
		liability_payment->error_message =
			liability_payment_error_message(
				"Could not generate the transactions." );

		return liability_payment;
	}

	return liability_payment;
}

LIABILITY_PAYMENT *liability_payment_calloc( void )
{
	LIABILITY_PAYMENT *liability_payment;

	if ( ! ( liability_payment =
			calloc( 1, sizeof( LIABILITY_PAYMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_payment;
}

LIABILITY_CHECK *liability_check_calloc( void )
{
	LIABILITY_CHECK *liability_check;

	if ( ! ( liability_check = calloc( 1, sizeof( LIABILITY_CHECK ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_check;
}

LIABILITY_CHECK_LIST *liability_check_list_new(
			char *application_name,
			double dialog_box_payment_amount,
			int starting_check_number,
			char *transaction_memo,
			char *document_root_directory,
			char *process_name,
			char *session_key,
			LIST *liability_payment_entity_list )
{
	LIABILITY_CHECK_LIST *liability_check_list;
	LIABILITY_CHECK *liability_check;
	FILE *output_file;

	if ( !application_name
	||   !document_root_directory
	||   !process_name
	||   !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( liability_payment_entity_list ) )
	{
		return (LIABILITY_CHECK_LIST *)0;
	}

	liability_check_list = liability_check_list_calloc();

	liability_check_list->documentclass =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		liability_check_list_documentclass();

	liability_check_list->usepackage =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		liability_check_list_usepackage();

	liability_check_list->pagenumbering_gobble =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		liability_check_list_pagenumbering_gobble();

	liability_check_list->begin_document =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		liability_check_list_begin_document();

	liability_check_list->heading =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		liability_check_list_heading(
			liability_check_list->documentclass,
			liability_check_list->usepackage,
			liability_check_list->pagenumbering_gobble,
			liability_check_list->begin_document );

	liability_check_list->list = list_new();
	list_rewind( liability_payment_entity_list );

	do {
		list_set(
			liability_check_list->list,
			liability_check_new(
				dialog_box_payment_amount,
				starting_check_number++
					/* check_number */,
				transaction_memo,
				list_get( liability_payment_entity_list ) ) );

	} while ( list_next( liability_payment_entity_list ) );

	if ( !list_length( liability_check_list->list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_check_list->end_document =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		liability_check_list_end_document();

	liability_check_list->liability_check_appaserver_link =
		liability_check_appaserver_link_new(
			application_name,
			document_root_directory,
			process_name,
			session_key );

	if ( ! ( output_file =
			fopen(
				liability_check_list->
					liability_check_appaserver_link->
					output_filename,
				"w" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			liability_check_list->
				liability_check_appaserver_link->
				output_filename );
		exit( 1 );
	}

	fprintf(output_file,
		"%s\n",
		liability_check_list->heading );

	list_rewind( liability_check_list->list );

	do {
		liability_check = list_get( liability_check_list->list );

		if ( !liability_check->output_string )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: output_string is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		fprintf(output_file,
			"%s\n",
			liability_check->output_string );

	} while ( list_next( liability_check_list->list ) );

	fprintf(output_file,
		"%s\n",
		liability_check_list->end_document );

	fclose( output_file );

	latex_tex2pdf(
		liability_check_list->
			liability_check_appaserver_link->
			output_filename,
		appaserver_link_source_directory(
			document_root_directory,
			application_name )
				/* working_directory */ );

	return liability_check_list;
}

LIABILITY_CHECK *liability_check_new(
			double dialog_box_payment_amount,
			int check_number,
			char *transaction_memo,
			LIABILITY_ENTITY *liability_entity )
{
	LIABILITY_CHECK *liability_check;

	if ( !liability_entity
	||   !liability_entity->entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: entity is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_check = liability_check_calloc();

	liability_check->amount =
		liability_check_amount(
			dialog_box_payment_amount,
			liability_entity->amount_due );

	if ( liability_check->amount <= 0.0 )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: negative liability check amount.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (LIABILITY_CHECK *)0;
	}

	liability_check->dollar_text =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_check_dollar_text(
			liability_check->amount );

	liability_check->escape_payable_to =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_check_escape_payable_to(
			liability_entity->
				entity->
				full_name );

	liability_check->move_down =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		liability_check_move_down();

	liability_check->date_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_check_date_display(
			pipe2string(
				LIABILITY_CHECK_DATE_COMMAND )
					/* check_date */ );

	liability_check->amount_due_display =
		/* ------------------------------------------------- */
		/* Returns amount_due_display which is static memory */
		/* ------------------------------------------------- */
		liability_check_amount_due_display(
			place_commas_in_money(
				liability_check->amount ) );

	liability_check->vendor_name_amount_due_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_check_vendor_name_amount_due_display(
			liability_check->escape_payable_to,
			liability_check->amount_due_display );

	liability_check->amount_due_stub_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_check_amount_due_stub_display(
			liability_check->amount_due_display );

	liability_check->dollar_text_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_check_dollar_text_display(
			liability_check->dollar_text );

	liability_check->memo_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_check_memo_display(
			transaction_memo );

	liability_check->number_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_check_number_display(
			check_number );

	liability_check->newpage =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		liability_check_newpage();

	liability_check->output_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		liability_check_output_string(
			liability_check->move_down,
			liability_check->date_display,
			liability_check->vendor_name_amount_due_display,
			liability_check->amount_due_stub_display,
			liability_check->dollar_text_display,
			liability_check->memo_display,
			liability_check->number_display,
			liability_check->newpage );

	return liability_check;
}

char *liability_check_list_documentclass( void )
{
	return
"\\documentclass{report}";
}

char *liability_check_list_usepackage( void )
{
	return
"\\usepackage[	portrait,\n"
"		top=0in,\n"
"		left=0in,\n"
"		paperheight=2.875in,\n"
"		paperwidth=8.5in,\n"
"		textheight=2.875in,\n"
"		textwidth=8.5in,\n"
"		noheadfoot]{geometry}";
}

char *liability_check_list_pagenumbering_gobble( void )
{
	return
"\\pagenumbering{gobble}";
}

char *liability_check_list_begin_document( void )
{
	return
"\\begin{document}";
}

char *liability_check_newpage( void )
{
	return
"\\newpage\n";
}

char *liability_check_list_end_document( void )
{
	return
"\\end{document}";
}

char *liability_check_dollar_text(
			double amount_due )
{
	static char text[ 128 ];

	if ( amount_due <= 0.0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid amount_due.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	dollar_text( text, amount_due );

	return text;
}

char *liability_check_move_down( void )
{
	return
"\\begin{tabular}l\n"
"\\end{tabular}\n\n"
"\\vspace{0.45in}";
}

char *liability_check_date_display( char *check_date )
{
	static char date_display[ 256 ];

	if ( !check_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: check_date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(date_display,
"\\begin{tabular}{p{0.2in}p{6.6in}l}\n"
"& %s & %s\n"
"\\end{tabular}\n\n",
		check_date,
		check_date );

	return date_display;
}

char *liability_check_vendor_amount_due_display(
			char *payable_to,
			char *amount_due_display )
{
	static char display[ 256 ];

	if ( !payable_to
	||   !amount_due_display )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(display,
"\\begin{tabular}{p{0.2in}p{2.5in}p{4.1in}l}\n"
"& %.26s & %s & %s\n"
"\\end{tabular}\n",
		payable_to,
		payable_to,
		amount_due_display );

	return display;
}

char *liability_check_amount_due_display( char *amount_due_display )
{
	return amount_due_display;
}

char *liability_check_amount_due_stub_display( char *amount_due_display )
{
	static char display[ 128 ];

	if ( !amount_due_display )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: amount_due_display is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(display,
"\\vspace{0.10in}\n\n"
"\\begin{tabular}{p{0.2in}l}\n"
"& %s\n"
"\\end{tabular}",
		amount_due_display );

	return display;
}

char *liability_check_dollar_text_display( char *dollar_text )
{
	static char display[ 256 ];

	if ( !dollar_text )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: dollar_text is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(display,
"\\vspace{0.10in}\n\n"
"\\begin{tabular}{p{0.2in}p{2.5in}l}\n"
"& & %s\n"
"\\end{tabular}\n",
		dollar_text );

	return display;
}

char *liability_check_memo_display( char *transaction_memo )
{
	static char display[ 256 ];

	if ( !transaction_memo )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: transaction_memo is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( *transaction_memo )
	{
		sprintf(display,
"\\vspace{0.20in}\n\n"
"\\begin{tabular}{p{0.2in}p{2.5in}l}\n"
"& %s & %s\n"
"\\end{tabular}\n\n",
			transaction_memo,
			transaction_memo );
	}
	else
	{
		sprintf(display,
"\\vspace{0.20in}\n\n"
"\\begin{tabular}l\n"
"\\end{tabular}\n\n" );
	}

	return display;
}

char *liability_check_number_display( int check_number )
{
	static char display[ 128 ];

	if ( !check_number )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: check_number is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(display,
"\\vspace{0.05in}\n\n"
"\\begin{tabular}{p{0.2in}l}\n"
"& Check: %d\n"
"\\end{tabular}\n",
		check_number );

	return display;
}

char *liability_check_output_string(
			char *liability_check_move_down,
			char *liability_check_date_display,
			char *liability_check_vendor_name_amount_due_display,
			char *liability_check_amount_due_stub_display,
			char *liability_check_dollar_text_display,
			char *liability_check_memo_display,
			char *liability_check_number_display,
			char *liability_check_newpage )
{
	char output_string[ 4096 ];

	if ( !liability_check_move_down
	||   !liability_check_date_display
	||   !liability_check_vendor_name_amount_due_display
	||   !liability_check_amount_due_stub_display
	||   !liability_check_dollar_text_display
	||   !liability_check_memo_display
	||   !liability_check_number_display
	||   !liability_check_newpage )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(output_string,
		"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
		liability_check_move_down,
		liability_check_date_display,
		liability_check_vendor_name_amount_due_display,
		liability_check_amount_due_stub_display,
		liability_check_dollar_text_display,
		liability_check_memo_display,
		liability_check_number_display,
		liability_check_newpage );

	return strdup( output_string );
}

char *liability_check_list_heading(
			char *documentclass,
			char *usepackage,
			char *pagenumbering_gobble,
			char *begin_document )
{
	char heading[ 2048 ];

	if ( !documentclass
	||   !usepackage
	||   !pagenumbering_gobble
	||   !begin_document )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(heading,
		"%s\n%s\n%s\n%s\n",
		documentclass,
		usepackage,
		pagenumbering_gobble,
		begin_document );

	return strdup( heading );
}

LIABILITY_CHECK_APPASERVER_LINK *
	liability_check_appaserver_link_new(
			char *application_name,
			char *document_root_directory,
			char *process_name,
			char *session_key )
{
	APPASERVER_LINK_FILE *appaserver_link_file;
	LIABILITY_CHECK_APPASERVER_LINK *liability_check_appaserver_link;

	liability_check_appaserver_link =
		liability_check_appaserver_link_calloc();

	appaserver_link_file =
	liability_check_appaserver_link->
		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
				document_root_directory,
				process_name /* filename_stem */,
				application_name,
				getpid(),
				(char *)0 /* session_key */,
		 		"tex" );

	liability_check_appaserver_link->
		output_filename =
			appaserver_link_get_output_filename(
				appaserver_link_file->
					output_file->
					document_root_directory,
				appaserver_link_file->application_name,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension );

	appaserver_link_file->extension = "pdf";

	liability_check_appaserver_link->
		pdf_output_filename =
			appaserver_link_get_link_prompt(
				appaserver_link_file->
					link_prompt->
					prepend_http_boolean,
				appaserver_link_file->
					link_prompt->
					http_prefix,
				appaserver_link_file->
					link_prompt->
					server_address,
				appaserver_link_file->application_name,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension );

	liability_check_appaserver_link->ftp_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->
				server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	appaserver_link_file->extension = "pdf";
	appaserver_link_file->session = session_key;
	appaserver_link_file->process_id = 0;

	liability_check_appaserver_link->document_root_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	return liability_check_appaserver_link;
}

LIABILITY_CHECK_APPASERVER_LINK *
	liability_check_appaserver_link_calloc(
			void )
{
	LIABILITY_CHECK_APPASERVER_LINK *
		liability_check_appaserver_link;

	if ( ! ( liability_check_appaserver_link =
			calloc(
				1,
				sizeof( LIABILITY_CHECK_APPASERVER_LINK ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_check_appaserver_link;
}

LIABILITY_ACCOUNT *liability_account_getset(
			LIST *list,
			char *account_name )
{
	LIABILITY_ACCOUNT *liability_account;

	if ( !list
	||   !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( list ) )
	{
		liability_account =
			liability_account_new(
				account_name );

		list_set(
			list,
			liability_account );

		return liability_account;
	}

	do {
		liability_account = list_get( list );

		if ( strcmp(
			account_name,
			liability_account->account_name ) == 0 )
		{
			return liability_account;
		}

	} while ( list_next( list ) );

	liability_account =
		liability_account_new(
			account_name );

	list_set(
		list,
		liability_account );

	return liability_account;
}

LIABILITY_ACCOUNT *liability_account_new( char *account_name )
{
	LIABILITY_ACCOUNT *liability_account;

	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_account = liability_account_calloc();
	liability_account->account_name = account_name;

	return liability_account;
}

LIABILITY_ACCOUNT *liability_account_calloc( void )
{
	LIABILITY_ACCOUNT *liability_account;

	if ( ! ( liability_account =
			calloc( 1, sizeof( LIABILITY_ACCOUNT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_account;
}

LIABILITY_ACCOUNT_LIST *liability_account_list_new(
			LIST *journal_system_list )
{
	LIABILITY_ACCOUNT_LIST *liability_account_list;
	JOURNAL *journal;
	LIABILITY_ACCOUNT *liability_account;

	if ( !list_rewind( journal_system_list ) )
	{
		return (LIABILITY_ACCOUNT_LIST *)0;
	}

	liability_account_list = liability_account_list_calloc();
	liability_account_list->list = list_new();

	do {
		journal = list_get( journal_system_list );

		liability_account =
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			liability_account_getset(
				liability_account_list->list,
				journal->account_name );

		if ( journal->credit_amount )
		{
			liability_account->credit_amount +=
				journal->credit_amount;
		}
		else
		{
			liability_account->credit_amount -=
				journal->debit_amount;
		}

	} while( list_next( journal_system_list ) );

	return liability_account_list;
}

LIABILITY_ACCOUNT_LIST *
	liability_account_list_calloc(
			void )
{
	LIABILITY_ACCOUNT_LIST *liability_account_list;

	if ( ! ( liability_account_list =
			calloc( 1,
				sizeof( LIABILITY_ACCOUNT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_account_list;
}

LIABILITY_CALCULATE *liability_calculate_new( char *application_name )
{
	LIABILITY_CALCULATE *liability_calculate;

	if ( !application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_calculate = liability_calculate_calloc();

	liability_calculate->liability_account_entity_list =
		liability_account_entity_list();

	liability_calculate->exclude_account_name_list =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		liability_account_entity_account_name_list(
			liability_calculate->
				liability_account_entity_list );

	list_set(
		liability_calculate->exclude_account_name_list,
		ACCOUNT_UNCLEARED_CHECKS );

	liability_calculate->account_current_liability_name_list =
		account_current_liability_name_list(
			ACCOUNT_TABLE,
			SUBCLASSIFICATION_CURRENT_LIABILITY,
			ACCOUNT_CREDIT_CARD_KEY,
			liability_calculate->exclude_account_name_list );

	liability_calculate->journal_account_distinct_entity_list =
		journal_account_distinct_entity_list(
			JOURNAL_TABLE,
			liability_calculate->
				account_current_liability_name_list
					/* account_name_list */ );

	liability_calculate->account_receivable_name_list =
		account_receivable_name_list(
			ACCOUNT_TABLE,
			SUBCLASSIFICATION_RECEIVABLE );

	if ( ! ( liability_calculate->entity_self = entity_self_fetch() ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: entity_self_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_calculate->liability_entity_list = list_new();

	if ( list_length(
		liability_calculate->
			liability_account_entity_list ) )
	{
		list_set_list(
			liability_calculate->liability_entity_list,
			liability_entity_list_account(
				liability_calculate->
					liability_account_entity_list ) );
	}

	if ( list_length(
		liability_calculate->
			journal_account_distinct_entity_list ) )
	{
		list_set_list(
			liability_calculate->liability_entity_list,
			liability_entity_list_entity(
				liability_calculate->
					account_current_liability_name_list,
				liability_calculate->
					journal_account_distinct_entity_list,
				liability_calculate->
					account_receivable_name_list,
				liability_calculate->
					entity_self ) );
	}

	if ( !list_length( liability_calculate->liability_entity_list ) )
	{
		free( liability_calculate );
		return (LIABILITY_CALCULATE *)0;
	}
	else
	{
		return liability_calculate;
	}
}

LIABILITY_CALCULATE *liability_calculate_calloc( void )
{
	LIABILITY_CALCULATE *liability_calculate;

	if ( ! ( liability_calculate =
			calloc( 1, sizeof( LIABILITY_CALCULATE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_calculate;
}

LIABILITY_JOURNAL_LIST *liability_journal_list_new(
			double dialog_box_payment_amount,
			char *liability_payment_credit_account_name,
			LIABILITY_ENTITY *liability_entity )
{
	LIABILITY_JOURNAL_LIST *liability_journal_list;
	LIABILITY_ACCOUNT *liability_account;

	if ( !liability_payment_credit_account_name
	||   !liability_entity
	||   !liability_entity->liability
	||   !liability_entity->liability->liability_account_list
	||   !list_rewind(
		liability_entity->
			liability->
			liability_account_list->
			list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_journal_list = liability_journal_list_calloc();
	liability_journal_list->journal_list = list_new();

	liability_journal_list->transaction_amount =
		liability_journal_list_transaction_amount(
			dialog_box_payment_amount,
			liability_entity->amount_due );

	if ( !liability_journal_list->transaction_amount )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: liability_journal_list->transaction_amount is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_set(
		liability_journal_list->journal_list,
		journal_credit_new(
			liability_payment_credit_account_name,
			liability_journal_list->
				transaction_amount ) );

	do {
		liability_account =
			list_get(
				liability_entity->
					liability->
					liability_account_list->
					list );

		if ( !liability_account->credit_amount ) continue;

		list_set(
			liability_journal_list->journal_list,
			journal_debit_new(
				liability_account->account_name,
				(dialog_box_payment_amount)
					? dialog_box_payment_amount
					: liability_account->credit_amount ) );

		if ( liability_entity->receivable
		&&   liability_entity->receivable->receivable_account_list
		&&   list_rewind(
			liability_entity->
				receivable->
				receivable_account_list->
				list ) )
		{
			RECEIVABLE_ACCOUNT *receivable_account;

			do {
				receivable_account =
					list_get(
						liability_entity->
						       receivable->
						       receivable_account_list->
						       list );

				list_set(
					liability_journal_list->journal_list,
					journal_credit_new(
						receivable_account->
							account_name,
						receivable_account->
							debit_amount ) );

			} while ( list_next(
					liability_entity->
						receivable->
						receivable_account_list->
						list ) );
		}

	} while ( list_next(
			liability_entity->
				liability->
				liability_account_list->
				list ) );

	return liability_journal_list;
}

LIABILITY_JOURNAL_LIST *liability_journal_list_calloc( void )
{
	LIABILITY_JOURNAL_LIST *liability_journal_list;

	if ( ! ( liability_journal_list =
			calloc( 1, sizeof( LIABILITY_JOURNAL_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_journal_list;
}

double liability_journal_list_transaction_amount(
			double dialog_box_payment_amount,
			double liability_entity_amount_due )
{
	if ( dialog_box_payment_amount )
		return dialog_box_payment_amount;
	else
		return liability_entity_amount_due;
}

LIABILITY_TRANSACTION_LIST *
	liability_transaction_list_new(
			double dialog_box_payment_amount,
			int starting_check_number,
			char *transaction_memo,
			LIST *liability_payment_entity_list,
			char *credit_account_name )
{
	LIABILITY_TRANSACTION_LIST *liability_transaction_list;
	LIABILITY_ENTITY *liability_entity;

	if ( dialog_box_payment_amount
	&& list_length( liability_payment_entity_list ) > 1 )
	{
		return (LIABILITY_TRANSACTION_LIST *)0;
	}

	if ( !list_rewind( liability_payment_entity_list ) )
	{
		return (LIABILITY_TRANSACTION_LIST *)0;
	}

	liability_transaction_list = liability_transaction_list_calloc();

	liability_transaction_list->list = list_new();

	liability_transaction_list->transaction_date_time =
		date_now_new(
			date_utc_offset() );

	do {
		liability_entity = list_get( liability_payment_entity_list );

		list_set(
			liability_transaction_list->list,
			liability_transaction_new(
				dialog_box_payment_amount,
				starting_check_number
					/* check_number */,
				transaction_memo,
				credit_account_name,
				liability_entity,
				liability_transaction_list->
					transaction_date_time ) );

		if ( starting_check_number )
		{
			starting_check_number++;
		}

		date_increment_seconds(
			liability_transaction_list->transaction_date_time,
			1 );

	} while ( list_next( liability_payment_entity_list ) );

	return liability_transaction_list;
}

LIABILITY_TRANSACTION_LIST *
	liability_transaction_list_calloc(
			void )
{
	LIABILITY_TRANSACTION_LIST *liability_transaction_list;

	if ( ! ( liability_transaction_list =
			calloc( 1, sizeof( LIABILITY_TRANSACTION_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_transaction_list;
}

LIABILITY_TRANSACTION *liability_transaction_new(
			double dialog_box_payment_amount,
			int check_number,
			char *transaction_memo,
			char *credit_account_name,
			LIABILITY_ENTITY *liability_entity,
			DATE *transaction_date_time )
{
	LIABILITY_TRANSACTION *liability_transaction;

	if ( !transaction_memo
	||   !credit_account_name
	||   !liability_entity
	||   !liability_entity->entity
	||   !transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_transaction = liability_transaction_calloc();

	liability_transaction->liability_journal_list =
		liability_journal_list_new(
			dialog_box_payment_amount,
			credit_account_name,
			liability_entity );

	if ( !liability_transaction->liability_journal_list
	||   !liability_transaction->
		liability_journal_list->
		transaction_amount )
	{
		free( liability_transaction );
		return (LIABILITY_TRANSACTION *)0;
	}

	liability_transaction->transaction =
		transaction_entity_new(
			liability_entity->entity,
			date_display_19( transaction_date_time ),
			liability_transaction->
				liability_journal_list->
				transaction_amount,
			check_number,
			(*transaction_memo)
				? transaction_memo
				: LIABILITY_TRANSACTION_MEMO,
			liability_transaction->
				liability_journal_list->
				journal_list );

	return liability_transaction;
}

LIABILITY_TRANSACTION *liability_transaction_calloc( void )
{
	LIABILITY_TRANSACTION *liability_transaction;

	if ( ! ( liability_transaction =
			calloc( 1, sizeof( LIABILITY_TRANSACTION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_transaction;
}

void liability_transaction_list_insert(
			LIABILITY_TRANSACTION_LIST *
				liability_transaction_list )
{
	if ( !liability_transaction_list )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: liability_transaction_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( liability_transaction_list->list ) ) return;

	/* May reset transaction_date_time */
	/* ------------------------------- */
	transaction_list_insert(
		liability_transaction_list_extract(
			liability_transaction_list->list ),
		1 /* insert_journal_list_boolean */ );
}

LIST *liability_transaction_list_extract( LIST *list )
{
	LIST *transaction_list;
	LIABILITY_TRANSACTION *liability_transaction;

	if ( !list_rewind( list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		liability_transaction = list_get( list );

		list_set(
			transaction_list,
			liability_transaction->transaction );

	} while ( list_next( list ) );

	return transaction_list;
}

void liability_transaction_list_html_display(
			LIABILITY_TRANSACTION_LIST *
				liability_transaction_list )
{
	if ( !liability_transaction_list )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: liability_transaction_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( liability_transaction_list->list ) ) return;

	transaction_list_html_display(
		liability_transaction_list_extract(
			liability_transaction_list->list ) );
}

void liability_calculate_stdout( LIST *liability_entity_list )
{
	LIABILITY_ENTITY *liability_entity;

	if ( !list_rewind( liability_entity_list ) ) return;

	do {
		liability_entity =
			list_get(
				liability_entity_list );

		if ( liability_entity->amount_due > 0.0 )
		{
			printf(	"%s\n",
				liability_entity_display(
					liability_entity ) );
		}

	} while ( list_next( liability_entity_list ) );
}

char *liability_entity_display( LIABILITY_ENTITY *liability_entity )
{
	static char display[ 128 ];

	if ( !liability_entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: liability_entity is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(display,
		"%s^%s [%.2lf]",
		liability_entity->
			entity->
			full_name,
		liability_entity->
			entity->
			street_address,
		liability_entity->amount_due );

	return display;
}

LIABILITY_CHECK_LIST *liability_check_list_calloc( void )
{
	LIABILITY_CHECK_LIST *liability_check_list;

	if ( ! ( liability_check_list =
			calloc( 1, sizeof( LIABILITY_CHECK_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_check_list;
}

char *liability_check_vendor_name_amount_due_display(
			char *escape_payable_to,
			char *amount_due_display )
{
	static char display[ 256 ];

	if ( !escape_payable_to
	||   !amount_due_display )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(display,
"\\begin{tabular}{p{0.2in}p{2.5in}p{4.1in}l}\n"
"& %.26s & %s & %s\n"
"\\end{tabular}\n\n",
		escape_payable_to,
		escape_payable_to,
		amount_due_display );

	return display;
}

char *liability_check_escape_payable_to( char *full_name )
{
	static char escape_payable_to[ 256 ];

	if ( !full_name || !*full_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter_is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	latex_column_data_escape(
		escape_payable_to /* destination */,
		full_name /* source */,
		256 );
}

char *liability_payment_error_message( char *message )
{
	return message;
}

LIST *liability_entity_list_account(
			LIST *liability_account_entity_list )
{
	LIST *liability_entity_list;
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;
	LIABILITY_ENTITY *liability_entity;

	if ( !list_rewind( liability_account_entity_list ) )
		return (LIST *)0;

	liability_entity_list = list_new();

	do {
		liability_account_entity =
			list_get(
				liability_account_entity_list );

		if ( !liability_account_entity->account_name
		||   !liability_account_entity->entity )
		{
			continue;
		}

		liability_entity =
			liability_entity_account_name_new(
				liability_account_entity->account_name,
				liability_account_entity->entity );

		if ( liability_entity )
		{
			list_set(
				liability_entity_list,
				liability_entity );
		}

	} while ( list_next( liability_account_entity_list ) );

	return liability_entity_list;
}

LIST *liability_entity_list_entity(
			LIST *account_current_liability_name_list,
			LIST *journal_account_distinct_entity_list,
			LIST *account_receivable_name_list,
			ENTITY_SELF *entity_self )
{
	ENTITY *entity;
	LIABILITY_ENTITY *liability_entity;
	LIST *liability_entity_list;

	if ( !entity_self
	||   !entity_self->entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: entity_self is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( account_current_liability_name_list )
	||   !list_rewind( journal_account_distinct_entity_list ) )
	{
		return (LIST *)0;
	}

	liability_entity_list = list_new();

	do {
		entity =
			list_get(
				journal_account_distinct_entity_list );

		if ( strcmp(
			entity->full_name,
			entity_self->
				entity->
				full_name ) == 0
		&&   strcmp(
			entity->street_address,
			entity_self->
				entity->
				street_address ) == 0 )
		{
			continue;
		}

		if ( ( liability_entity =
				liability_entity_account_list_new(
					account_current_liability_name_list,
					account_receivable_name_list,
					entity ) ) )
		{
			list_set( liability_entity_list, liability_entity );
		}

	} while ( list_next( journal_account_distinct_entity_list ) );

	return liability_entity_list;
}

LIABILITY_ENTITY *liability_entity_seek(
			char *full_name,
			char *street_address,
			LIST *liability_entity_list )
{
	LIABILITY_ENTITY *liability_entity;

	if ( !list_rewind( liability_entity_list ) )
		return (LIABILITY_ENTITY *)0;

	do {
		liability_entity =
			list_get(
				liability_entity_list );

		if ( !liability_entity->entity )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: liability_entity->entity is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(
			full_name,
			liability_entity->entity->full_name ) == 0
		&&   strcmp(
			street_address,
			liability_entity->entity->street_address ) == 0 )
		{
			return liability_entity;
		}

	} while ( list_next( liability_entity_list ) );

	return (LIABILITY_ENTITY *)0;
}

LIST *liability_payment_entity_list(
			LIST *entity_full_street_list,
			LIST *liability_entity_list )
{
	ENTITY *entity;
	LIABILITY_ENTITY *liability_entity;
	LIST *entity_list;

	if ( !list_rewind( entity_full_street_list ) ) return (LIST *)0;

	entity_list = list_new();

	do {
		entity = list_get( entity_full_street_list );

		if ( ! ( liability_entity =
				liability_entity_seek(
					entity->full_name,
					entity->street_address,
					liability_entity_list ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: liability_entity_seek(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				entity->full_name );
			exit( 1 );
		}

		list_set(
			entity_list,
			liability_entity );

	} while ( list_next( entity_full_street_list ) );

	return entity_list;
}

double liability_check_amount(
			double dialog_box_payment_amount,
			double liability_entity_amount_due )
{
	if ( dialog_box_payment_amount )
		return dialog_box_payment_amount;
	else
		return liability_entity_amount_due;
}
