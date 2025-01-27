/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax_form.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "piece.h"
#include "sql.h"
#include "tax_form.h"

TAX_FORM_ENTITY *tax_form_entity_calloc( void )
{
	TAX_FORM_ENTITY *tax_form_entity;

	if ( ! ( tax_form_entity = calloc( 1, sizeof ( TAX_FORM_ENTITY ) ) ) )
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

LIST *tax_form_entity_list( LIST *tax_form_line_account_journal_list )
{
	TAX_FORM_LINE_ACCOUNT_JOURNAL *tax_form_line_account_journal;
	JOURNAL *journal;
	LIST *list;

	if ( !list_rewind( tax_form_line_account_journal_list ) )
	{
		return (LIST *)0;
	}

	list = list_new();

	do {
		tax_form_line_account_journal =
			list_get(
				tax_form_line_account_journal_list );

		journal = tax_form_line_account_journal->journal;

		if ( !journal )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: journal is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		tax_form_entity_getset(
			list,
			journal->full_name,
			tax_form_line_account_journal->amount );

	} while ( list_next( tax_form_line_account_journal_list ) );

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

	tax_form_line_account->account =
		account_fetch(
			tax_form_line_account->account_name,
			1 /* fetch_subclassification */,
			1 /* fetch_element */ );

	if ( !tax_form_line_account->account
	||   !tax_form_line_account->account->subclassification
	||   !tax_form_line_account->account->subclassification->element )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: account_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			tax_form_line_account->account_name );
		exit( 1 );
	}

	tax_form_line_account->subclassification_current_liability_boolean =
		subclassification_current_liability_boolean(
			SUBCLASSIFICATION_CURRENT_LIABILITY,
			tax_form_line_account->
				account->
				subclassification->
				subclassification_name );

	tax_form_line_account->subclassification_receivable_boolean =
		subclassification_receivable_boolean(
			SUBCLASSIFICATION_RECEIVABLE,
			tax_form_line_account->
				account->
				subclassification->
				subclassification_name );

	tax_form_line_account->journal_list =
		tax_form_line_account_journal_list(
			tax_form_fiscal_begin_date,
			tax_form_fiscal_end_date,
			TRANSACTION_DATE_PRECLOSE_TIME,
			tax_form_line_account->account_name,
			tax_form_line_account->
				account->
				subclassification->
				element->
				accumulate_debit
					/* element_accumulate_debit */,
			tax_form_line_account->
				subclassification_current_liability_boolean,
			tax_form_line_account->
				subclassification_receivable_boolean );

	tax_form_line_account->journal_list_total =
		tax_form_line_account_journal_list_total(
			tax_form_line_account->journal_list );

	tax_form_line_account->tax_form_entity_list =
		tax_form_entity_list(
			tax_form_line_account->journal_list );

	return tax_form_line_account;
}

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_calloc( void )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( ! ( tax_form_line_account =
			calloc(	1,
				sizeof ( TAX_FORM_LINE_ACCOUNT ) ) ) )
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
	char buffer[ 512 ];

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
	tax_form_line->description = strdup( buffer );

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
			calloc( 1, sizeof ( TAX_FORM_LINE ) ) ) )
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

		total += tax_form_line_account->journal_list_total;

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
		char *data_directory,
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

		if ( !tax_form->tax_form_table ) return NULL;
	}
	else
	if ( tax_form->statement_output_medium == statement_output_PDF )
	{
		tax_form->tax_form_latex =
			tax_form_latex_new(
				application_name,
				process_name,
				data_directory,
				tax_form_name,
				tax_form->statement_caption,
				tax_form->tax_form_line_list );

		if ( !tax_form->tax_form_latex ) return NULL;
	}

	return tax_form;
}

TAX_FORM *tax_form_calloc( void )
{
	TAX_FORM *tax_form;

	if ( ! ( tax_form = calloc( 1, sizeof ( TAX_FORM ) ) ) )
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

TAX_FORM_LATEX *tax_form_latex_new(
		char *application_name,
		char *process_name,
		char *data_directory,
		char *tax_form_name,
		STATEMENT_CAPTION *statement_caption,
		LIST *tax_form_line_list )
{
	TAX_FORM_LATEX *tax_form_latex;

	if ( !application_name
	||   !process_name
	||   !data_directory
	||   !tax_form_name
	||   !statement_caption )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( tax_form_line_list ) ) return NULL;

	tax_form_latex = tax_form_latex_calloc();

	tax_form_latex->statement_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		statement_link_new(
			application_name,
			process_name,
			data_directory,
			(char *)0
				/* transaction_date_begin_date_string */,
			statement_caption->date_now16
				/* end_date_string */,
			getpid() /* process_id */ );

	tax_form_latex->latex =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		latex_new(
			tax_form_latex->statement_link->tex_filename,
			tax_form_latex->
				statement_link->
				appaserver_link_working_directory,
			0 /* not landscape_boolean */,
			(char *)0 /* logo_filename */ );

	tax_form_latex->latex_table_list = list_new();

	tax_form_latex->tax_form_line_latex_table =
		tax_form_line_latex_table_new(
			tax_form_name,
			statement_caption->sub_title,
			tax_form_line_list );

	if ( !tax_form_latex->tax_form_line_latex_table ) return NULL;

	list_set(
		tax_form_latex->latex_table_list,
		tax_form_latex->tax_form_line_latex_table->latex_table );

	tax_form_latex->tax_form_account_latex_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		tax_form_account_latex_list_new(
			tax_form_line_list );

	list_set_list(
		tax_form_latex->latex_table_list,
		tax_form_latex->
			tax_form_account_latex_list->
			latex_table_list );

	tax_form_latex->tax_form_entity_latex_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		tax_form_entity_latex_list_new(
			tax_form_line_list );

	list_set_list(
		tax_form_latex->latex_table_list,
		tax_form_latex->
			tax_form_entity_latex_list->
			latex_table_list );

	return tax_form_latex;
}

TAX_FORM_LATEX *tax_form_latex_calloc( void )
{
	TAX_FORM_LATEX *tax_form_latex;

	if ( ! ( tax_form_latex = calloc( 1, sizeof ( TAX_FORM_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_latex;
}

TAX_FORM_LINE_LATEX_TABLE *
	tax_form_line_latex_table_new(
		char *tax_form_name,
		char *statement_caption_sub_title,
		LIST *tax_form_line_list )
{
	TAX_FORM_LINE_LATEX_TABLE *tax_form_line_latex_table;
	LIST *column_list;
	LIST *row_list;

	if ( !tax_form_name
	||   !statement_caption_sub_title )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( tax_form_line_list ) ) return NULL;

	tax_form_line_latex_table = tax_form_line_latex_table_calloc();

	column_list = tax_form_line_latex_table_column_list();

	row_list =
		tax_form_line_latex_table_row_list(
			tax_form_line_list,
			column_list );

	if ( !list_length( row_list ) ) return NULL;

	tax_form_line_latex_table->latex_table =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		latex_table_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			tax_form_line_latex_table_caption(
				tax_form_name,
				statement_caption_sub_title )
				/* title */,
			column_list,
			row_list );

	return tax_form_line_latex_table;
}

TAX_FORM_LINE_LATEX_TABLE *tax_form_line_latex_table_calloc( void )
{
	TAX_FORM_LINE_LATEX_TABLE *tax_form_line_latex_table;

	if ( ! ( tax_form_line_latex_table =
			calloc(	1,
				sizeof ( TAX_FORM_LINE_LATEX_TABLE ) ) ) )
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
		char *statement_caption_sub_title )
{
	char caption[ 1024 ];

	if ( !tax_form_name
	||   !statement_caption_sub_title )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(caption,
		"%s %s",
		tax_form_name,
		statement_caption_sub_title );

	return strdup( caption );
}

LIST *tax_form_line_latex_table_column_list( void )
{
	LIST *latex_column_list = list_new();

	list_set(
		latex_column_list,
		latex_column_new(
			"tax_form_line",
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );

	list_set(
		latex_column_list,
		latex_column_new(
			"tax_form_line_description",
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		latex_column_list,
		latex_column_new(
			"balance",
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	return latex_column_list;
}

LIST *tax_form_line_latex_table_row_list(
		LIST *tax_form_line_list,
		LIST *latex_column_list )
{
	TAX_FORM_LINE *tax_form_line;
	LIST *list;

	if ( !list_rewind( tax_form_line_list ) ) return (LIST *)0;

	list = list_new();

	do {
		tax_form_line = list_get( tax_form_line_list );

		if ( !tax_form_line->total ) continue;

		list_set(
			list,
			tax_form_line_latex_table_row(
				tax_form_line,
				latex_column_list ) );

	} while ( list_next( tax_form_line_list ) );

	return list;
}

LATEX_ROW *tax_form_line_latex_table_row(
		TAX_FORM_LINE *tax_form_line,
		LIST *latex_column_list )
{
	LIST *latex_cell_list;
	LATEX_COLUMN *latex_column;

	if ( !tax_form_line
	||   !tax_form_line->total
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		latex_cell_list,
		latex_cell_small_new(
			latex_column,
			tax_form_line->string ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		latex_cell_list,
		latex_cell_small_new(
			latex_column,
			tax_form_line->description ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		latex_cell_list,
		latex_cell_float_new(
			latex_column,
			tax_form_line->total /* value */ ) );

	return
	latex_row_new( latex_cell_list );
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
		char message[ 128 ];

		sprintf(message, "tax_form_line_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	tax_form_account_latex_list =
		tax_form_account_latex_list_calloc();

	tax_form_account_latex_list->latex_table_list = list_new();

	do {
		tax_form_line =
			list_get(
				tax_form_line_list );

		if ( !tax_form_line->total ) continue;

		list_set(
			tax_form_account_latex_list->latex_table_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			tax_form_account_latex_table(
				tax_form_line->string,
				tax_form_line->
					tax_form_line_account_list,
				tax_form_line->description,
				tax_form_line->total ) );

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
		  calloc( 1, sizeof ( TAX_FORM_ACCOUNT_LATEX_LIST ) ) ) )
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

LATEX_TABLE *tax_form_account_latex_table(
		char *tax_form_line_string,
		LIST *tax_form_line_account_list,
		char *tax_form_line_description,
		double tax_form_line_total )
{
	LIST *latex_column_list;
	LIST *latex_row_list;
	char *caption;

	if ( !tax_form_line_string
	||   !list_length( tax_form_line_account_list )
	||   !tax_form_line_description
	||   !tax_form_line_total )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	caption =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		tax_form_account_latex_caption(
			tax_form_line_string,
			tax_form_line_description,
			tax_form_line_total );

	latex_column_list =
		tax_form_account_latex_column_list();

	latex_row_list =
		tax_form_account_latex_row_list(
			tax_form_line_account_list,
			latex_column_list );

	if ( !list_length( latex_row_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"tax_form_account_latex_row_list(%s) returned empty.",
			tax_form_line_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_table_new(
		caption /* title */,
		latex_column_list,
		latex_row_list );
}

char *tax_form_account_latex_caption(
		char *tax_form_line_string,
		char *tax_form_line_description,
		double tax_form_line_total )
{
	char caption[ 256 ];
	char buffer[ 128 ];

	if ( !tax_form_line_string
	||   !tax_form_line_description
	||   !tax_form_line_total )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(caption,
		"Tax form line: %s/%s; Total: %s",
		tax_form_line_string,
		string_initial_capital( buffer, tax_form_line_description ),
		strdup( 
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_double(
				tax_form_line_total,
				0 /* decimal_count */ ) ) );

	return strdup( caption );
}

LIST *tax_form_account_latex_column_list( void )
{
	LIST *latex_column_list = list_new();

	list_set(
		latex_column_list,
		latex_column_new(
			"account_name",
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );

	list_set(
		latex_column_list,
		latex_column_new(
			"total",
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	return latex_column_list;
}

LIST *tax_form_account_latex_row_list(
		LIST *tax_form_line_account_list,
		LIST *latex_column_list )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;
	LIST *latex_row_list;

	if ( !list_length( latex_column_list )
	||   !list_rewind( tax_form_line_account_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_row_list = list_new();

	do {
		tax_form_line_account =
			list_get(
				tax_form_line_account_list );

		if ( !tax_form_line_account->journal_list_total )
		{
			continue;
		}

		list_set(
			latex_row_list,
			tax_form_line_account_latex_row(
				tax_form_line_account,
				latex_column_list ) );

	} while ( list_next( tax_form_line_account_list ) );

	return latex_row_list;
}

LATEX_ROW *tax_form_line_account_latex_row(
		TAX_FORM_LINE_ACCOUNT *tax_form_line_account,
		LIST *latex_column_list )
{
	LIST *latex_cell_list;
	LATEX_COLUMN *latex_column;
	char buffer[ 128 ];

	if ( !tax_form_line_account
	||   !tax_form_line_account->journal_list_total
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		latex_cell_list,
		latex_cell_small_new(
			latex_column,
			strdup(
				string_initial_capital(
					buffer,
					tax_form_line_account->
						account_name ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		latex_cell_list,
		latex_cell_float_new(
			latex_column,
			tax_form_line_account->journal_list_total ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( latex_cell_list );
}

TAX_FORM_ENTITY_LATEX_LIST *
	tax_form_entity_latex_list_new(
		LIST *tax_form_line_list )
{
	TAX_FORM_ENTITY_LATEX_LIST *
		tax_form_entity_latex_list;
	TAX_FORM_LINE *tax_form_line;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;
	LATEX_TABLE *latex_table;

	if ( !list_rewind( tax_form_line_list ) )
	{
		char message[ 128 ];

		sprintf(message, "tax_form_line_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	tax_form_entity_latex_list =
		tax_form_entity_latex_list_calloc();

	tax_form_entity_latex_list->latex_table_list = list_new();

	do {
		tax_form_line =
			list_get(
				tax_form_line_list );

		if ( !tax_form_line->total ) continue;

		if ( list_rewind(
				tax_form_line->
					tax_form_line_account_list ) )
		do {
			tax_form_line_account =
				list_get(
					tax_form_line->
						tax_form_line_account_list );

			if ( !tax_form_line_account->journal_list_total )
			{
				continue;
			}

			latex_table =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				tax_form_entity_latex_table(
					tax_form_line->string,
					tax_form_line_account->account_name,
					tax_form_line_account->
						journal_list_total,
					tax_form_line_account->
						tax_form_entity_list );

			list_set(
				tax_form_entity_latex_list->
					latex_table_list,
				latex_table );

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
			sizeof ( TAX_FORM_ENTITY_LATEX_LIST ) ) ) )
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

LATEX_TABLE *tax_form_entity_latex_table(
		char *tax_form_line_string,
		char *account_name,
		double journal_list_total,
		LIST *tax_form_entity_list )
{
	LIST *latex_column_list;
	LIST *latex_row_list;
	char *caption;

	if ( !tax_form_line_string
	||   !account_name
	||   !journal_list_total
	||   !list_length( tax_form_entity_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_column_list =
		tax_form_entity_latex_column_list();

	latex_row_list =
		tax_form_entity_latex_row_list(
			tax_form_entity_list,
			latex_column_list );

	if ( !list_length( latex_row_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"tax_form_entity_latex_row_list(%s) returned empty.",
			account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	caption =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		tax_form_entity_latex_caption(
			tax_form_line_string,
			account_name,
			journal_list_total );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_table_new(
		caption /* title */,
		latex_column_list,
		latex_row_list );
}

char *tax_form_entity_latex_caption(
		char *tax_form_line_string,
		char *account_name,
		double journal_list_total )
{
	char caption[ 1024 ];
	char buffer[ 128 ];

	if ( !tax_form_line_string
	||   !account_name
	||   !journal_list_total )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(caption,
		"Tax form line: %s; Account: %s; Total: %s",
		tax_form_line_string,
		string_initial_capital(
			buffer,
			account_name ),
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_double(
			journal_list_total,
			0 /* decimal_count */ ) );

	return strdup( caption );
}

LIST *tax_form_entity_latex_column_list( void )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		latex_column_new(
			"full_name",
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			"total",
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	return column_list;
}

LIST *tax_form_entity_latex_row_list(
		LIST *tax_form_entity_list,
		LIST *latex_column_list )
{
	LIST *row_list;
	TAX_FORM_ENTITY *tax_form_entity;

	if ( !list_length( latex_column_list )
	||   !list_rewind( tax_form_entity_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list = list_new();

	do {
		tax_form_entity =
			list_get(
				tax_form_entity_list );

		if ( !tax_form_entity->total ) continue;

		list_set(
			row_list,
			tax_form_entity_latex_row(
				tax_form_entity,
				latex_column_list ) );

	} while ( list_next( tax_form_entity_list ) );

	return row_list;
}

LATEX_ROW *tax_form_entity_latex_row(
		TAX_FORM_ENTITY *tax_form_entity,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;

	if ( !tax_form_entity
	||   !tax_form_entity->entity
	||   !tax_form_entity->total
	||   !tax_form_entity->entity->full_name
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			tax_form_entity->entity->full_name ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_float_new(
			latex_column,
			tax_form_entity->total ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
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

	if ( !list_length( tax_form_line_list ) ) return NULL;

	tax_form_table = tax_form_table_calloc();

	tax_form_table->html_table_list = list_new();

	tax_form_table->tax_form_line_html_table =
		tax_form_line_html_table_new(
			tax_form_name,
			statement_caption->sub_title,
			tax_form_line_list );

	if ( !tax_form_table->tax_form_line_html_table ) return NULL;

	list_set(
		tax_form_table->html_table_list,
		tax_form_table->tax_form_line_html_table->html_table );

	tax_form_table->tax_form_account_html_list =
		tax_form_account_html_list_new(
			tax_form_line_list );

	list_set_list(
		tax_form_table->html_table_list,
		tax_form_table->
			tax_form_account_html_list->
			html_table_list );

	tax_form_table->tax_form_entity_html_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		tax_form_entity_html_list_new(
			tax_form_line_list );

	list_set_list(
		tax_form_table->html_table_list,
		tax_form_table->
			tax_form_entity_html_list->
			html_table_list );

	return tax_form_table;
}

TAX_FORM_TABLE *tax_form_table_calloc( void )
{
	TAX_FORM_TABLE *tax_form_table;

	if ( ! ( tax_form_table = calloc( 1, sizeof ( TAX_FORM_TABLE ) ) ) )
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

TAX_FORM_LINE_HTML_TABLE *tax_form_line_html_table_new(
		char *tax_form_name,
		char *statement_caption_sub_title,
		LIST *tax_form_line_list )
{
	TAX_FORM_LINE_HTML_TABLE *tax_form_line_html_table;

	if ( !tax_form_name
	||   !statement_caption_sub_title
	||   !list_length( tax_form_line_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	tax_form_line_html_table = tax_form_line_html_table_calloc();

	tax_form_line_html_table->caption =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		tax_form_line_html_table_caption(
			tax_form_name,
			statement_caption_sub_title );

	tax_form_line_html_table->html_table =
		html_table_new(
			(char *)0 /* title */,
			(char *)0 /* sub_title */,
			tax_form_line_html_table->caption
				/* sub_sub_title */ );

	tax_form_line_html_table->html_table->column_list =
		tax_form_line_html_table_column_list();

	tax_form_line_html_table->html_table->row_list =
		tax_form_line_html_table_row_list(
			tax_form_line_list );

	if ( !list_length( tax_form_line_html_table->html_table->row_list ) )
		return NULL;

	return tax_form_line_html_table;
}

TAX_FORM_LINE_HTML_TABLE *tax_form_line_html_table_calloc( void )
{
	TAX_FORM_LINE_HTML_TABLE *tax_form_line_html_table;

	if ( ! ( tax_form_line_html_table =
			calloc( 1, sizeof ( TAX_FORM_LINE_HTML_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_line_html_table;
}

char *tax_form_line_html_table_caption(
		char *tax_form_name,
		char *statement_caption_sub_title )
{
	char caption[ 1024 ];

	if ( !tax_form_name
	||   !statement_caption_sub_title )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(caption,
		"%s %s",
		tax_form_name,
		statement_caption_sub_title );

	return strdup( caption );
}

LIST *tax_form_line_html_table_column_list( void )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		html_column_new(
			"tax_form_line",
			0 /* right_justify_flag */ ) );

	list_set(
		column_list,
		html_column_new(
			"tax_form_description",
			0 /* not right_Justified_flag */ ) );

	list_set(
		column_list,
		html_column_new(
			"balance",
			1 /* right_justified_flag */ ) );

	return column_list;
}

LIST *tax_form_line_html_table_row_list(
		LIST *tax_form_line_list )
{
	LIST *row_list;
	TAX_FORM_LINE *tax_form_line;

	if ( !list_rewind( tax_form_line_list ) )
	{
		char message[ 128 ];

		sprintf(message, "tax_form_line_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list = list_new();

	do {
		tax_form_line = list_get( tax_form_line_list );

		if ( !tax_form_line->total ) continue;

		list_set(
			row_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			tax_form_line_html_table_row(
				tax_form_line ) );

	} while ( list_next( tax_form_line_list ) );

	return row_list;
}

HTML_ROW *tax_form_line_html_table_row( TAX_FORM_LINE *tax_form_line )
{
	LIST *cell_list;

	if ( !tax_form_line )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			tax_form_line->string,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			tax_form_line->description,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				string_commas_double(
					tax_form_line->total,
					0 /* decimal_count */ ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

TAX_FORM_ACCOUNT_HTML_LIST *tax_form_account_html_list_new(
		LIST *tax_form_line_list )
{
	TAX_FORM_ACCOUNT_HTML_LIST *tax_form_account_html_list;
	TAX_FORM_LINE *tax_form_line;

	if ( !list_rewind( tax_form_line_list ) )
	{
		char message[ 128 ];

		sprintf(message, "tax_form_line_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	tax_form_account_html_list = tax_form_account_html_list_calloc();

	tax_form_account_html_list->html_table_list = list_new();

	do {
		tax_form_line = list_get( tax_form_line_list );

		if ( !tax_form_line->total ) continue;

		list_set(
			tax_form_account_html_list->html_table_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			tax_form_account_html_table(
				tax_form_line->string,
				tax_form_line->
					tax_form_line_account_list,
				tax_form_line->description,
				tax_form_line->total ) );

	} while ( list_next( tax_form_line_list ) );

	return tax_form_account_html_list;
}

TAX_FORM_ACCOUNT_HTML_LIST *tax_form_account_html_list_calloc( void )
{
	TAX_FORM_ACCOUNT_HTML_LIST *tax_form_account_html_list;

	if ( ! ( tax_form_account_html_list =
			calloc( 1, sizeof ( TAX_FORM_ACCOUNT_HTML_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_account_html_list;
}

HTML_TABLE *tax_form_account_html_table(
		char *tax_form_line_string,
		LIST *tax_form_line_account_list,
		char *tax_form_line_description,
		double tax_form_line_total )
{
	HTML_TABLE *html_table;

	if ( !tax_form_line_string
	||   !list_length( tax_form_line_account_list )
	||   !tax_form_line_description
	||   !tax_form_line_total )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_table =
		html_table_new(
			(char *)0 /* title */,
			(char *)0 /* sub_title */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			tax_form_account_html_caption(
				tax_form_line_string,
				tax_form_line_description,
				tax_form_line_total )
					/* sub_sub_title */ );

	html_table->column_list = tax_form_account_html_column_list();

	html_table->row_list =
		tax_form_account_html_row_list(
			tax_form_line_account_list );

	if ( !list_length( html_table->row_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"tax_form_account_html_row_list(%s) returned empty.",
			tax_form_line_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return html_table;
}

char *tax_form_account_html_caption(
			char *tax_form_line_string,
			char *tax_form_line_description,
			double tax_form_line_total )
{
	char caption[ 256 ];
	char buffer[ 128 ];

	if ( !tax_form_line_string
	||   !tax_form_line_description
	||   !tax_form_line_total )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(caption,
		"Tax form line: %s/%s; Total: %s",
		tax_form_line_string,
		format_initial_capital( buffer, tax_form_line_description ),
		strdup( 
			timlib_dollar_round_string(
				tax_form_line_total ) ) );

	return strdup( caption );
}

LIST *tax_form_account_html_column_list( void )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		html_column_new(
			"account_name",
			0 /* right_justify_flag */ ) );

	list_set(
		column_list,
		html_column_new(
			"total",
			1 /* right_justified_flag */ ) );

	return column_list;
}

LIST *tax_form_account_html_row_list(
		LIST *tax_form_line_account_list )
{
	LIST *row_list;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( !list_rewind( tax_form_line_account_list ) )
	{
		char message[ 128 ];

		sprintf(message, "tax_form_line_account_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list = list_new();

	do {
		tax_form_line_account = list_get( tax_form_line_account_list );

		if ( !tax_form_line_account->journal_list_total ) continue;

		list_set(
			row_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			tax_form_line_account_html_row(
				tax_form_line_account ) );

	} while ( list_next( tax_form_line_account_list ) );

	return row_list;
}

HTML_ROW *tax_form_line_account_html_row(
		TAX_FORM_LINE_ACCOUNT *tax_form_line_account )
{
	LIST *cell_list;

	if ( !tax_form_line_account )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: tax_form_line_account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			tax_form_line_account->account_name,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_double(
					tax_form_line_account->
						journal_list_total,
					0 /* decimal_count */ ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

TAX_FORM_ENTITY_HTML_LIST *tax_form_entity_html_list_new(
		LIST *tax_form_line_list )
{
	TAX_FORM_ENTITY_HTML_LIST *tax_form_entity_html_list;
	TAX_FORM_LINE *tax_form_line;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( !list_rewind( tax_form_line_list ) )
	{
		char message[ 128 ];

		sprintf(message, "tax_form_line_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	tax_form_entity_html_list = tax_form_entity_html_list_calloc();
	tax_form_entity_html_list->html_table_list = list_new();

	do {
		tax_form_line = list_get( tax_form_line_list );

		if ( !tax_form_line->total ) continue;

		if ( list_rewind(
			tax_form_line->
				tax_form_line_account_list ) )
		do {
			tax_form_line_account =
				list_get(
					tax_form_line->
						tax_form_line_account_list );

			if ( !tax_form_line_account->journal_list_total )
				continue;

			list_set(
				tax_form_entity_html_list->html_table_list,
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				tax_form_entity_html_table(
					tax_form_line->string,
					tax_form_line_account->
						account_name,
					tax_form_line_account->
						journal_list_total,
					tax_form_line_account->
						tax_form_entity_list ) );

		} while ( list_next(
				tax_form_line->tax_form_line_account_list ) );

	} while ( list_next( tax_form_line_list ) );

	return tax_form_entity_html_list;
}

TAX_FORM_ENTITY_HTML_LIST *tax_form_entity_html_list_calloc( void )
{
	TAX_FORM_ENTITY_HTML_LIST *tax_form_entity_html_list;

	if ( ! ( tax_form_entity_html_list =
			calloc( 1, sizeof ( TAX_FORM_ENTITY_HTML_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_entity_html_list;
}

HTML_TABLE *tax_form_entity_html_table(
		char *tax_form_line_string,
		char *account_name,
		double journal_list_total,
		LIST *tax_form_entity_list )
{
	HTML_TABLE *html_table;

	if ( !tax_form_line_string
	||   !account_name
	||   !journal_list_total
	||   !list_length( tax_form_entity_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	html_table =
		html_table_new(
			(char *)0 /* title */,
			(char *)0 /* sub_title */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			tax_form_entity_html_caption(
				tax_form_line_string,
				account_name,
				journal_list_total )
					/* sub_sub_title */ );

	html_table->column_list =
		tax_form_entity_html_column_list();

	html_table->row_list =
		tax_form_entity_html_row_list(
			tax_form_entity_list );

	return html_table;
}

char *tax_form_entity_html_caption(
		char *tax_form_line_string,
		char *account_name,
		double journal_list_total )
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
		"Tax form line: %s; Account: %s; Total: %s",
		tax_form_line_string,
		format_initial_capital(
			buffer,
			account_name ),
		strdup(
			timlib_dollar_round_string(
				journal_list_total ) ) );

	return strdup( caption );
}

LIST *tax_form_entity_html_column_list( void )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		html_column_new(
			"full_name",
			0 /* right_justify_flag */ ) );

	list_set(
		column_list,
		html_column_new(
			"total",
			1 /* right_justified_flag */ ) );

	return column_list;
}

LIST *tax_form_entity_html_row_list( LIST *tax_form_entity_list )
{
	LIST *row_list;
	TAX_FORM_ENTITY *tax_form_entity;

	if ( !list_rewind( tax_form_entity_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		tax_form_entity = list_get( tax_form_entity_list );

		if ( !tax_form_entity->total ) continue;

		list_set(
			row_list,
			tax_form_entity_html_row(
				tax_form_entity ) );

	} while ( list_next( tax_form_entity_list ) );

	return row_list;
}

HTML_ROW *tax_form_entity_html_row( TAX_FORM_ENTITY *tax_form_entity )
{
	LIST *cell_list;

	if ( !tax_form_entity
	||   !tax_form_entity->entity )
	{
		char message[ 128 ];

		sprintf(message, "tax_form_entity is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			tax_form_entity->entity->full_name,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_double(
					tax_form_entity->total,
					0 /* decimal_count */ ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	return
	html_row_new( cell_list );
}

LIST *tax_form_line_account_journal_list(
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date,
		const char *transaction_date_preclose_time,
		char *account_name,
		boolean element_accumulate_debit,
		boolean subclassification_current_liability_boolean,
		boolean subclassification_receivable_boolean )
{
	LIST *journal_list;
	LIST *list;
	JOURNAL *journal;

	journal_list =
		journal_tax_form_list(
			tax_form_fiscal_begin_date,
			tax_form_fiscal_end_date,
			transaction_date_preclose_time,
			account_name );

	if ( !list_rewind( journal_list ) ) return (LIST *)0;

	list = list_new();

	do {
		journal = list_get( journal_list );

		list_set(
			list,
			tax_form_line_account_journal_new(
				element_accumulate_debit,
				subclassification_current_liability_boolean,
				subclassification_receivable_boolean,
			journal ) );

	} while ( list_next( journal_list ) );

	return list;
}

TAX_FORM_LINE_ACCOUNT_JOURNAL *
	tax_form_line_account_journal_new(
			boolean element_accumulate_debit,
			boolean subclassification_current_liability_boolean,
			boolean subclassification_receivable_boolean,
			JOURNAL *journal )
{
	TAX_FORM_LINE_ACCOUNT_JOURNAL *tax_form_line_account_journal;

	if ( !journal )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: journal is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	tax_form_line_account_journal =
		tax_form_line_account_journal_calloc();

	tax_form_line_account_journal->amount =
		tax_form_line_account_journal_amount(
			element_accumulate_debit,
			subclassification_current_liability_boolean,
			subclassification_receivable_boolean,
			journal->debit_amount,
			journal->credit_amount );

	tax_form_line_account_journal->journal = journal;

	return tax_form_line_account_journal;
}

TAX_FORM_LINE_ACCOUNT_JOURNAL *
	tax_form_line_account_journal_calloc(
			void )
{
	TAX_FORM_LINE_ACCOUNT_JOURNAL *tax_form_line_account_journal;

	if ( ! ( tax_form_line_account_journal =
		  calloc(
			1,
			sizeof ( TAX_FORM_LINE_ACCOUNT_JOURNAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return tax_form_line_account_journal;
}

double tax_form_line_account_journal_amount(
			boolean element_accumulate_debit,
			boolean subclassification_current_liability_boolean,
			boolean subclassification_receivable_boolean,
			double debit_amount,
			double credit_amount )
{
	double amount;

	if ( subclassification_current_liability_boolean )
	{
		amount = debit_amount;
	}
	else
	if ( subclassification_receivable_boolean )
	{
		amount = credit_amount;
	}
	else
	{
		amount =
			journal_amount(
				debit_amount,
				credit_amount,
				element_accumulate_debit );
	}

	return amount;
}

double tax_form_line_account_journal_list_total(
			LIST *tax_form_line_account_journal_list )
{
	TAX_FORM_LINE_ACCOUNT_JOURNAL *tax_form_line_account_journal;
	double total;

	if ( !list_rewind(
			tax_form_line_account_journal_list ) )
	{
		return 0.0;
	}

	total = 0.0;

	do {
		tax_form_line_account_journal =
			list_get(
				tax_form_line_account_journal_list );

		total +=
			tax_form_line_account_journal->amount;

	} while ( list_next( 
			tax_form_line_account_journal_list ) );

	return total;
}

