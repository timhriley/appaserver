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

		if ( journal->balance )
		{
			tax_form_entity_getset(
				list,
				journal->full_name,
				journal->street_address,
				journal->balance );
		}

	} while ( list_next( journal_tax_form_list ) );

	return list;
}

void tax_form_entity_getset(
			LIST *list,
			char *full_name,
			char *street_address,
			double journal_balance )
{
	TAX_FORM_ENTITY *tax_form_entity;

	if ( !list
	||   !full_name
	||   !street_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !journal_balance ) return;

	if ( list_rewind( list ) )
	{
		do {
			tax_form_entity = list_get( list );


			if ( strcmp(
				tax_form_entity->entity->full_name,
				full_name ) == 0
			&& strcmp(
				tax_form_entity->entity->street_address,
				street_address ) == 0 )
			{
				tax_form_entity->balance += journal_balance;
				return;
			}
		} while( list_next( list ) );
	}

	tax_form_entity = tax_form_entity_calloc();

	tax_form_entity->entity =
		entity_new(
			full_name,
			street_address );

	tax_form_entity->balance = journal_balance;

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
			tax_form_line_account->account_name );

	if ( !list_length( tax_form_line_account->journal_tax_form_list ) )
	{
		return (TAX_FORM_LINE_ACCOUNT *)0;
	}

	tax_form_line_account->total =
		tax_form_line_account_total(
			tax_form_line_account->
				journal_tax_form_list );

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
	return
	journal_balance_sum(
		journal_tax_form_list
			/* journal_list */ );
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
	tax_form_line->tax_form_line_string = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	tax_form_line->tax_form_description = strdup( buffer );

	tax_form_line->tax_form_line_account_list =
		tax_form_line_account_list(
			tax_form_name,
			tax_form_fiscal_begin_date,
			tax_form_fiscal_end_date,
			tax_form_line->tax_form_line_string );

	tax_form_line->total =
		tax_form_line_total(
			tax_form_line->
				tax_form_line_account_list );

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

	if ( tax_form->statement_output_medium == statement_output_table )
	{
		tax_form->tax_form_table =
			tax_form_table_new(
				tax_form->statement_caption,
				tax_form->tax_form_line_list );
	}
	else
	if ( tax_form->statement_output_medium == statement_output_PDF )
	{
		tax_form->tax_form_PDF =
			tax_form_pdf_new(
				application_name,
				process_name,
				document_root_directory,
				tax_form_name,
				tax_form->statement_caption,
				tax_form->tax_form_line_list );
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

	sprintf(date,
		"%d-%.2d-01",
		tax_year,
		fiscal_begin_month );

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

	if ( !list_rewind( tax_form_line_list ) ) return (TAX_FORM_PDF *)0;

	tax_form_pdf = tax_form_pdf_calloc();

	tax_form_pdf->statement_link =
		statement_link_new(
			application_name,
			process_name,
			document_root_directory,
			tax_form_fiscal_begin_date,
			tax_form_fiscal_end_date,
			(char *)0 /* statement_pdf_preclose_key */,
			getpid() /* process_id */ );

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

