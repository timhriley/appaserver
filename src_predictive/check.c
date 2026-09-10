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
				/* ------------------------ */
				/* Returns either parameter */
				/* ------------------------ */
				check_list_memo(
					transaction_memo,
					liability_entity->
						liability->
						journal_list_last_memo )
					/* transaction_memo */,
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

	check->move_down_latex =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		check_move_down_latex();

	check->date_latex =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_date_latex(
			string_pipe_fetch(
				CHECK_DATE_COMMAND )
					/* check_date */ );

	commas_money =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			check->amount );

	check->vendor_amount_due_latex =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_vendor_amount_due_latex(
			check->escape_payable_to,
			commas_money );

	check->amount_due_stub_latex =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_amount_due_stub_latex(
			commas_money );

	check->dollar_text_latex =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_dollar_text_latex(
			check->dollar_text );

	check->memo_stub_trim =
		check_memo_stub_trim(
			CHECK_STUB_MEMO_MAX_LENGTH,
			transaction_memo );

	check->memo_stub_latex =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_memo_stub_latex( check->memo_stub_trim );

	check->number_stub_latex =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_number_stub_latex( check_number );

	check->memo_latex =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		check_memo_latex( transaction_memo );

	check->newpage_latex =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		check_newpage_latex();

	check->output_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		check_output_string(
			check->move_down_latex,
			check->date_latex,
			check->vendor_amount_due_latex,
			check->amount_due_stub_latex,
			check->dollar_text_latex,
			check->memo_stub_latex,
			check->number_stub_latex,
			check->memo_latex,
			check->newpage_latex );

	return check;
}

char *check_list_documentclass( void )
{
	return latex_documentclass( 0 /* not landscape */ );
/*
"\\documentclass{report}";
*/
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

char *check_newpage_latex( void )
{
	return
"\\newpage\n";
}

char *check_list_end_document( void )
{
	return
"\\end{document}";
}

char *check_dollar_text( double amount_due )
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

char *check_move_down_latex( void )
{
	return
"\\begin{tabular}l\n"
"\\end{tabular}\n\n"
"\\vspace{0.45in}";
}

char *check_date_latex( char *check_date )
{
	static char date_latex[ 256 ];

	if ( !check_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: check_date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		date_latex,
		sizeof ( date_latex ),
"\\begin{tabular}{p{0.2in}p{6.6in}l}\n"
"& %s & %s\n"
"\\end{tabular}\n\n",
		check_date,
		check_date );

	return date_latex;
}

char *check_vendor_amount_due_latex(
		char *payable_to,
		char *commas_money )
{
	static char latex[ 256 ];

	if ( !payable_to
	||   !commas_money )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		latex,
		sizeof ( latex ),
"\\begin{tabular}{p{0.2in}p{2.5in}p{4.1in}l}\n"
"& %.26s & %s & %s\n"
"\\end{tabular}\n",
		payable_to,
		payable_to,
		commas_money );

	return latex;
}

char *check_amount_due_stub_latex( char *commas_money )
{
	static char latex[ 128 ];

	if ( !commas_money )
	{
		char message[ 128 ];

		sprintf(message, "commas_money is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		latex,
		sizeof ( latex ),
"\\vspace{0.10in}\n\n"
"\\begin{tabular}{p{0.2in}l}\n"
"& %s\n"
"\\end{tabular}",
		commas_money );

	return latex;
}

char *check_dollar_text_latex( char *dollar_text )
{
	static char latex[ 256 ];

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

	snprintf(
		latex,
		sizeof ( latex ),
"\\vspace{0.10in}\n\n"
"\\begin{tabular}{p{0.2in}p{2.5in}l}\n"
"& & %s\n"
"\\end{tabular}\n",
		dollar_text );

	return latex;
}

char *check_memo_stub_latex( char *stub_trim )
{
	static char latex[ 256 ];

	if ( stub_trim && *stub_trim )
	{
		snprintf(
			latex,
			sizeof ( latex ),
"\\vspace{0.20in}\n\n"
"\\begin{tabular}{p{0.2in}l}\n"
"& %s\n"
"\\end{tabular}\n\n",
			stub_trim );
	}
	else
	{
		snprintf(
			latex,
			sizeof ( latex ),
"\\vspace{0.20in}\n\n"
"\\begin{tabular}l\n"
"\\end{tabular}\n\n" );
	}

	return latex;
}

char *check_memo_latex( char *transaction_memo )
{
	static char latex[ 256 ];

	if ( transaction_memo && *transaction_memo )
	{
		snprintf(
			latex,
			sizeof ( latex ),
"\\begin{tabular}{p{0.2in}p{2.5in}l}\n"
"& & %s\n"
"\\end{tabular}\n\n",
			transaction_memo );
	}
	else
	{
		snprintf(
			latex,
			sizeof ( latex ),
"\\begin{tabular}l\n"
"\\end{tabular}\n\n" );
	}

	return latex;
}

char *check_number_stub_latex( int check_number )
{
	static char latex[ 128 ];

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

	snprintf(
		latex,
		sizeof ( latex ),
"\\vspace{0.05in}\n\n"
"\\begin{tabular}{p{0.2in}l}\n"
"& Check: %d\n"
"\\end{tabular}\n",
		check_number );

	return latex;
}

char *check_output_string(
		char *check_move_down_latex,
		char *check_date_latex,
		char *check_vendor_amount_due_latex,
		char *check_amount_due_stub_latex,
		char *check_dollar_text_latex,
		char *check_memo_stub_latex,
		char *check_number_stub_latex,
		char *check_memo_latex,
		char *check_newpage_latex )
{
	char output_string[ 4096 ];

	if ( !check_move_down_latex
	||   !check_date_latex
	||   !check_vendor_amount_due_latex
	||   !check_amount_due_stub_latex
	||   !check_dollar_text_latex
	||   !check_memo_stub_latex
	||   !check_number_stub_latex
	||   !check_memo_latex
	||   !check_newpage_latex )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		output_string,
		sizeof ( output_string ),
		"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
		check_move_down_latex,
		check_date_latex,
		check_vendor_amount_due_latex,
		check_amount_due_stub_latex,
		check_dollar_text_latex,
		check_memo_stub_latex,
		check_number_stub_latex,
		check_memo_latex,
		check_newpage_latex );

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
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
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

char *check_memo_stub_trim(
		int max_length,
		char *transaction_memo )
{
	static char trim[ 128 ];

	if ( string_strlen( transaction_memo ) > max_length )
	{
		string_strncpy( trim, transaction_memo, max_length );
		return trim;
	}
	else
	{
		return transaction_memo;
	}
}

char *check_list_memo(
		char *transaction_memo,
		char *journal_list_last_memo )
{
	if ( transaction_memo && *transaction_memo )
		return transaction_memo;
	else
		return journal_list_last_memo;
}
