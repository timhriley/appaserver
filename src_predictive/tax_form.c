/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax_form.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "tax_form.h"

TAX_FORM_ENTITY *tax_form_entity_calloc( void )
{
	TAX_FORM_ENTITY *tax_form_entity;

	if ( ! ( tax_form_entity = calloc( 1, sizeof( TAX_FORM_ENTITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return tax_form_entity;
}

LIST *tax_form_entity_list( LIST *journal_tax_form_list )
{
	JOURNAL *journal;
	LIST *list;

	if ( !list_rewind( journal_tax_form_list ) ) return (LIST *)0;

	list = list_new();

	do {
		journal = list_get( journal_tax_form_list );

		if ( !journal->account
		||   !journal->account->subclassification
		||   !journal->account->subclassification->element )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: element is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		tax_form_entity_getset(
			list,
			journal->full_name,
			journal_amount(
				journal->debit_amount,
				journal->credit_amount,
				journal->
					account->
					subclassification->
					element->
					accumulate_debit ) );

	} while ( list_next( journal_tax_form_list ) );

	return list;
}

void tax_form_entity_getset(
			LIST *list,
			char *full_name,
			double journal_amount )
{
	TAX_FORM_ENTITY *tax_form_entity;

	if ( !list
	||   !full_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !journal_amount ) return;

	if ( list_rewind( list ) )
	{
		do {
			tax_form_entity = list_get( list );

			if ( strcmp(
				tax_form_entity->entity->full_name,
				full_name ) == 0 )
			{
				tax_form_entity->total += journal_amount;
				return;
			}
		} while( list_next( list ) );
	}

	tax_form_entity = tax_form_entity_calloc();

	tax_form_entity->entity =
		entity_new(
			full_name,
			(char *)0 /* street_address */ );

	tax_form_entity->total = journal_amount;

	list_set( list, tax_form_entity );
}

LIST *tax_form_line_account_list(
			char *tax_form_name,
			char *tax_form_fiscal_begin_date,
			char *tax_form_fiscal_end_date,
			char *tax_form_line_string )
{
	char *where;

	if ( !tax_form_name
	||   !tax_form_fiscal_begin_date
	||   !tax_form_fiscal_end_date
	||   !tax_form_line_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		tax_form_line_where(
			tax_form_name,
			tax_form_line_string );

	return
	tax_form_line_account_system_list(
		tax_form_name,
		tax_form_fiscal_begin_date,
		tax_form_fiscal_end_date,
		tax_form_line_string,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		tax_form_line_account_system_string(
			TAX_FORM_LINE_ACCOUNT_SELECT,
			TAX_FORM_LINE_ACCOUNT_TABLE,
			where ) );
}

char *tax_form_line_account_system_string(
			char *select,
			char *table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !select
	||   !table
	||   !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" select",
		select,
		table,
		where );

	return strdup( system_string );
}

LIST *tax_form_line_account_system_list(
			char *tax_form_name,
			char *tax_form_fiscal_begin_date,
			char *tax_form_fiscal_end_date,
			char *tax_form_line_string,
			char *tax_form_line_account_system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *list;

	if ( !tax_form_name
	||   !tax_form_fiscal_begin_date
	||   !tax_form_fiscal_end_date
	||   !tax_form_line_string
	||   !tax_form_line_account_system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	input_pipe =
		tax_form_line_account_input_pipe(
			tax_form_line_account_system_string );

	list = list_new();

	while( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			tax_form_line_account_parse(
				tax_form_name,
				tax_form_fiscal_begin_date,
				tax_form_fiscal_end_date,
				tax_form_line_string,
				input ) );
	}

	pclose( input_pipe );

	return list;
}

FILE *tax_form_line_account_input_pipe(
			char *system_string )
{
	return popen( system_string, "r" );
}

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_parse(
			char *tax_form_name,
			char *tax_form_fiscal_begin_date,
			char *tax_form_fiscal_end_date,
			char *tax_form_line_string,
			char *input )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( !tax_form_name
	||   !tax_form_fiscal_begin_date
	||   !tax_form_fiscal_end_date
	||   !tax_form_line_string
	||   !input
	||   !*input )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	tax_form_line_account = tax_form_line_account_calloc();

	tax_form_line_account->tax_form_name = tax_form_name;
	tax_form_line_account->tax_form_line_string = tax_form_line_string;

	/* See TAX_FORM_LINE_ACCOUNT_SELECT */
	/* -------------------------------- */
	tax_form_line_account->account_name = strdup( input );

	tax_form_line_account->journal_tax_form_list =
		journal_tax_form_list(
			tax_form_fiscal_begin_date,
			tax_form_fiscal_end_date,
			TRANSACTION_PRECLOSE_TIME,
			tax_form_line_account->account_name );

	if ( !list_length( tax_form_line_account->journal_tax_form_list ) )
	{
		return (TAX_FORM_LINE_ACCOUNT *)0;
	}

	tax_form_line_account->total =
		tax_form_line_account_total(
			tax_form_line_account->
				journal_tax_form_list );

	if ( !tax_form_line_account->total )
	{
		return (TAX_FORM_LINE_ACCOUNT *)0;
	}

	tax_form_line_account->tax_form_entity_list =
		tax_form_entity_list(
			tax_form_line_account->
				journal_tax_form_list );

	return tax_form_line_account;
}

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_calloc( void )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( ! ( tax_form_line_account =
			calloc(	1,
				sizeof( TAX_FORM_LINE_ACCOUNT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_line_account;
}

double tax_form_line_account_total(
			LIST *journal_tax_form_list )
{
	if ( !list_length( journal_tax_form_list ) )
	{
		return 0.0;
	}
	else
	{
		JOURNAL *last_journal = list_last( journal_tax_form_list );
		return last_journal->balance;
	}
}

char *tax_form_line_where(
			char *tax_form_name,
			char *tax_form_line_string )
{
	static char where[ 128 ];

	if ( !tax_form_name
	||   !tax_form_line_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"tax_form = '%s' and tax_form_line = '%s'",
		tax_form_name,
		tax_form_line_string );

	return where;
}

LIST *tax_form_line_list(
			char *tax_form_name,
			char *tax_form_fiscal_begin_date,
			char *tax_form_fiscal_end_date )
{
	char *where;
	char *system_string;

	if ( !tax_form_name
	||   !tax_form_fiscal_begin_date
	||   !tax_form_fiscal_end_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		tax_form_primary_where(
			tax_form_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		tax_form_line_system_string(
			TAX_FORM_LINE_SELECT,
			TAX_FORM_LINE_TABLE,
			where );

	return
	tax_form_line_system_list(
		tax_form_name,
		tax_form_fiscal_begin_date,
		tax_form_fiscal_end_date,
		system_string );
}

char *tax_form_line_system_string(
			char *select,
			char *table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !select
	||   !table
	||   !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" select",
		select,
		table,
		where );

	return strdup( system_string );
}

LIST *tax_form_line_system_list(
			char *tax_form_name,
			char *tax_form_fiscal_begin_date,
			char *tax_form_fiscal_end_date,
			char *tax_form_line_system_string )
{
	FILE *input_pipe;
	LIST *list;
	char input[ 1024 ];

	if ( !tax_form_name
	||   !tax_form_fiscal_begin_date
	||   !tax_form_fiscal_end_date
	||   !tax_form_line_system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	input_pipe =
		tax_form_line_input_pipe(
			tax_form_line_system_string );

	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			tax_form_line_parse(
				tax_form_name,
				tax_form_fiscal_begin_date,
				tax_form_fiscal_end_date,
				input ) );
	}

	pclose( input_pipe );

	return list;
}

FILE *tax_form_line_input_pipe( char *system_string )
{
	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return popen( system_string, "r" );
}

TAX_FORM_LINE *tax_form_line_parse(
			char *tax_form_name,
			char *tax_form_fiscal_begin_date,
			char *tax_form_fiscal_end_date,
			char *input )
{
	TAX_FORM_LINE *tax_form_line;
	char buffer[ 128 ];

	if ( !tax_form_name
	||   !tax_form_fiscal_begin_date
	||   !tax_form_fiscal_end_date
	||   !input )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	tax_form_line = tax_form_line_calloc();

	tax_form_line->tax_form_name = tax_form_name;

	/* See TAX_FORM_LINE_SELECT */
	/* ------------------------ */
	piece( buffer, SQL_DELIMITER, input, 0 );
	tax_form_line->string = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	tax_form_line->tax_form_description = strdup( buffer );

	tax_form_line->tax_form_line_account_list =
		tax_form_line_account_list(
			tax_form_name,
			tax_form_fiscal_begin_date,
			tax_form_fiscal_end_date,
			tax_form_line->string );

	tax_form_line->total =
		tax_form_line_total(
			tax_form_line->
				tax_form_line_account_list );

	if ( !tax_form_line->total )
		return (TAX_FORM_LINE *)0;
	else
		return tax_form_line;
}

TAX_FORM_LINE *tax_form_line_calloc( void )
{
	TAX_FORM_LINE *tax_form_line;

	if ( ! ( tax_form_line =
			calloc( 1, sizeof( TAX_FORM_LINE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_line;
}

double tax_form_line_total( LIST *tax_form_line_account_list )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;
	double total;

	if ( !list_rewind( tax_form_line_account_list ) ) return 0.0;

	total = 0.0;

	do {
		tax_form_line_account =
			list_get(
				tax_form_line_account_list );

		total += tax_form_line_account->total;

	} while ( list_next( tax_form_line_account_list ) );

	return total;
}

char *tax_form_primary_where( char *tax_form_name )
{
	static char where[ 128 ];

	if ( !tax_form_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: tax_form_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"tax_form = '%s'",
		tax_form_name );

	return where;
}

TAX_FORM *tax_form_fetch(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			char *tax_form_name,
			int tax_year,
			int fiscal_begin_month /* zero or one based */,
			char *output_medium_string )
{
	TAX_FORM *tax_form;

	if ( !tax_form_name
	||   !tax_year )
	{
		return (TAX_FORM *)0;
	}

	tax_form = tax_form_calloc();

	tax_form->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	tax_form->fiscal_begin_date =
		tax_form_fiscal_begin_date(
			tax_year,
			fiscal_begin_month );

	tax_form->fiscal_end_date =
		tax_form_fiscal_end_date(
			tax_year,
			fiscal_begin_month );

	tax_form->statement_caption =
		statement_caption_new(
			application_name,
			process_name,
			tax_form->fiscal_begin_date,
			tax_form->fiscal_end_date );

	tax_form->tax_form_line_list =
		tax_form_line_list(
			tax_form_name,
			tax_form->fiscal_begin_date,
			tax_form->fiscal_end_date );

	if ( !list_length( tax_form->tax_form_line_list ) )
	{
		return tax_form;
	}

	if ( tax_form->statement_output_medium == statement_output_table )
	{
		tax_form->tax_form_table =
			tax_form_table_new(
				tax_form_name,
				tax_form->statement_caption,
				tax_form->tax_form_line_list );

		if ( !tax_form->tax_form_table )
		{
			return (TAX_FORM *)0;
		}
	}
	else
	if ( tax_form->statement_output_medium == statement_output_PDF )
	{
		tax_form->tax_form_pdf =
			tax_form_pdf_new(
				application_name,
				process_name,
				document_root_directory,
				tax_form_name,
				tax_form->statement_caption,
				tax_form->tax_form_line_list );

		if ( !tax_form->tax_form_pdf
		||   !tax_form->tax_form_pdf->latex )
		{
			return (TAX_FORM *)0;
		}
	}

	return tax_form;
}

TAX_FORM *tax_form_calloc( void )
{
	TAX_FORM *tax_form;

	if ( ! ( tax_form = calloc( 1, sizeof( TAX_FORM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form;
}

char *tax_form_fiscal_begin_date(
			int tax_year,
			int fiscal_begin_month )
{
	static char date[ 16 ];

	if ( fiscal_begin_month )
	{
		sprintf(date,
			"%d-%.2d-01",
			tax_year,
			fiscal_begin_month );
	}
	else
	{
		sprintf(date,
			"%d-01-01",
			tax_year );
	}

	return date;
}

char *tax_form_fiscal_end_date(
			int tax_year,
			int fiscal_begin_month )
{
	static char date[ 16 ];

	if ( fiscal_begin_month )
	{
		int fiscal_end_month;
		int over_12;

		fiscal_end_month = fiscal_begin_month + 11;
		over_12 = fiscal_end_month - 12;

		if ( over_12 > 0 )
		{
			fiscal_end_month = over_12;
			tax_year++;
		}

		sprintf(date,
			"%d-%.2d-%d",
			tax_year,
			fiscal_end_month,
			date_days_in_month(
				fiscal_end_month,
				tax_year ) );
	}
	else
	{
		sprintf(date,
			"%d-12-31",
			tax_year );
	}

	return date;
}

TAX_FORM_PDF *tax_form_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			char *tax_form_name,
			STATEMENT_CAPTION *statement_caption,
			LIST *tax_form_line_list )
{
	TAX_FORM_PDF *tax_form_pdf;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
	||   !tax_form_name
	||   !statement_caption )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( tax_form_line_list ) ) return (TAX_FORM_PDF *)0;

	tax_form_pdf = tax_form_pdf_calloc();

	tax_form_pdf->statement_link =
		statement_link_new(
			application_name,
			process_name,
			document_root_directory,
			statement_caption->begin_date_string,
			statement_caption->end_date_string,
			(char *)0 /* statement_pdf_preclose_key */,
			getpid() /* process_id */ );

	if ( !tax_form_pdf->statement_link )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: statement_link_new(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			process_name );
		exit( 1 );
	}

	tax_form_pdf->latex =
		latex_new(
			tax_form_pdf->statement_link->tex_filename,
			tax_form_pdf->statement_link->dvi_filename,
			tax_form_pdf->statement_link->working_directory,
			0 /* not statement_pdf_landscape_boolean */,
			(char *)0 /* statement_logo_filename */ );

	tax_form_pdf->tax_form_line_latex_table =
		tax_form_line_latex_table_new(
			tax_form_name,
			statement_caption->subtitle,
			tax_form_line_list );

	if ( !tax_form_pdf->tax_form_line_latex_table )
	{
		return (TAX_FORM_PDF *)0;
	}

	list_set(
		tax_form_pdf->latex->table_list,
		tax_form_pdf->tax_form_line_latex_table->latex_table );

	tax_form_pdf->tax_form_account_latex_list =
		tax_form_account_latex_list_new(
			tax_form_line_list );

	if ( !tax_form_pdf->tax_form_account_latex_list )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: tax_form_account_latex_list_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_set_list(
		tax_form_pdf->latex->table_list,
		tax_form_pdf->
			tax_form_account_latex_list->
			list );

	tax_form_pdf->tax_form_entity_latex_list =
		tax_form_entity_latex_list_new(
			tax_form_line_list );

	if ( !tax_form_pdf->tax_form_entity_latex_list )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: tax_form_entity_latex_list_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_set_list(
		tax_form_pdf->latex->table_list,
		tax_form_pdf->
			tax_form_entity_latex_list->
			list );

	return tax_form_pdf;
}

TAX_FORM_PDF *tax_form_pdf_calloc( void )
{
	TAX_FORM_PDF *tax_form_pdf;

	if ( ! ( tax_form_pdf = calloc( 1, sizeof( TAX_FORM_PDF ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_pdf;
}

TAX_FORM_LINE_LATEX_TABLE *
	tax_form_line_latex_table_new(
			char *tax_form_name,
			char *statement_caption_subtitle,
			LIST *tax_form_line_list )
{
	TAX_FORM_LINE_LATEX_TABLE *tax_form_line_latex_table;

	if ( !tax_form_name
	||   !statement_caption_subtitle )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( tax_form_line_list ) )
	{
		return (TAX_FORM_LINE_LATEX_TABLE *)0;
	}

	tax_form_line_latex_table = tax_form_line_latex_table_calloc();

	tax_form_line_latex_table->latex_table =
		latex_table_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			tax_form_line_latex_table_caption(
				tax_form_name,
				statement_caption_subtitle ) );

	tax_form_line_latex_table->latex_table->heading_list =
		tax_form_line_latex_table_heading_list();

	tax_form_line_latex_table->latex_table->row_list =
		tax_form_line_latex_table_row_list(
			tax_form_line_list );

	if ( !list_length( tax_form_line_latex_table->latex_table->row_list ) )
	{
		return (TAX_FORM_LINE_LATEX_TABLE *)0;
	}
	else
	{
		return tax_form_line_latex_table;
	}
}

TAX_FORM_LINE_LATEX_TABLE *tax_form_line_latex_table_calloc( void )
{
	TAX_FORM_LINE_LATEX_TABLE *tax_form_line_latex_table;

	if ( ! ( tax_form_line_latex_table =
			calloc(	1,
				sizeof( TAX_FORM_LINE_LATEX_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_line_latex_table;
}

char *tax_form_line_latex_table_caption(
			char *tax_form_name,
			char *statement_caption_subtitle )
{
	char caption[ 1024 ];

	if ( !tax_form_name
	||   !statement_caption_subtitle )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(caption,
		"%s %s",
		tax_form_name,
		statement_caption_subtitle );

	return strdup( caption );
}

LIST *tax_form_line_latex_table_heading_list( void )
{
	LIST *heading_list = list_new();

	list_set(
		heading_list,
		latex_table_heading_new(
			"tax_form_line",
			0 /* right_justify_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"tax_form_description",
			0 /* not right_Justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"balance",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	return heading_list;
}

LIST *tax_form_line_latex_table_row_list( LIST *tax_form_line_list )
{
	TAX_FORM_LINE *tax_form_line;
	LIST *list;

	if ( !list_rewind( tax_form_line_list ) ) return (LIST *)0;

	list = list_new();

	do {
		tax_form_line = list_get( tax_form_line_list );

		if ( !money_virtually_same(
			tax_form_line->total, 0.0 ) )
		{
			list_set(
				list,
				tax_form_line_latex_table_row(
					tax_form_line ) );
		}

	} while ( list_next( tax_form_line_list ) );

	return list;
}

LATEX_ROW *tax_form_line_latex_table_row( TAX_FORM_LINE *tax_form_line )
{
	LATEX_ROW *latex_row;

	if ( !tax_form_line
	||   !tax_form_line->total )
	{
		return (LATEX_ROW *)0;
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		tax_form_line->string,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		tax_form_line->tax_form_description,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup(
			timlib_dollar_round_string(
				tax_form_line->total ) ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	return latex_row;
}

TAX_FORM_ACCOUNT_LATEX_LIST *
	tax_form_account_latex_list_new(
			LIST *tax_form_line_list )
{
	TAX_FORM_ACCOUNT_LATEX_LIST *
		tax_form_account_latex_list;
	TAX_FORM_LINE *tax_form_line;

	if ( !list_rewind( tax_form_line_list ) )
	{
		return (TAX_FORM_ACCOUNT_LATEX_LIST *)0;
	}

	tax_form_account_latex_list =
		tax_form_account_latex_list_calloc();

	tax_form_account_latex_list->list = list_new();

	do {
		tax_form_line =
			list_get(
				tax_form_line_list );

		if ( !money_virtually_same(
			tax_form_line->total, 0.0 ) )
		{
			list_set(
				tax_form_account_latex_list->list,
				tax_form_account_latex_new(
					tax_form_line->string,
					tax_form_line->
						tax_form_line_account_list ) );
		}

	} while ( list_next( tax_form_line_list ) );

	return tax_form_account_latex_list;
}


TAX_FORM_ACCOUNT_LATEX_LIST *
	tax_form_account_latex_list_calloc(
			void )
{
	TAX_FORM_ACCOUNT_LATEX_LIST *
		tax_form_account_latex_list;

	if ( ! ( tax_form_account_latex_list =
		  calloc( 1, sizeof( TAX_FORM_ACCOUNT_LATEX_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_account_latex_list;
}

LATEX_TABLE *tax_form_account_latex_new(
			char *tax_form_line_string,
			LIST *tax_form_line_account_list )
{
	LATEX_TABLE *latex_table;

	if ( !tax_form_line_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( tax_form_line_account_list ) )
	{
		return (LATEX_TABLE *)0;
	}

	latex_table =
		latex_table_new(
			/* Returns heap memory */
			/* ------------------- */
			tax_form_account_latex_caption(
				tax_form_line_string ) );

	latex_table->heading_list =
		tax_form_account_latex_heading_list();

	latex_table->row_list =
		tax_form_account_latex_row_list(
			tax_form_line_account_list );

	if ( !list_length( latex_table->row_list ) )
		return (LATEX_TABLE *)0;
	else
		return latex_table;
}

char *tax_form_account_latex_caption(
			char *tax_form_line_string )
{
	char caption[ 128 ];

	if ( !tax_form_line_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: tax_form_line_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(caption,
		"Tax form line: %s",
		tax_form_line_string );

	return strdup( caption );
}

LIST *tax_form_account_latex_heading_list( void )
{
	LIST *heading_list = list_new();

	list_set(
		heading_list,
		latex_table_heading_new(
			"account_name",
			0 /* right_justify_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"total",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	return heading_list;
}

LIST *tax_form_account_latex_row_list(
			LIST *tax_form_line_account_list )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;
	LIST *list;

	if ( !list_rewind( tax_form_line_account_list ) )
	{
		return (LIST *)0;
	}

	list = list_new();

	do {
		tax_form_line_account =
			list_get(
				tax_form_line_account_list );

		if ( !money_virtually_same(
			tax_form_line_account->total, 0.0 ) )
		{
			list_set(
				list,
				tax_form_line_account_latex_row(
					tax_form_line_account ) );
		}

	} while ( list_next( tax_form_line_account_list ) );

	return list;
}

LATEX_ROW *tax_form_line_account_latex_row(
			TAX_FORM_LINE_ACCOUNT *tax_form_line_account )
{
	LATEX_ROW *latex_row;
	char buffer[ 128 ];

	if ( !tax_form_line_account
	||   !tax_form_line_account->total )
	{
		return (LATEX_ROW *)0;
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		strdup(
			format_initial_capital(
				buffer,
				tax_form_line_account->account_name ) ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup(
			timlib_dollar_round_string(
				tax_form_line_account->total ) ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	return latex_row;
}

TAX_FORM_ENTITY_LATEX_LIST *
	tax_form_entity_latex_list_new(
			LIST *tax_form_line_list )
{
	TAX_FORM_ENTITY_LATEX_LIST *
		tax_form_entity_latex_list;
	TAX_FORM_LINE *tax_form_line;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( !list_rewind( tax_form_line_list ) )
	{
		return (TAX_FORM_ENTITY_LATEX_LIST *)0;
	}

	tax_form_entity_latex_list =
		tax_form_entity_latex_list_calloc();

	tax_form_entity_latex_list->list = list_new();

	do {
		tax_form_line =
			list_get(
				tax_form_line_list );

		if ( money_virtually_same( tax_form_line->total, 0.0 ) )
		{
			continue;
		}

		list_rewind( tax_form_line->tax_form_line_account_list );

		do {
			tax_form_line_account =
				list_get(
					tax_form_line->
						tax_form_line_account_list );

			if ( money_virtually_same(
				tax_form_line_account->total,
				0.0 ) )
			{
				continue;
			}

			list_set(
				tax_form_entity_latex_list->list,
				tax_form_account_entity_latex_new(
					tax_form_line->string,
					tax_form_line_account->account_name,
					tax_form_line_account->
						tax_form_entity_list ) );

		} while ( list_next(
				tax_form_line->
					tax_form_line_account_list ) );

	} while ( list_next( tax_form_line_list ) );

	return tax_form_entity_latex_list;
}

TAX_FORM_ENTITY_LATEX_LIST *
	tax_form_entity_latex_list_calloc(
			void )
{
	TAX_FORM_ENTITY_LATEX_LIST *
		tax_form_entity_latex_list;

	if ( ! ( tax_form_entity_latex_list =
		  calloc(
			1,
			sizeof( TAX_FORM_ENTITY_LATEX_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_entity_latex_list;
}

LATEX_TABLE *tax_form_account_entity_latex_new(
			char *tax_form_line_string,
			char *account_name,
			LIST *tax_form_entity_list )
{
	LATEX_TABLE *latex_table;

	if ( !tax_form_line_string
	||   !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( tax_form_entity_list ) ) return (LATEX_TABLE *)0;

	latex_table =
		latex_table_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			tax_form_entity_latex_caption(
				tax_form_line_string,
				account_name ) );

	latex_table->heading_list =
		tax_form_entity_latex_heading_list();

	latex_table->row_list =
		tax_form_entity_latex_row_list(
			tax_form_entity_list );

	if ( !list_length( latex_table->row_list ) )
		return (LATEX_TABLE *)0;
	else
		return latex_table;
}

char *tax_form_entity_latex_caption(
			char *tax_form_line_string,
			char *account_name )
{
	char caption[ 1024 ];
	char buffer[ 128 ];

	if ( !tax_form_line_string
	||   !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(caption,
		"Tax form line: %s; Account: %s",
		tax_form_line_string,
		format_initial_capital(
			buffer,
			account_name ) );

	return strdup( caption );
}

LIST *tax_form_entity_latex_heading_list( void )
{
	LIST *heading_list = list_new();

	list_set(
		heading_list,
		latex_table_heading_new(
			"full_name",
			0 /* right_justify_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"total",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	return heading_list;
}

LIST *tax_form_entity_latex_row_list(
			LIST *tax_form_entity_list )
{
	LIST *row_list;
	TAX_FORM_ENTITY *tax_form_entity;

	if ( !list_rewind( tax_form_entity_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		tax_form_entity =
			list_get(
				tax_form_entity_list );

		if ( !money_virtually_same(
			tax_form_entity->total, 0.0 ) )
		{
			list_set(
				row_list,
				tax_form_entity_latex_row(
					tax_form_entity ) );
		}

	} while ( list_next( tax_form_entity_list ) );

	return row_list;
}

LATEX_ROW *tax_form_entity_latex_row(
			TAX_FORM_ENTITY *tax_form_entity )
{
	LATEX_ROW *latex_row;

	if ( !tax_form_entity
	||   !tax_form_entity->entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !tax_form_entity->total ) return (LATEX_ROW *)0;

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		tax_form_entity->entity->full_name,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup(
			timlib_dollar_round_string(
				tax_form_entity->total ) ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	return latex_row;
}

TAX_FORM_TABLE *tax_form_table_new(
			char *tax_form_name,
			STATEMENT_CAPTION *statement_caption,
			LIST *tax_form_line_list )
{
	TAX_FORM_TABLE *tax_form_table;

	if ( !tax_form_name
	||   !statement_caption )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	if ( !list_length( tax_form_line_list ) )
		return (TAX_FORM_TABLE *)0;

	tax_form_table = tax_form_table_calloc();

	return tax_form_table;
}

TAX_FORM_TABLE *tax_form_table_calloc( void )
{
	TAX_FORM_TABLE *tax_form_table;

	if ( ! ( tax_form_table = calloc( 1, sizeof( TAX_FORM_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_table;
}
