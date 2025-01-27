/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/check.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "date.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "application.h"
#include "latex.h"
#include "dollar.h"
#include "liability.h"
#include "check.h"

CHECK *check_calloc( void )
{
	CHECK *check;

	if ( ! ( check = calloc( 1, sizeof ( CHECK ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return check;
}

CHECK_LIST *check_list_new(
		char *application_name,
		double dialog_box_payment_amount,
		int starting_check_number,
		char *transaction_memo,
		char *data_directory,
		char *process_name,
		char *session_key,
		LIST *liability_payment_entity_list )
{
	CHECK_LIST *check_list;
	LIABILITY_ENTITY *liability_entity;
	CHECK *check;
	FILE *output_file;

	if ( !application_name
	||   !data_directory
	||   !process_name
	||   !session_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( liability_payment_entity_list ) ) return NULL;

	check_list = check_list_calloc();

	check_list->documentclass =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		check_list_documentclass();

	check_list->usepackage =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		check_list_usepackage();

	check_list->pagenumbering_gobble =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		check_list_pagenumbering_gobble();

	check_list->begin_document =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		check_list_begin_document();

	check_list->heading =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		check_list_heading(
			check_list->documentclass,
			check_list->usepackage,
			check_list->pagenumbering_gobble,
			check_list->begin_document );

	check_list->list = list_new();

	do {
		liability_entity =
			list_get(
				liability_payment_entity_list );

		list_set(
			check_list->list,
			check_new(
				dialog_box_payment_amount,
				starting_check_number++
					/* check_number */,
				transaction_memo,
				liability_entity->amount_due,
				liability_entity->entity->full_name ) );

	} while ( list_next( liability_payment_entity_list ) );

	if ( !list_length( check_list->list ) )
	{
		char message[ 128 ];

		sprintf(message, "check_list->list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	check_list->end_document =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		check_list_end_document();

	check_list->check_link =
		check_link_new(
			application_name,
			process_name,
			data_directory,
			session_key );

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file( 
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			latex_directory_filename(
				check_list->
					check_link->
					tex_filename,
				check_list->
					check_link->
					appaserver_link_working_directory ) );

	fprintf(output_file,
		"%s\n",
		check_list->heading );

	list_rewind( check_list->list );

	do {
		check = list_get( check_list->list );

		if ( !check->output_string )
		{
			fclose( output_file );

			fprintf(stderr,
			"ERROR in %s/%s()/%d: check->output_string is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		fprintf(output_file,
			"%s\n",
			check->output_string );

	} while ( list_next( check_list->list ) );

	fprintf(output_file,
		"%s\n",
		check_list->end_document );

	fclose( output_file );

	latex_tex2pdf(
		check_list->
			check_link->
			tex_filename,
		check_list->
			check_link->
			appaserver_link_working_directory );

	return check_list;
}

CHECK *check_new(
		double dialog_box_payment_amount,
		int check_number,
		char *transaction_memo,
		double liability_entity_amount_due,
		char *entity_full_name )
{
	CHECK *check;
	char *commas_money;

	if ( !check_number
	||   !liability_entity_amount_due
	||   !entity_full_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	check = check_calloc();

	check->amount =
		check_amount(
			dialog_box_payment_amount,
			liability_entity_amount_due );

	if ( check->amount <= 0.0 )
	{
		char message[ 128 ];

		sprintf(message,
		"check_amount(%.2lf,%.2lf) returned invalid amount=%.2lf.",
			dialog_box_payment_amount,
			liability_entity_amount_due,
			check->amount );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	check->dollar_text =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_dollar_text(
			check->amount );

	check->escape_payable_to =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_escape_payable_to(
			entity_full_name );

	check->move_down =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		check_move_down();

	check->date_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_date_display(
			string_pipe_fetch(
				CHECK_DATE_COMMAND )
					/* check_date */ );

	commas_money =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			check->amount );

	check->vendor_name_amount_due_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_vendor_name_amount_due_display(
			check->escape_payable_to,
			commas_money );

	check->amount_due_stub_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_amount_due_stub_display(
			commas_money );

	check->dollar_text_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_dollar_text_display(
			check->dollar_text );

	check->memo_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_memo_display( transaction_memo );

	check->number_display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_number_display( check_number );

	check->newpage =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		check_newpage();

	check->output_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		check_output_string(
			check->move_down,
			check->date_display,
			check->vendor_name_amount_due_display,
			check->amount_due_stub_display,
			check->dollar_text_display,
			check->memo_display,
			check->number_display,
			check->newpage );

	return check;
}

char *check_list_documentclass( void )
{
	return
"\\documentclass{report}";
}

char *check_list_usepackage( void )
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

char *check_list_pagenumbering_gobble( void )
{
	return
"\\pagenumbering{gobble}";
}

char *check_list_begin_document( void )
{
	return
"\\begin{document}";
}

char *check_newpage( void )
{
	return
"\\newpage\n";
}

char *check_list_end_document( void )
{
	return
"\\end{document}";
}

char *check_dollar_text(
			double amount_due )
{
	static char text[ 128 ];

	if ( amount_due <= 0.0 )
	{
		char message[ 128 ];

		sprintf(message,
			"invalid amount_due=%.2lf.",
			amount_due );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	dollar_text( text, amount_due );

	return text;
}

char *check_move_down( void )
{
	return
"\\begin{tabular}l\n"
"\\end{tabular}\n\n"
"\\vspace{0.45in}";
}

char *check_date_display( char *check_date )
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

char *check_vendor_amount_due_display(
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

char *check_amount_due_stub_display( char *amount_due_display )
{
	static char display[ 128 ];

	if ( !amount_due_display )
	{
		char message[ 128 ];

		sprintf(message, "amount_due_display is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(display,
"\\vspace{0.10in}\n\n"
"\\begin{tabular}{p{0.2in}l}\n"
"& %s\n"
"\\end{tabular}",
		amount_due_display );

	return display;
}

char *check_dollar_text_display( char *dollar_text )
{
	static char display[ 256 ];

	if ( !dollar_text )
	{
		char message[ 128 ];

		sprintf(message, "dollar_text is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(display,
"\\vspace{0.10in}\n\n"
"\\begin{tabular}{p{0.2in}p{2.5in}l}\n"
"& & %s\n"
"\\end{tabular}\n",
		dollar_text );

	return display;
}

char *check_memo_display( char *transaction_memo )
{
	static char display[ 256 ];

	if ( transaction_memo && *transaction_memo )
	{
		sprintf(display,
"\\vspace{0.20in}\n\n"
"\\begin{tabular}{p{0.2in}p{2.5in}l}\n"
"& %s & %s\n"
"\\end{tabular}\n\n",
			/* ----------------------------------------- */
			/* Returns transaction_memo or static memory */
			/* ----------------------------------------- */
			check_stub_memo(
				transaction_memo,
				30 /* max_length */ ),
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

char *check_number_display( int check_number )
{
	static char display[ 128 ];

	if ( !check_number )
	{
		char message[ 128 ];

		sprintf(message, "check_number is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(display,
"\\vspace{0.05in}\n\n"
"\\begin{tabular}{p{0.2in}l}\n"
"& Check: %d\n"
"\\end{tabular}\n",
		check_number );

	return display;
}

char *check_output_string(
		char *check_move_down,
		char *check_date_display,
		char *check_vendor_name_amount_due_display,
		char *check_amount_due_stub_display,
		char *check_dollar_text_display,
		char *check_memo_display,
		char *check_number_display,
		char *check_newpage )
{
	char output_string[ 4096 ];

	if ( !check_move_down
	||   !check_date_display
	||   !check_vendor_name_amount_due_display
	||   !check_amount_due_stub_display
	||   !check_dollar_text_display
	||   !check_memo_display
	||   !check_number_display
	||   !check_newpage )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(output_string,
		"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
		check_move_down,
		check_date_display,
		check_vendor_name_amount_due_display,
		check_amount_due_stub_display,
		check_dollar_text_display,
		check_memo_display,
		check_number_display,
		check_newpage );

	return strdup( output_string );
}

char *check_list_heading(
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
		"%s\n%s\n%s\n%s",
		documentclass,
		usepackage,
		pagenumbering_gobble,
		begin_document );

	return strdup( heading );
}

CHECK_LINK *check_link_new(
		char *application_name,
		char *process_name,
		char *data_directory,
		char *session_key )
{
	CHECK_LINK *check_link;

	if ( !application_name
	||   !process_name
	||   !data_directory
	||   !session_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	check_link = check_link_calloc();

	check_link->appaserver_link =
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			application_server_address(),
			data_directory,
			process_name /* filename_stem */,
			application_name,
			(pid_t)0 /* process_id */,
			session_key,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
		 	(char *)0 /* extension */ );

	check_link->appaserver_link->extension = "tex";

	check_link->tex_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			check_link->appaserver_link->filename_stem,
			check_link->
				appaserver_link->
				begin_date_string,
			check_link->
				appaserver_link->
				end_date_string,
			check_link->appaserver_link->process_id,
			check_link->appaserver_link->session_key,
			check_link->appaserver_link->extension );

	check_link->appaserver_link->extension = "pdf";

	check_link->appaserver_link->appaserver_link_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_prompt_new(
			APPASERVER_LINK_PROMPT_DIRECTORY
				/* probably appaserver_data */,
			check_link->appaserver_link->http_prefix,
			check_link->appaserver_link->server_address,
			application_name,
			check_link->appaserver_link->filename_stem,
			check_link->
				appaserver_link->
				begin_date_string,
			check_link->
				appaserver_link->
				end_date_string,
			check_link->appaserver_link->process_id,
			check_link->appaserver_link->session_key,
			check_link->appaserver_link->extension );

	check_link->pdf_anchor_html =
		appaserver_link_anchor_html(
			check_link->
				appaserver_link->
				appaserver_link_prompt->
				filename /* prompt_filename */,
			process_name /* target_window */,
			CHECK_PROMPT );

	check_link->appaserver_link_working_directory =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_working_directory(
			data_directory,
			application_name );

	return check_link;
}

CHECK_LINK *check_link_calloc( void )
{
	CHECK_LINK *check_link;

	if ( ! ( check_link =
			calloc(	1,
				sizeof ( CHECK_LINK ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return check_link;
}

CHECK_LIST *check_list_calloc( void )
{
	CHECK_LIST *check_list;

	if ( ! ( check_list = calloc( 1, sizeof ( CHECK_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return check_list;
}

char *check_vendor_name_amount_due_display(
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

char *check_escape_payable_to( char *full_name )
{
	if ( !full_name || !*full_name )
	{
		char message[ 128 ];

		sprintf(message, "full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	latex_cell_escape( full_name /* datum */ );
}

double check_amount(
		double dialog_box_payment_amount,
		double liability_entity_amount_due )
{
	double amount;

	if ( dialog_box_payment_amount )
		amount = dialog_box_payment_amount;
	else
		amount = liability_entity_amount_due;

	return amount;
}

char *check_stub_memo(
		char *transaction_memo,
		int max_length )
{
	static char memo[ 128 ];

	if ( string_strlen( transaction_memo ) > max_length )
	{
		string_strncpy( memo, transaction_memo, max_length );
		return memo;
	}
	else
	{
		return transaction_memo;
	}
}
