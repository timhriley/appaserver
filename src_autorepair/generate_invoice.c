/* $APPASERVER_HOME/src_autorepair/generate_invoice.c			*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/
/* Note: links to generate_workorder					*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "latex_invoice.h"
#include "application_constants.h"
#include "date_convert.h"
#include "autorepair.h"
#include "ledger.h"
#include "entity.h"
#include "customer.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean generate_invoice_vehicle_information(
			char **vehicle_make,
			char **vehicle_model,
			char **vehicle_trim,
			int *vehicle_year,
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time );

double populate_line_item_list(
			LIST *invoice_line_item_list,
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *completed_date_time );

LATEX_INVOICE_CUSTOMER *generate_invoice_invoice_customer(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *vehicle_make,
			char *vehicle_model,
			char *vehicle_trim,
			int vehicle_year );

boolean build_latex_invoice(	FILE *output_stream,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year,
				DICTIONARY *application_constants_dictionary,
				boolean omit_money,
				boolean workorder );

void output_invoice_window(
				char *application_name,
				char *ftp_output_filename,
				int process_id,
				char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int process_id = getpid();
	FILE *output_stream;
	char *output_filename;
	char *output_directory;
	char *ftp_output_filename;
	DOCUMENT *document;
	APPLICATION_CONSTANTS *application_constants;
	boolean workorder = 0;
	char sys_string[ 1024 ];
	char *vehicle_make = {0};
	char *vehicle_model = {0};
	char *vehicle_trim = {0};
	int vehicle_year = 0;
	APPASERVER_LINK_FILE *appaserver_link_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
"Usage: %s process full_name street_address sale_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	workorder = ( strcmp( argv[ 0 ], "generate_workorder" ) == 0 );

	process_name = argv[ 1 ];
	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	appaserver_link_file->extension = "tex";

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

	if ( ! ( output_stream = fopen( output_filename, "w" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: file open error = (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			output_filename );
		exit( 1 );
	}

	document_quick_output( application_name );

	if ( !generate_invoice_vehicle_information(
				&vehicle_make,
				&vehicle_model,
				&vehicle_trim,
				&vehicle_year,
				application_name,
				full_name,
				street_address,
				sale_date_time ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get vehicle information for %s/%s/%s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 sale_date_time );
		exit( 1 );
	}

	if ( !build_latex_invoice(
		output_stream,
		application_name,
		full_name,
		street_address,
		sale_date_time,
		vehicle_make,
		vehicle_model,
		vehicle_trim,
		vehicle_year,
		application_constants->dictionary,
		0 /* not omit_money */,
		workorder ) )
	{
		printf( "<h3>Please choose a customer sale.</h3>\n" );
		fclose( output_stream );
		document_tag_close();
		exit( 0 );
	}

	fclose( output_stream );

	output_directory =
		appaserver_link_source_directory(
			appaserver_parameter_file->
				document_root,
			application_name );

	sprintf( sys_string,
		 "cd %s && pdflatex %s 1>&2",
		 output_directory,
		 output_filename );
/*
	sprintf( sys_string,
		 "cd %s && pdflatex %s >/dev/null 2>&1",
		 output_directory,
		 output_filename );
*/

/* fprintf( stderr, "%s\n", sys_string ); */

	if ( system( sys_string ) ) {};

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

	output_invoice_window(
			application_name,
			ftp_output_filename,
			process_id,
			process_name );

	document_tag_close();
	return 0;
}

void output_invoice_window(
			char *application_name,
			char *ftp_output_filename,
			int process_id,
			char *process_name )
{
	char window_label[ 128 ];
	char buffer[ 128 ];

	sprintf( window_label, "latex_invoice_window_%d", process_id );

	printf(
"<body bgcolor=\"%s\" onload=\"window.open('%s','%s','menubar=yes,resizeable=yes,scrollbars=yes,status=no,toolbar=no,location=no', 'false');\">\n",
			application_background_color(
				application_name ),
			ftp_output_filename,
			window_label );

	printf( "<h1>%s<br>",
		format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	printf( "</h1>\n" );
	printf( "<a href='%s' target=%s>Press to view document.</a>\n",
		ftp_output_filename,
		window_label );
	fflush( stdout );
}

boolean build_latex_invoice(	FILE *output_stream,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year,
				DICTIONARY *application_constants_dictionary,
				boolean omit_money,
				boolean workorder )
{
	LATEX_INVOICE *latex_invoice;
	ENTITY_SELF *self;
	char *todays_date;
	LIST *extra_label_list = list_new();
	char extra_label[ 256 ];
	char *completed_date_time;
	AUTOREPAIR_CUSTOMER_SALE *customer_sale;
	char title[ 128 ];

	if ( ! ( self = entity_self_load( application_name ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot load from SELF.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( workorder )
	{
		sprintf(	title,
				"%s Workorder",
				self->entity->full_name );
	}
	else
	{
		sprintf(	title,
				"%s Invoice",
				self->entity->full_name );
	}

	completed_date_time =
		customer_sale_fetch_completed_date_time(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !completed_date_time ) return 0;

	customer_sale =
		autorepair_customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				completed_date_time,
				vehicle_make,
				vehicle_model,
				vehicle_trim,
				vehicle_year );

	if ( customer_sale->odometer_miles )
	{
		sprintf( extra_label,
			 "Odometer miles: %d",
			 customer_sale->odometer_miles );
		list_append_pointer(	extra_label_list,
					strdup( extra_label ) );
	}

	if ( *customer_sale->mechanic_full_name )
	{
		sprintf( extra_label,
			 "Mechanic: %s",
			 customer_sale->mechanic_full_name );
		list_append_pointer(	extra_label_list,
					strdup( extra_label ) );
	}

	todays_date = pipe2string( "now.sh full 0" );

	latex_invoice =
		latex_invoice_new(
			strdup( todays_date ),
			self->entity->full_name,
			self->entity->street_address,
			self->entity->city,
			self->entity->state_code,
			self->entity->zip_code,
			self->entity->phone_number,
			self->entity->email_address );

	latex_invoice->instructions = customer_sale->symptom;
	latex_invoice->extra_label_list = extra_label_list;
	latex_invoice->omit_money = omit_money;

	latex_invoice_output_header( output_stream );

	latex_invoice->invoice_customer =
		generate_invoice_invoice_customer(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			vehicle_make,
			vehicle_model,
			vehicle_trim,
			vehicle_year );

	if ( !populate_line_item_list(
			latex_invoice->invoice_line_item_list,
			application_name,
			full_name,
			street_address,
			sale_date_time,
			customer_sale->completed_date_time ) )
	{
		printf( "<H3>Error: no line items for this invoice.</h3>\n" );
		document_tag_close();
		exit( 0 );
	}

	latex_invoice->exists_discount_amount =
		latex_invoice_exists_discount_amount(
			latex_invoice->invoice_line_item_list );

	latex_invoice_output_invoice_header(
			output_stream,
			latex_invoice->invoice_key,
			latex_invoice->invoice_date,
			latex_invoice->line_item_key_heading,
			latex_invoice->invoice_company,
			latex_invoice->invoice_customer,
			latex_invoice->exists_discount_amount,
			title,
			latex_invoice->omit_money,
		 	application_constants_safe_fetch(
				application_constants_dictionary,
				LOGO_FILENAME_KEY ),
			latex_invoice->instructions,
			latex_invoice->extra_label_list,
			"Description" /* first_column_label */,
			"Extended" /* last_column_label */,
			"Customer" /* customer_label */ );

	latex_invoice->quantity_decimal_places =
		latex_invoice_quantity_decimal_places(
			latex_invoice_each_quantity_integer(
				latex_invoice->invoice_line_item_list ),
			LATEX_INVOICE_QUANTITY_DECIMAL_PLACES );

	latex_invoice_output_invoice_line_items(
		output_stream,
		latex_invoice->invoice_line_item_list,
		latex_invoice->exists_discount_amount,
		latex_invoice->omit_money,
		latex_invoice->quantity_decimal_places );

	if ( !omit_money )
	{
		latex_invoice_output_invoice_footer(
			output_stream,
			latex_invoice->invoice_customer->extension_total,
			latex_invoice->invoice_customer->sales_tax,
			latex_invoice->invoice_customer->shipping_charge,
			latex_invoice->invoice_customer->total_payment,
			latex_invoice->line_item_key_heading,
			latex_invoice->exists_discount_amount,
			workorder /* is_estimate */ );
	}

	latex_invoice_output_footer(
		output_stream,
		workorder /* with_customer_signature */ );

	return 1;
}

LATEX_INVOICE_CUSTOMER *generate_invoice_invoice_customer(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *vehicle_make,
				char *vehicle_model,
				char *vehicle_trim,
				int vehicle_year )
{
	LATEX_INVOICE_CUSTOMER *invoice_customer;
	char invoice_key[ 128 ];
	char customer_service_key[ 128 ];
	double sales_tax;
	double total_payment;

	sprintf(invoice_key,
		"%s %s %s",
		full_name,
		street_address,
		sale_date_time );

	sprintf( customer_service_key,
		 "%s %s %s %d",
		 vehicle_make,
		 vehicle_model,
		 vehicle_trim,
		 vehicle_year );

	sales_tax = autorepair_get_sales_tax(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	total_payment =
		autorepair_get_total_payment(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	invoice_customer = latex_invoice_customer_new(
					strdup( invoice_key ),
					strdup( full_name ),
					strdup( street_address ),
					strdup( "" ),
					strdup( "" ),
					strdup( "" ),
					strdup( "" ),
					strdup( customer_service_key ),
					sales_tax,
					0.0 /* shipping_charge */,
					total_payment );

	return invoice_customer;

}

double populate_line_item_list(
			LIST *invoice_line_item_list,
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *completed_date_time )
{
	double extension_total = 0.0;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char inventory_key[ 128 ];
	char quantity_string[ 128 ];
	char retail_price_string[ 128 ];
	char discount_amount_string[ 128 ];

	sprintf( sys_string,
		 "select_invoice_lineitems.sh %s '%s' '%s' '%s' '%s'",
		 application_name,
		 full_name,
		 street_address,
		 sale_date_time,
		 (completed_date_time) ? completed_date_time : "" );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( inventory_key, '^', input_buffer, 0 );
		piece( quantity_string, '^', input_buffer, 1 );
		piece( retail_price_string, '^', input_buffer, 2 );
		piece( discount_amount_string, '^', input_buffer, 3 );

		extension_total += latex_invoice_append_line_item(
					invoice_line_item_list,
					(char *)0 /* line_item_key */,
					strdup( inventory_key ),
					atof( quantity_string ),
					atof( retail_price_string ),
					atof( discount_amount_string ) );
	}

	pclose( input_pipe );
	return extension_total;
}

boolean generate_invoice_vehicle_information(
				char **vehicle_make,
				char **vehicle_model,
				char **vehicle_trim,
				int *vehicle_year,
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *results;
	char piece_buffer[ 128 ];
	char *select;
	char *folder_name;
	char *where;

	select = "vehicle_make,vehicle_model,vehicle_trim,vehicle_year";

	folder_name = "customer_sale";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder_name,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*vehicle_make = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*vehicle_model = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*vehicle_trim = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	*vehicle_year = atoi( piece_buffer );

	return 1;

}

