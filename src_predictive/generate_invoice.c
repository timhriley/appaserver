/* $APPASERVER_HOME/src_predictive/generate_invoice.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/
/* Note: links to generate_workorder					*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "invoice_latex.h"
#include "application_constant.h"
#include "date_convert.h"
#include "entity.h"
#include "sale.h"
#include "customer_payment.h"
#include "entity_self.h"
#include "appaserver_link.h"

boolean populate_line_item_list(
		LIST *invoice_line_item_list,
		char *application_name,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *completed_date_time );

LATEX_INVOICE_CUSTOMER *generate_invoice_customer(
		char *full_name,
		char *street_address,
		char *sale_date_time );

boolean build_latex_invoice(
		FILE *output_stream,
		char *application_name,
		char *full_name,
		char *street_address,
		char *sale_date_time,
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
	APPASERVER_PARAMETER *appaserver_parameter;
	int process_id = getpid();
	FILE *output_stream;
	char *output_filename;
	char *output_directory;
	char *ftp_output_filename;
	APPLICATION_CONSTANT *application_constant;
	boolean workorder = 0;
	char sys_string[ 1024 ];
	APPASERVER_LINK *appaserver_link;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
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

	appaserver_parameter = appaserver_parameter_new();

	application_constant = application_constant_new();
	application_constant->dictionary =
		application_constant_dictionary(
			application_name );

	appaserver_link =
		appaserver_link_new(
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
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			(char *)0 /* extension */ );

	appaserver_link->extension = "tex";

	output_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );

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

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	if ( !build_latex_invoice(
			output_stream,
			application_name,
			full_name,
			street_address,
			sale_date_time,
			application_constants->dictionary,
			0 /* not omit_money */,
			workorder ) )
	{
		printf( "<h3>Please choose a customer sale.</h3>\n" );
		fclose( output_stream );
		document_close();
		exit( 0 );
	}


	fclose( output_stream );

	output_directory =
		appaserver_link_working_directory(
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

	appaserver_link->extension = "pdf";

	ftp_output_filename =
		appaserver_link_prompt_filename(
			appaserver_link_prompt_link_half(
				appaserver_link->prepend_http,
				appaserver_link->http_prefix,
				appaserver_link->server_address ),
			appaserver_link->application_name,
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			appaserver_link->extension );

	output_invoice_window(
			application_name,
			ftp_output_filename,
			process_id,
			process_name );

	document_close();

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
				DICTIONARY *application_constants_dictionary,
				boolean omit_money,
				boolean workorder )
{
	LATEX_INVOICE *latex_invoice;
	ENTITY_SELF *self;
	char *todays_date;
	SALE *sale;
	char title[ 128 ];

	if ( ! ( self = entity_self_load() ) )
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

	sale =
		/* Returns sale_steady_state() */
		/* --------------------------- */
		sale_fetch(
			full_name,
			street_address,
			sale_date_time );

	if ( !sale->completed_date_time ) return 0;

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

	latex_invoice->omit_money = omit_money;
	latex_invoice->extended_price_total = sale->sale_gross_revenue;

	if ( ! ( latex_invoice->invoice_customer =
			generate_invoice_customer(
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		return 0;
	}

	latex_invoice_output_header( output_stream );

	if ( !populate_line_item_list(
			latex_invoice->invoice_line_item_list,
			application_name,
			full_name,
			street_address,
			sale_date_time,
			sale->completed_date_time ) )
	{
		printf( "<H3>Error: no line items for this invoice.</h3>\n" );
		document_close();
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
		latex_invoice->invoice_self,
		latex_invoice->invoice_customer,
		latex_invoice->customer_service_key,
		latex_invoice->exists_discount_amount,
		title,
		latex_invoice->omit_money,
		application_constants_safe_fetch(
			application_constants_dictionary,
			PREDICTIVE_LOGO_FILENAME_KEY ),
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

	latex_invoice_output_line_item_list(
		output_stream,
		latex_invoice->
			invoice_line_item_list,
		latex_invoice->exists_discount_amount,
		latex_invoice->omit_money,
		latex_invoice->quantity_decimal_places );

	if ( !latex_invoice->omit_money )
	{
		latex_invoice_output_invoice_footer(
			output_stream,
			latex_invoice->extended_price_total,
			latex_invoice->sales_tax,
			latex_invoice->shipping_charge,
			latex_invoice->total_payment,
			latex_invoice->line_item_key_heading,
			latex_invoice->exists_discount_amount,
			workorder /* is_estimate */ );
	}

	latex_invoice_output_footer(
		output_stream,
		workorder /* with_customer_signature */ );

	return 1;
}

LATEX_INVOICE_CUSTOMER *generate_invoice_customer(
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	LATEX_INVOICE_CUSTOMER *invoice_customer;
	char invoice_key[ 128 ];

	sprintf(invoice_key,
		"%s %s %s",
		full_name,
		street_address,
		sale_date_time );

	invoice_customer =
		latex_invoice_customer_new(
			strdup( full_name ),
			strdup( street_address ),
			(char *)0 /* city */,
			(char *)0 /* state_code */,
			(char *)0 /* zip_code */,
			(char *)0 /* customer_phone_number */,
			(char *)0 /* customer_email_address */ );

	return invoice_customer;
}

boolean populate_line_item_list(
			LIST *invoice_line_item_list,
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *completed_date_time )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char inventory_key[ 128 ];
	char quantity_string[ 128 ];
	char retail_price_string[ 128 ];
	char discount_amount_string[ 128 ];
	char extended_price_string[ 128 ];
	boolean got_one = 0;

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
		piece( extended_price_string, '^', input_buffer, 4 );

		latex_invoice_line_item_set(
			invoice_line_item_list,
				(char *)0 /* line_item_key */,
				strdup( inventory_key ),
				atof( quantity_string ),
				atof( retail_price_string ),
				atof( discount_amount_string ),
				atof( extended_price_string ) );
		got_one = 1;
	}

	pclose( input_pipe );
	return got_one;
}

