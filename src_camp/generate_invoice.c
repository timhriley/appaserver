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
#include "ledger.h"
#include "entity.h"
#include "camp.h"
#include "email.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define FROM_ADDRESS	"donner@cloudacus.com"
#define SUBJECT		"Invoice"
#define MESSAGE		"Here is your invoice."

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
				pid_t process_id );

void generate_invoice_email_send(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				pid_t process_id );

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

LATEX_INVOICE_CUSTOMER *generate_invoice_customer(
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

	application_name = environ_application_name( argv[ 0 ] );

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

	if ( !*output_option
	||   strcmp( output_option, "output_option" ) == 0
	||   strcmp( output_option, "invoice_output_option" ) == 0 )
	{
		output_option = "PDF";
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h1>%s</h1>\n",
		format_initial_capital( title, process_name ) );
	fflush( stdout );
	if ( system( timlib_system_date_string() ) ){};
	fflush( stdout );

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
}

void output_invoice_window(
			char *application_name,
			char *ftp_output_filename,
			int process_id )
{
	char window_label[ 128 ];

	sprintf( window_label, "latex_invoice_window_%d", process_id );

	printf(
"<body bgcolor=\"%s\" onload=\"window.open('%s','%s','menubar=yes,resizeable=yes,scrollbars=yes,status=no,toolbar=no,location=no', 'false');\">\n",
			application_background_color(
				application_name ),
			ftp_output_filename,
			window_label );

	printf( "<a href='%s' target=%s>Press to view document.</a>\n",
		ftp_output_filename,
		window_label );
	fflush( stdout );

}

boolean build_latex_invoice(	FILE *output_stream,
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				DICTIONARY *application_constants_dictionary,
				double extended_price_total )
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
				self->entity->city,
				self->entity->state_code,
				self->entity->zip_code,
				self->entity->phone_number,
				self->entity->email_address );

	latex_invoice->extended_price_total = extended_price_total;

	if ( ! ( latex_invoice->invoice_customer =
			generate_invoice_customer(
				camp->camp_enrollment ) ) )
	{
		return 0;
	}

	if ( !generate_invoice_populate_line_item_list(
			latex_invoice->invoice_line_item_list,
			camp->enrollment_cost,
			camp->camp_enrollment ) ) )
	{
		printf( "<H3>Error: No camp to generate.</h3>\n" );
		document_close();
		exit( 0 );
	}

	latex_invoice_output_header( output_stream );

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
		"Tuition" /* last_column_label */,
		"Student" /* customer_label */ );

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

	latex_invoice_output_invoice_footer(
		output_stream,
		latex_invoice->extended_price_total,
		latex_invoice->sales_tax,
		latex_invoice->shipping_charge,
		latex_invoice->total_payment,
		latex_invoice->line_item_key_heading,
		latex_invoice->exists_discount_amount,
		0 /* not is_estimate */ );

	latex_invoice_output_footer(
		output_stream,
		0 /* not with_customer_signature */ );

	return 1;
}

LATEX_INVOICE_CUSTOMER *generate_invoice_customer(
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

}

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

}

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
		if ( ! ( output_filename =
				generate_invoice_PDF(
					&ftp_output_filename,
					application_name,
					process_name,
					document_root_directory,
					camp_begin_date,
					camp_title,
					full_name,
					street_address,
					process_id ) ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: generate_invoice_PDF() failed.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

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
				street_address,
				process_id );
	}

}

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
		application_constants_dictionary(
			application_name );

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	appaserver_link_file->extension = "tex";

	output_filename =
		appaserver_link_output_filename(
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
		appaserver_link_source_directory(
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

	return appaserver_link_output_filename(
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

}

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
	ENTITY *entity;
	char *sendmail_command;
	char *sendmail_string;

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

	if ( ! ( entity = entity_fetch(
				application_name,
				full_name,
				street_address ) ) )
	{
		printf( "<h3>Error: cannot fetch %s/%s</h3>\n",
			full_name,
			street_address );
		return;
	}

	/* Returns heap memory. */
	/* -------------------- */
	sendmail_command =
		email_sendmail_command(
			entity->email_address /* to_address */,
			SUBJECT,
			output_filename
				/* attachment_filename */ );

	/* Returns heap memory. */
	/* -------------------- */
	sendmail_string =
		email_sendmail_string(
			FROM_ADDRESS,
			entity->email_address /* to_address */,
			SUBJECT,
			"Here is your invoice." /* message */,
			(char *)0 /* reply_to */,
			entity->full_name );

	printf( "<p>%s\n", sendmail_command );
	printf( "<p>%s\n", sendmail_string );

}

void generate_invoice_email_send(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				pid_t process_id )
{
	char *output_filename;
	char *ftp_output_filename;
	ENTITY *entity;

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

	if ( ! ( entity = entity_fetch(
				application_name,
				full_name,
				street_address ) ) )
	{
		printf( "<h3>Error: cannot fetch %s/%s</h3>\n",
			full_name,
			street_address );
		return;
	}

	email_sendmail(	FROM_ADDRESS,
			entity->email_address /* to_address */,
			SUBJECT,
			MESSAGE,
			(char *)0 /* reply_to */,
			entity->full_name,
			output_filename /* attachment_filename */ );

	printf( "<h3>Message send.</h3>\n" );

}

