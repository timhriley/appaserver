/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/tax_form_report.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "statement.h"
#include "tax_form.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *tax_form_name;
	int tax_year;
	int fiscal_begin_month /* zero or one based */;
	char *output_medium_string;
	TAX_FORM *tax_form;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s process tax_form tax_year fiscal_begin_month output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	tax_form_name = argv[ 2 ];

	if ( ! ( tax_year = atoi( argv[ 3 ] ) ) )
	{
		tax_year =
			atoi( date_get_current_yyyy_mm_dd(
				date_get_utc_offset() ) );
	}

	fiscal_begin_month = atoi( argv[ 4 ] );
	output_medium_string = argv[ 5 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( strcmp( output_medium_string, "stdout" ) != 0 )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );
	}

	tax_form =
		tax_form_fetch(
			application_name,
			process_name,
			appaserver_parameter_file->document_root,
			tax_form_name,
			tax_year,
			fiscal_begin_month,
			output_medium_string );

	if ( !tax_form )
	{
		printf( "<h3>Error. Invalid year.</h3>\n" );
		document_close();
		exit( 0 );
	}

	printf( "%s\n",
		tax_form->
			statement_caption->
			frame_title );

	if ( !list_length( tax_form->tax_form_line_list ) )
	{
		printf(
		"<h3>Error. Nothing to report for this tax form.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( tax_form->tax_form_table )
	{
		printf(
		"<h3>Warning. This option is not yet written.</h3>\n" );
/*
		html_table_list_output(
			tax_form->
				tax_form_table->
				html_table_list,
			HTML_TABLE_ROWS_BETWEEN_HEADING );
*/
	}
	else
	if ( tax_form->tax_form_pdf )
	{
		statement_latex_output(
			tax_form->
				tax_form_pdf->
				latex,
			tax_form->
				tax_form_pdf->
				statement_link->
				ftp_output_filename,
			statement_pdf_prompt( process_name ),
			process_name,
			tax_form->
				statement_caption->
				date_time_string );
	}

	if ( strcmp( output_medium_string, "stdout" ) != 0 )
		document_close();

	return 0;
}

