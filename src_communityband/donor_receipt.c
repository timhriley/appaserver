/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_communityband/donor_receipt.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "application_constants.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "date.h"
#include "ledger.h"
#include "latex.h"
#include "date_convert.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define TABLE_ROWS			27
#define STATEMENT_PROMPT		"Press here to view statement."
#define LATEX_PROMPT			"Press here to view latex file."

/* Prototypes */
/* ---------- */
void donor_receipt_get_address(
					char **street_address,
					char **city_state_zip,
					char *donor_name,
					char *application_name );

void donor_receipt_donor(		FILE *output_stream,
					LATEX_TABLE *latex_table,
					char *logo_filename,
					char *application_name,
					char *year_string,
					char *donor_name );

LIST *donor_receipt_get_donor_name_list(
					char *application_name,
					char *year_string );

void donor_receipt_each_donor(
					FILE *output_stream,
					LATEX_TABLE *latex_table,
					char *logo_filename,
					char *application_name,
					char *year_string,
					LIST *donor_name_list );

void donor_receipt(			char *application_name,
					char *year_string,
					char *process_name,
					char *document_root_directory );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 256 ];
	char sub_title[ 256 ];
	char *database_string = {0};
	char *year_string;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
"Usage: %s ignored process year\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	year_string = argv[ 3 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	format_initial_capital( title, process_name );
	sprintf( sub_title, "For year %s", year_string );

	document = document_new( title, application_name );
	document->output_content_type = 1;

	document_output_heading(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->
				appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(	document->application_name,
				document->onload_control_string );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

	donor_receipt(
		application_name,
		year_string,
		process_name,
		appaserver_parameter_file->
			document_root );

	document_close();

	return 0;

} /* main() */

void donor_receipt(		char *application_name,
				char *year_string,
				char *process_name,
				char *document_root_directory )
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	char *tex_output_filename;
	pid_t pid = getpid();
	LATEX_TABLE_HEADING *table_heading;
	APPASERVER_LINK_FILE *appaserver_link_file;
	LIST *donor_name_list;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			process_name,
			application_name,
			pid,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	working_directory =
		appaserver_link_source_directory(
			document_root_directory,
			application_name );

	appaserver_link_file->extension = "tex";

	latex_filename =
		strdup( appaserver_link_get_tail_half(
				(char *)0 /* application_name */,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension ) );

	tex_output_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	appaserver_link_file->extension = "dvi";

	dvi_filename =
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

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			application_constants_quick_fetch(
				application_name,
				"logo_filename" /* key */ ) );

	latex_output_documentclass_letter(
		latex->output_stream,
		1 /* omit_page_numbers */ );

	latex_table =
		latex_new_latex_table(
			(char *)0 /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Donation Date";
	table_heading->right_justified_flag = 0;
	list_append_pointer( latex_table->heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Account";
	table_heading->right_justified_flag = 0;
	list_append_pointer( latex_table->heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Amount";
	table_heading->right_justified_flag = 1;
	list_append_pointer( latex_table->heading_list, table_heading );

	donor_name_list =
		donor_receipt_get_donor_name_list(
			application_name,
			year_string );

	if ( list_length( donor_name_list ) )
	{
		donor_receipt_each_donor(
			latex->output_stream,
			list_get_first_pointer(
				latex->table_list ),
			latex->logo_filename,
			application_name,
			year_string,
			donor_name_list );
	}

	latex_output_document_footer( latex->output_stream );

	fclose( latex->output_stream );

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

	appaserver_link_file->extension = "pdf";

	ftp_output_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		STATEMENT_PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );

	printf( "<br>\n" );

	appaserver_library_output_ftp_prompt(
		tex_output_filename, 
		LATEX_PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );

} /* donor_receipt() */

void donor_receipt_each_donor(
				FILE *output_stream,
				LATEX_TABLE *latex_table,
				char *logo_filename,
				char *application_name,
				char *year_string,
				LIST *donor_name_list )
{
	char *donor_name;

	if ( !list_rewind( donor_name_list ) ) return;

	do {
		donor_name = list_get_pointer( donor_name_list );

		donor_receipt_donor(
				output_stream,
				latex_table,
				logo_filename,
				application_name,
				year_string,
				donor_name );

		if ( !list_at_end( donor_name_list ) )
		{
			fprintf( output_stream, "\n\\newpage\n\n" );
		}

	} while( list_next( donor_name_list ) );

} /* donor_receipt_each_donor() */

void donor_receipt_donor(	FILE *output_stream,
				LATEX_TABLE *latex_table,
				char *logo_filename,
				char *application_name,
				char *year_string,
				char *donor_name )
{
	LATEX_ROW *latex_row;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char buffer[ 128 ];
	char donation_date[ 16 ];
	char american_date[ 16 ];
	char account[ 64 ];
	char amount_string[ 16 ];
	double amount;
	double total_amount = 0.0;
	int row_count = 0;
	char *street_address;
	char *city_state_zip;
	char *date_string;

	date_string = pipe2string( "now.sh full" );

	latex_output_return_envelope_document_heading(
		output_stream,
		logo_filename,
		"Capitolpops Concert Band" /* organization_name */,
		"P.O. Box 633" /* street_address */,
		"Citrus Heights, CA 95611" /* city_state_zip */,
		date_string );

	fprintf( output_stream,
		 "\\begin{tabular}[t]{l}\n"
		 "\\bf Donor:\n"
		 "\\end{tabular}\n\n" );

	street_address = "";
	city_state_zip = "";

	donor_receipt_get_address(
		&street_address,
		&city_state_zip,
		donor_name,
		application_name );

	sprintf( sys_string,
		 "donor_list.sh %s donor_list %s \"%s\" stdout",
		 application_name,
		 year_string,
		 escape_character(
			buffer,
			donor_name,
			'\'' ) );

	input_pipe = popen( sys_string, "r" );

	latex_table->row_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( timlib_strncmp( input_buffer, "Date," ) == 0 ) continue;

		if ( character_count(
			FOLDER_DATA_DELIMITER,
			input_buffer ) != 2 )
		{
			fprintf( stderr,
		"Warning in %s/%s()/%d: not two delimiters in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			continue;
		}

		piece( donation_date, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		latex_row = latex_new_latex_row();
		list_append_pointer( latex_table->row_list, latex_row );

		date_convert_source_international(
			american_date,
			american,
			donation_date );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( american_date ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );

		piece( account, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		format_initial_capital( account, account );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( account ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );

		piece( amount_string, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		amount = atof( amount_string );
		sprintf( amount_string, "\\$%.2lf", amount );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( amount_string ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );

		total_amount += amount;

		row_count++;
	}

	pclose( input_pipe );

	latex_output_indented_address(
		output_stream,
		donor_name /* full_name */,
		street_address,
		city_state_zip );

	fprintf( output_stream,
		 "\\begin{tabular}{l}\n" );

	if ( list_length( latex_table->row_list ) == 1 )
	{
		fprintf(output_stream,
			"\\\\\n"
		 	"Thank you for your %s donation.\\\\\n"
			"\\\\\n"
			"\n",
		 	year_string );
	}
	else
	{
		fprintf(output_stream,
			"\\\\\n"
		 	"Thank you for your %s donations.\\\\\n"
			"\\\\\n"
			"\n",
		 	year_string );
	}

	fprintf( output_stream,
		 "\\end{tabular}\n\n" );

	latex_output_table_heading(
		output_stream,
		(char *)0 /* caption */,
		latex_table->heading_list );

	latex_output_table_row_list(
		output_stream,
		latex_table->row_list,
		list_length( latex_table->heading_list ) );

	latex_output_table_vertical_padding(
		output_stream,
		list_length( latex_table->heading_list ),
		TABLE_ROWS - row_count /* empty_vertical_rows */ );

	/* Total Amount */
	/* ------------ */
	latex_table->row_list = list_new();

	latex_row = latex_new_latex_row();
	latex_row->preceed_double_line = 1;
	latex_row->large_boolean = 1;
	latex_row->bold_boolean = 1;
	list_set( latex_table->row_list, latex_row );

	latex_column_data_set(
		latex_row->column_data_list,
		"Total",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	sprintf( amount_string, "\\$%.2lf", total_amount );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( amount_string ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_output_table_row_list(
		output_stream,
		latex_table->row_list,
		list_length( latex_table->heading_list ) );

	latex_output_table_footer( output_stream );

	fprintf( output_stream,
"\nThe Capitol Pops Concert Band is a 501(c)(3) tax deductible organization.\n"
"Our federal tax identification number is 90-0090160. Consult\n"
"your tax advisor about the deductibility of contributions.\n" );

} /* donor_receipt_donor() */

LIST *donor_receipt_get_donor_name_list(
			char *application_name,
			char *year_string )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "donor_list.sh %s donor_list %s donor stdout	|"
		 "grep -v 'Full Name'				 ",
		 application_name,
		 year_string );

	return pipe2list( sys_string );

} /* donor_receipt_get_donor_name_list() */

void donor_receipt_get_address(
			char **street_address,
			char **city_state_zip,
			char *donor_name,
			char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char where[ 128 ];
	char buffer[ 512 ];
	char *results;
	char local_street_address[ 128 ];
	char city[ 32 ];
	char state[ 8 ];
	char zip_code[ 16 ];

	select = "street_address, city, state_code, zip_code";

	sprintf( where,
		 "full_name = '%s'",
		 escape_character(
			buffer,
			donor_name,
			'\'' ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=entity		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot fetch (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 donor_name );
		exit( 1 );
	}

	piece(	local_street_address,
		FOLDER_DATA_DELIMITER,
		results,
		0 );

	piece(	city,
		FOLDER_DATA_DELIMITER,
		results,
		1 );

	piece(	state,
		FOLDER_DATA_DELIMITER,
		results,
		2 );

	piece(	zip_code,
		FOLDER_DATA_DELIMITER,
		results,
		3 );

	*street_address = strdup( local_street_address );

	sprintf(	buffer,
			"%s, %s %s",
			city,
			state,
			zip_code );

	*city_state_zip = strdup( buffer );

} /* donor_receipt_get_address() */

