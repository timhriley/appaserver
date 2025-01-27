/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/tax_form_report.c			*/
/* ----------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "statement.h"
#include "tax_form.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	APPASERVER_PARAMETER *appaserver_parameter;
	char *tax_form_name;
	int tax_year;
	int fiscal_begin_month /* zero or one based */;
	char *output_medium_string;
	TAX_FORM *tax_form;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
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
			atoi(
				/* Returns static memory */
				/* --------------------- */
				date_current_yyyy_mm_dd_string(
					date_utc_offset() ) );
	}

	fiscal_begin_month = atoi( argv[ 4 ] );
	output_medium_string = argv[ 5 ];

	appaserver_parameter = appaserver_parameter_new();

	if ( strcmp( output_medium_string, "stdout" ) != 0 )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name /* title */ );
	}

	tax_form =
		tax_form_fetch(
			application_name,
			process_name,
			appaserver_parameter->data_directory,
			tax_form_name,
			tax_year,
			fiscal_begin_month,
			output_medium_string );

	if ( !tax_form )
	{
		printf( "<h3>Warning. Nothing to report for year.</h3>\n" );
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
		html_table_list_output(
			tax_form->
				tax_form_table->
				html_table_list,
			HTML_TABLE_ROWS_BETWEEN_HEADING );
	}
	else
	if ( tax_form->tax_form_latex )
	{
		latex_table_list_output(
			tax_form->
				tax_form_latex->
				statement_link->
				tex_filename,
			tax_form->
				tax_form_latex->
				statement_link->
				appaserver_link_working_directory,
			tax_form->
				tax_form_latex->
				statement_link->
				pdf_anchor_html,
			tax_form->
				tax_form_latex->
				latex,
			tax_form->
				tax_form_latex->
				latex_table_list );
	}

	if ( strcmp( output_medium_string, "stdout" ) != 0 )
		document_close();

	return 0;
}

