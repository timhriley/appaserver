/* $APPASERVER_HOME/src_camp/generate_invoice.c				*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
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
#include "ledger.h"
#include "entity.h"
#include "camp.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void generate_invoice_email_display(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				int process_id );

void generate_invoice_email_send(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address );

/* Returns output_filename */
/* ----------------------- */
char *generate_invoice_PDF(	char **ftp_output_filename,
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				int process_id );

void generate_invoice(		char *application_name,
				char *process_name,
				char *document_root_directory,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				char *output_option );

double generate_invoice_populate_line_item_list(
				LIST *invoice_line_item_list,
				double enrollment_cost,
				CAMP_ENROLLMENT *camp_enrollment );

LATEX_INVOICE_CUSTOMER *generate_invoice_get_invoice_customer(
				CAMP_ENROLLMENT *camp_enrollment );

boolean build_latex_invoice(	FILE *output_stream,
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				DICTIONARY *application_constants_dictionary );

void output_invoice_window(
				char *application_name,
				char *ftp_output_filename,
				int process_id );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char title[ 128 ];
	char *camp_begin_date;
	char *camp_title;
	char *full_name;
	char *street_address;
	char *output_option;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s process camp_begin_date camp_title full_name street_address output_option\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	camp_begin_date = argv[ 2 ];
	camp_title = argv[ 3 ];
	full_name = argv[ 4 ];
	street_address = argv[ 5 ];
	output_option = argv[ 6 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h1>%s</h1>\n",
		format_initial_capital( title, process_name ) );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	fflush( stdout );
	printf( "</h2>\n" );

	generate_invoice(	application_name,
				process_name,
				appaserver_parameter_file->
					document_root,
				camp_begin_date,
				camp_title,
				full_name,
				street_address,
				output_option );

	document_close();

	exit( 0 );

} /* main() */

void output_invoice_window(
			char *application_name,
			char *ftp_output_filename,
			int process_id )
{
	char window_label[ 128 ];

	sprintf( window_label, "latex_invoice_window_%d", process_id );

	printf(
"<body bgcolor=\"%s\" onload=\"window.open('%s','%s','menubar=yes,resizeable=yes,scrollbars=yes,status=no,toolbar=no,location=no', 'false');\">\n",
			application_get_background_color(
				application_name ),
			ftp_output_filename,
			window_label );

/*
	printf( "<h1>%s<br>",
		format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	printf( "</h1>\n" );
*/
	printf( "<a href='%s' target=%s>Press to view document.</a>\n",
		ftp_output_filename,
		window_label );
	fflush( stdout );

} /* output_invoice_window() */

boolean build_latex_invoice(	FILE *output_stream,
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				DICTIONARY *application_constants_dictionary )
{
	LATEX_INVOICE *latex_invoice;
	ENTITY_SELF *self;
	char *todays_date;
	CAMP *camp;
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

	sprintf(	title,
			"%s Invoice",
			self->entity->full_name );

	if ( ! ( camp =
			camp_fetch(
				application_name,
				camp_begin_date,
				camp_title ) ) )
	{
		return 0;
	}

	if ( ! ( camp->camp_enrollment =
			camp_enrollment_fetch(
				application_name,
				camp->camp_begin_date,
				camp->camp_title,
				full_name,
				street_address,
				camp->enrollment_cost ) ) )
	{
		return 0;
	}

	todays_date = pipe2string( "now.sh full 0" );

	latex_invoice = latex_invoice_new(
				strdup( todays_date ),
				self->entity->full_name,
				self->entity->street_address,
				(char *)0 /* unit */,
				self->entity->city,
				self->entity->state_code,
				self->entity->zip_code,
				self->entity->phone_number,
				self->entity->email_address,
				strdup( "" ) /* line_item_key_heading */,
				(char *)0 /* instructions */,
				(LIST *)0 /* extra_label_list */ );

	if ( ! ( latex_invoice->invoice_customer =
			generate_invoice_get_invoice_customer(
				camp->camp_enrollment ) ) )
	{
		return 0;
	}

	latex_invoice_output_header( output_stream );

	if ( ! ( latex_invoice->invoice_customer->extension_total =
			generate_invoice_populate_line_item_list(
				latex_invoice->
					invoice_customer->
					invoice_line_item_list,
				camp->enrollment_cost,
				camp->camp_enrollment ) ) )
	{
		printf( "<H3>Error: No camp to generate.</h3>\n" );
		document_close();
		exit( 0 );
	}

	latex_invoice_output_invoice_header(
		output_stream,
		latex_invoice->invoice_date,
		latex_invoice->line_item_key_heading,
		&latex_invoice->invoice_company,
		latex_invoice->invoice_customer,
		latex_invoice->
			invoice_customer->
			exists_discount_amount,
		title,
		latex_invoice->omit_money,
	 	application_constants_safe_fetch(
			application_constants_dictionary,
			PREDICTIVE_LOGO_FILENAME_KEY ),
		latex_invoice->instructions,
		latex_invoice->extra_label_list );

	if ( latex_invoice_each_quantity_integer(
		latex_invoice->invoice_customer->invoice_line_item_list ) )
	{
		latex_invoice->quantity_decimal_places = 0;
	}

	latex_invoice_output_invoice_line_items(
		output_stream,
		latex_invoice->
			invoice_customer->
			invoice_line_item_list,
		latex_invoice->
			invoice_customer->
			exists_discount_amount,
		latex_invoice->omit_money,
		latex_invoice->quantity_decimal_places );

	latex_invoice_output_invoice_footer(
			output_stream,
			latex_invoice->invoice_customer->extension_total,
			latex_invoice->invoice_customer->sales_tax,
			latex_invoice->invoice_customer->shipping_charge,
			latex_invoice->invoice_customer->total_payment,
			latex_invoice->line_item_key_heading,
				latex_invoice->
					invoice_customer->
					exists_discount_amount,
			0 /* not is_estimate */ );

	latex_invoice_output_footer(
		output_stream,
		0 /* not with_customer_signature */ );

	/* Needs all strdups() */
	/* ------------------- */
	/* latex_invoice_company_free( &latex_invoice->invoice_company ); */

	latex_invoice_free( latex_invoice );

	return 1;

} /* build_latex_invoice() */

LATEX_INVOICE_CUSTOMER *generate_invoice_get_invoice_customer(
				CAMP_ENROLLMENT *camp_enrollment )
{
	LATEX_INVOICE_CUSTOMER *invoice_customer;
	char *transaction_date_time;
	char invoice_key[ 128 ];

	if ( !camp_enrollment
	||   !camp_enrollment->camp_enrollment_transaction )
	{
		return (LATEX_INVOICE_CUSTOMER *)0;
	}

	transaction_date_time = 
		camp_enrollment->
			camp_enrollment_transaction->
			transaction_date_time;

	sprintf(invoice_key,
		"%s %s %s",
		camp_enrollment->full_name,
		camp_enrollment->street_address,
		transaction_date_time );

	invoice_customer =
		latex_invoice_customer_new(
			strdup( invoice_key ),
			strdup( camp_enrollment->full_name ),
			strdup( camp_enrollment->street_address ),
			strdup( "" )
				/* suite_number */,
			strdup( camp_enrollment->city ),
			strdup( camp_enrollment->state_code ),
			strdup( camp_enrollment->zip_code ),
			(char *)0 /* customer_service_key */,
			0.0 /* sales_tax */,
			0.0 /* shipping_charge */,
			camp_enrollment->
				camp_enrollment_total_payment_amount );

	return invoice_customer;

} /* generate_invoice_get_invoice_customer() */

double generate_invoice_populate_line_item_list(
			LIST *invoice_line_item_list,
			double enrollment_cost,
			CAMP_ENROLLMENT *camp_enrollment )
{
	SERVICE_ENROLLMENT *service_enrollment;
	LIST *l;
	double extension_total;

	if ( !camp_enrollment ) return 0.0;

	/* Returns (quantity * retail_price ) - discount_amount */
	/* ---------------------------------------------------- */
	extension_total = latex_invoice_append_line_item(
				invoice_line_item_list,
				(char *)0 /* item_key */,
				"Camp Enrollment" /* item */,
				1.0 /* quantity */,
				enrollment_cost /* retail_price */,
				0.0 /* discount_amount */ );

	if ( !list_rewind( camp_enrollment->
				camp_enrollment_service_enrollment_list ) )
	{
		return extension_total;
	}

	l = camp_enrollment->camp_enrollment_service_enrollment_list;

	do {
		service_enrollment = list_get( l );

		/* Returns (quantity * retail_price ) - discount_amount */
		/* ---------------------------------------------------- */
		extension_total +=
			latex_invoice_append_line_item(
				invoice_line_item_list,
				(char *)0 /* item_key */,
				service_enrollment->service_name /* item */,
				(double)service_enrollment->purchase_quantity
					/* quantity */,
				service_enrollment->service_price
					/* retail_price */,
				0.0 /* discount_amount */ );

	} while ( list_next( l ) );

	return extension_total;

} /* generate_invoice_populate_line_item_list() */

void generate_invoice(		char *application_name,
				char *process_name,
				char *document_root_directory,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				char *output_option )
{
	int process_id = getpid();
	char *ftp_output_filename;
	char *output_filename;

	if ( strcmp( output_option, "PDF" ) == 0 )
	{
		output_filename =
			generate_invoice_PDF(
				&ftp_output_filename,
				application_name,
				process_name,
				document_root_directory,
				camp_begin_date,
				camp_title,
				full_name,
				street_address,
				process_id );

		output_invoice_window(
			application_name,
			ftp_output_filename,
			process_id );

	}
	else
	if ( strcmp( output_option, "email_display" ) == 0 )
	{
		generate_invoice_email_display(
				application_name,
				process_name,
				document_root_directory,
				camp_begin_date,
				camp_title,
				full_name,
				street_address,
				process_id );
	}
	else
	if ( strcmp( output_option, "email_send" ) == 0 )
	{
		generate_invoice_email_send(
				application_name,
				process_name,
				document_root_directory,
				camp_begin_date,
				camp_title,
				full_name,
				street_address );
	}

} /* generate_invoice() */

/* Returns output_filename */
/* ----------------------- */
char *generate_invoice_PDF(	char **ftp_output_filename,
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				int process_id )
{
	FILE *output_stream;
	char *output_filename;
	char *output_directory;
	APPLICATION_CONSTANTS *application_constants;
	char sys_string[ 1024 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
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

	if ( !build_latex_invoice(
			output_stream,
			application_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address,
			application_constants->dictionary ) )
	{
		printf( "<h3>Please choose a Camp Enrollment.</h3>\n" );
		fclose( output_stream );
		document_close();
		exit( 0 );
	}

	fclose( output_stream );

	output_directory =
		appaserver_link_get_source_directory(
			document_root_directory,
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

	*ftp_output_filename =
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

	return output_filename;

} /* generate_invoice_PDF() */

void generate_invoice_email_display(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				int process_id )
{
	char *output_filename;
	char *ftp_output_filename;
	char pdf_output_filename[ 1024 ];
	int str_len;

	output_filename =
		generate_invoice_PDF(
			&ftp_output_filename,
			application_name,
			process_name,
			document_root_directory,
			camp_begin_date,
			camp_title,
			full_name,
			street_address,
			process_id );

	strcpy( pdf_output_filename, output_filename );


	str_len = strlen( pdf_output_filename );

	sprintf( pdf_output_filename + str_len - 3,
		 "pdf" );

	printf( "<p>Generated %s\n", pdf_output_filename );

} /* generate_invoice_email_display() */

void generate_invoice_email_send(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address )
{
} /* generate_invoice_email_send() */

