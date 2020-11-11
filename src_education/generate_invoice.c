/* $APPASERVER_HOME/src_education/generate_invoice.c			*/
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
#include "column.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "latex_invoice.h"
#include "application_constants.h"
#include "entity.h"
#include "entity_self.h"
#include "enrollment.h"
#include "registration.h"
#include "email.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define FROM_ADDRESS	"TNT@cloudacus.com"
#define SUBJECT		"Invoice"
#define MESSAGE		"Here is your invoice."

/* Prototypes */
/* ---------- */
void generate_invoice_email_display(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
				pid_t process_id );

void generate_invoice_email_send(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
				pid_t process_id );

/* Returns output_filename */
/* ----------------------- */
char *generate_invoice_PDF(	char **ftp_output_filename,
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
				int process_id );

void generate_invoice(	char *application_name,
			char *process_name,
			char *document_root_directory,
			char *full_name,
			char *street_address,
			char *season_name,
			int year,
			char *output_option );

LIST *generate_invoice_line_item_list(
			LIST *enrollment_list );

LATEX_INVOICE_CUSTOMER *generate_invoice_customer(
			REGISTRATION *registration );

boolean build_latex_invoice(
			FILE *output_stream,
			char *full_name,
			char *street_address,
			char *season_name,
			int year,
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
	char *full_name;
	char *street_address;
	char *season_name;
	int year;
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
"Usage: %s process full_name street_address course_name year output_option\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	season_name = argv[ 4 ];
	year = atoi( argv[ 5 ] );
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
				full_name,
				street_address,
				season_name,
				year,
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
			application_get_background_color(
				application_name ),
			ftp_output_filename,
			window_label );

	printf( "<a href='%s' target=%s>Press to view document.</a>\n",
		ftp_output_filename,
		window_label );
	fflush( stdout );

}

boolean build_latex_invoice(	FILE *output_stream,
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
				DICTIONARY *application_constants_dictionary )
{
	LATEX_INVOICE *latex_invoice;
	ENTITY_SELF *self;
	char *todays_date;
	REGISTRATION *registration;
	char title[ 128 ];

	if ( ! ( self = entity_self_load() ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: entity_self_load() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf(	title,
			"%s Invoice",
			self->entity->full_name );

	if ( ! ( registration =
			registration_fetch(
				full_name,
				street_address,
				season_name,
				year,
				1 /* fetch_enrollment_list */ ) ) )
	{
		return 0;
	}

	todays_date = pipe2string( "now.sh full 0" );

	latex_invoice =
		latex_invoice_new(
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
			generate_invoice_customer(
				registration ) ) )
	{
		return 0;
	}

	latex_invoice_output_header( output_stream );

	if ( ! ( latex_invoice->
			invoice_customer->
			invoice_line_item_list =
				generate_invoice_line_item_list(
					registration->
					     registration_enrollment_list ) ) )
	{
		printf( "<H3>Error: Registration has no enrollments.</h3>\n" );
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
}

LATEX_INVOICE_CUSTOMER *generate_invoice_customer(
			REGISTRATION *registration )
{
	LATEX_INVOICE_CUSTOMER *invoice_customer;
	char registration_date[ 16 ];
	char invoice_key[ 128 ];

	if ( !registration )
	{
		return (LATEX_INVOICE_CUSTOMER *)0;
	}

	sprintf(invoice_key,
		"%s %s %s",
		registration->student_full_name,
		registration->street_address,
		column( registration_date,
			0,
			registration->registration_date_time ) );

	invoice_customer =
		latex_invoice_customer_new(
			strdup( invoice_key ),
			strdup( registration->student_full_name ),
			strdup( registration->street_address ),
			strdup( "" ) /* suite_number */,
			strdup( "" ) /* city */,
			strdup( "" ) /* state_code */,
			strdup( "" ) /* zip_code */,
			(char *)0 /* customer_service_key */,
			0.0 /* sales_tax */,
			0.0 /* shipping_charge */,
			registration->
				registration_tuition_payment_total
					/* total_payment */ );

	return invoice_customer;
}

LIST *generate_invoice_line_item_list(
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;
	LIST *line_item_list;

	if ( !list_rewind( enrollment_list ) ) return (LIST *)0;

	line_item_list = list_new();

	do {
		enrollment = list_get( enrollment_list );

		latex_invoice_append_line_item(
			line_item_list,
			(char *)0 /* item_key */,
			enrollment->offering->course->course_name /* item */,
			1.0 /* quantity */,
			enrollment->offering->course_price /* retail_price */,
			0.0 /* discount_amount */ );

	} while ( list_next( enrollment_list ) );

	return line_item_list;
}

void generate_invoice(		char *application_name,
				char *process_name,
				char *document_root_directory,
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
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
					full_name,
					street_address,
					season_name,
					year,
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
				full_name,
				street_address,
				season_name,
				year,
				process_id );
	}
	else
	if ( strcmp( output_option, "email_send" ) == 0 )
	{
		generate_invoice_email_send(
				application_name,
				process_name,
				document_root_directory,
				full_name,
				street_address,
				season_name,
				year,
				process_id );
	}
}

/* Returns output_filename */
/* ----------------------- */
char *generate_invoice_PDF(	char **ftp_output_filename,
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
				int process_id )
{
	FILE *output_stream;
	char *output_filename;
	char *output_directory;
	APPLICATION_CONSTANTS *application_constants;
	char sys_string[ 1024 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

fprintf(stderr,
	"%s/%s()/%d\n",
	__FILE__,
	__FUNCTION__,
	__LINE__ );

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

fprintf(stderr,
	"%s/%s()/%d\n",
	__FILE__,
	__FUNCTION__,
	__LINE__ );

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

fprintf(stderr,
	"%s/%s()/%d\n",
	__FILE__,
	__FUNCTION__,
	__LINE__ );

	if ( !build_latex_invoice(
			output_stream,
			full_name,
			street_address,
			season_name,
			year,
			application_constants->dictionary ) )
	{
		printf( "<h3>Please choose a Registration.</h3>\n" );
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
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
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
			full_name,
			street_address,
			season_name,
			year,
			process_id );

	if ( ! ( entity = entity_fetch(
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
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
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
			full_name,
			street_address,
			season_name,
			year,
			process_id );

	if ( ! ( entity = entity_fetch(
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

