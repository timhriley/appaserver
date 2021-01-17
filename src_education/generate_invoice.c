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
#include "String.h"
#include "list.h"
#include "table.h"
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
#include "table.h"

/* Constants */
/* --------- */
#define SUBJECT		"Invoice"
#define MESSAGE		"Here is your invoice."

/* Prototypes */
/* ---------- */
void generate_invoice_amount_due(
			char *application_name,
			char *document_root_directory,
			char *season_name,
			int year,
			char *output_option,
			ENTITY_SELF *self );

void generate_invoice_email_display(
			char *application_name,
			char *document_root_directory,
			char *full_name,
			char *street_address,
			char *season_name,
			int year,
			pid_t process_id,
			ENTITY_SELF *self );

void generate_invoice_email_send(
			char *application_name,
			char *document_root_directory,
			char *full_name,
			char *street_address,
			char *season_name,
			int year,
			pid_t process_id,
			ENTITY_SELF *self );

/* Returns output_filename */
/* ----------------------- */
char *generate_invoice_PDF(
			char **ftp_output_filename,
			char *application_name,
			char *document_root_directory,
			char *full_name,
			char *street_address,
			char *season_name,
			int year,
			int process_id,
			ENTITY_SELF *self );

void generate_invoice(	char *application_name,
			char *document_root_directory,
			char *full_name,
			char *street_address,
			char *season_name,
			int year,
			char *output_option,
			ENTITY_SELF *self );

boolean generate_invoice_line_item_list(
			LIST *invoice_line_item_list,
			LIST *enrollment_list );

LATEX_INVOICE_CUSTOMER *generate_invoice_customer(
			REGISTRATION *registration );

boolean build_latex_invoice(
			FILE *output_stream,
			char *full_name,
			char *street_address,
			char *season_name,
			int year,
			char *predictive_logo_filename,
			ENTITY_SELF *self );

void output_invoice_window(
			char *ftp_output_filename,
			int process_id,
			char *full_name );

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
	ENTITY_SELF *self;

	application_name = environ_exit_application_name( argv[ 0 ] );

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

	if ( ! ( self = entity_self_load() ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: entity_self_load() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( strcmp( full_name, "full_name" ) == 0 )
	{
		generate_invoice_amount_due(
			application_name,
			appaserver_parameter_file->
				document_root,
			season_name,
			year,
			output_option,
			self );
	}
	else
	{
		generate_invoice(
			application_name,
			appaserver_parameter_file->
				document_root,
			full_name,
			street_address,
			season_name,
			year,
			output_option,
			self );
	}

	document_close();

	exit( 0 );
}

void generate_invoice_amount_due(
			char *application_name,
			char *document_root_directory,
			char *season_name,
			int year,
			char *output_option,
			ENTITY_SELF *self )
{
	LIST *registration_list;
	REGISTRATION *registration;
	char where[ 1024 ];

	sprintf(where,
		"season_name = '%s' and year = %d and invoice_amount_due > 0",
		season_name,
		year );

	registration_list =
		registration_system_list(
			registration_sys_string( where ),
			0 /* not fetch_enrollment_list */,
			0 /* not fetch_offering */,
			0 /* not fetch_course */,
			0 /* not fetch_program */,
			0 /* not fetch_tuition_payment_list */,
			0 /* not fetch_tuition_refund_list */ );

	if ( !list_rewind( registration_list ) )
	{
		printf("<h3>No registrations with invoice amount due.</h3>\n" );
		return;
	}

	do {
		registration = list_get( registration_list );

		generate_invoice(
			application_name,
			document_root_directory,
			registration->
				student_entity->
				full_name,
			registration->
				student_entity->
				street_address,
			season_name,
			year,
			output_option,
			self );

	} while ( list_next( registration_list ) );
}

void output_invoice_window(
			char *ftp_output_filename,
			int process_id,
			char *full_name )
{
	char window_label[ 128 ];

	sprintf( window_label, "latex_invoice_window_%d", process_id );

/*
	printf(
"<body bgcolor=\"%s\" onload=\"window.open('%s','%s','menubar=yes,resizeable=yes,scrollbars=yes,status=no,toolbar=no,location=no', 'false');\">\n",
			application_get_background_color(
				application_name ),
			ftp_output_filename,
			window_label );
*/

	printf( "<br><a href='%s' target=%s>Invoice for %s.</a>\n",
		ftp_output_filename,
		window_label,
		full_name );
	fflush( stdout );
}

LATEX_INVOICE_CUSTOMER *generate_invoice_customer(
			REGISTRATION *registration )
{
	LATEX_INVOICE_CUSTOMER *invoice_customer;
	char registration_date[ 16 ];
	char invoice_key[ 128 ];
	ENTITY *student_entity;

	if ( !registration )
	{
		return (LATEX_INVOICE_CUSTOMER *)0;
	}

	sprintf(invoice_key,
		"%s %s %s",
		registration->student_entity->full_name,
		registration->student_entity->street_address,
		column( registration_date,
			0,
			registration->registration_date_time ) );

	if ( ! ( student_entity =
			entity_fetch( 
				registration->
					student_entity->
					full_name,
				registration->
					student_entity->
					street_address ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: entity_fetch(%s/%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			registration->student_entity->full_name,
			registration->student_entity->street_address );
		exit( 1 );
	}

	invoice_customer =
		latex_invoice_customer_new(
			student_entity->full_name,
			student_entity->street_address,
			student_entity->city,
			student_entity->state_code,
			student_entity->zip_code,
			student_entity->phone_number,
			student_entity->email_address );

	return invoice_customer;
}

boolean generate_invoice_line_item_list(
			LIST *invoice_line_item_list,
			LIST *enrollment_list )
{
	ENROLLMENT *enrollment;

	if ( !list_rewind( enrollment_list ) ) return 0;

	do {
		enrollment = list_get( enrollment_list );

		invoice_line_item_list =
			latex_invoice_line_item_set(
				invoice_line_item_list,
				(char *)0 /* item_key */,
				course_name_escape(
					enrollment->
						offering->
						course->
						course_name )
					/* item_name */,
				1.0	/* quantity */,
				enrollment->offering->course_price
					/* retail_price */,
				0.0	/* discount_amount */,
				enrollment->offering->course_price
					/* extended_price */ );

	} while ( list_next( enrollment_list ) );

	return 1;
}

void generate_invoice(		char *application_name,
				char *document_root_directory,
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
				char *output_option,
				ENTITY_SELF *self )
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
					document_root_directory,
					full_name,
					street_address,
					season_name,
					year,
					process_id,
					self ) ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: generate_invoice_PDF() failed.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		output_invoice_window(
			ftp_output_filename,
			process_id,
			full_name );
	}
	else
	if ( strcmp( output_option, "email_display" ) == 0 )
	{
		generate_invoice_email_display(
			application_name,
			document_root_directory,
			full_name,
			street_address,
			season_name,
			year,
			process_id,
			self );
	}
	else
	if ( strcmp( output_option, "email_send" ) == 0 )
	{
		generate_invoice_email_send(
			application_name,
			document_root_directory,
			full_name,
			street_address,
			season_name,
			year,
			process_id,
			self );
	}
}

/* Returns output_filename */
/* ----------------------- */
char *generate_invoice_PDF(	char **ftp_output_filename,
				char *application_name,
				char *document_root_directory,
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
				int process_id,
				ENTITY_SELF *self )
{
	FILE *output_stream;
	char *output_filename;
	char *output_directory;
	APPLICATION_CONSTANTS *application_constants;
	char sys_string[ 1024 ];
	APPASERVER_LINK_FILE *appaserver_link_file;
	char mnemonic[ 256 ];
	char filename_stem[ 256 ];

	application_constants = application_constants_new();

	application_constants->dictionary =
		application_constants_dictionary(
			application_name );

	sprintf( filename_stem,
		 "invoice_%s",
		 string_format_mnemonic(
			mnemonic,
			full_name ) );

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			filename_stem,
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
			full_name,
			street_address,
			season_name,
			year,
	 		application_constants_safe_fetch(
				application_constants->dictionary,
				PREDICTIVE_LOGO_FILENAME_KEY ),
			self ) )
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
			char *document_root_directory,
			char *full_name,
			char *street_address,
			char *season_name,
			int year,
			int process_id,
			ENTITY_SELF *self )
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
			document_root_directory,
			full_name,
			street_address,
			season_name,
			year,
			process_id,
			self );

	if ( ! ( entity = entity_fetch(
				full_name,
				street_address ) ) )
	{
		printf( "<h3>Error: cannot fetch %s/%s</h3>\n",
			full_name,
			street_address );
		return;
	}

	if ( !*entity->email_address )
	{
		printf( "<h3>Error: no email address for %s/%s</h3>\n",
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
			self->entity->email_address
				/* from_address */,
			entity->email_address
				/* to_address */,
			SUBJECT,
			"Here is your invoice."
				/* message */,
			self->entity->email_address
				/* reply_to */,
			self->entity->full_name );

	printf( "<p>%s\n", sendmail_command );
	printf( "<p>%s\n", sendmail_string );
}

void generate_invoice_email_send(
				char *application_name,
				char *document_root_directory,
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
				pid_t process_id,
				ENTITY_SELF *self )
{
	char *output_filename;
	char *ftp_output_filename;
	ENTITY *entity;

	output_filename =
		generate_invoice_PDF(
			&ftp_output_filename,
			application_name,
			document_root_directory,
			full_name,
			street_address,
			season_name,
			year,
			process_id,
			self );

	if ( ! ( entity = entity_fetch(
				full_name,
				street_address ) ) )
	{
		printf( "<h3>Error: cannot fetch %s/%s</h3>\n",
			full_name,
			street_address );
		return;
	}

	if ( !*entity->email_address )
	{
		printf( "<h3>Error: no email address for %s/%s</h3>\n",
			full_name,
			street_address );
		return;
	}

	email_sendmail(
		self->entity->email_address
			/* from_address */,
		entity->email_address
			/* to_address */,
		SUBJECT,
		MESSAGE,
		self->entity->email_address
			/* reply_to */,
		entity->full_name,
		output_filename
			/* attachment_filename */ );

	printf( "<h3>Message send.</h3>\n" );
}

boolean build_latex_invoice(	FILE *output_stream,
				char *full_name,
				char *street_address,
				char *season_name,
				int year,
				char *predictive_logo_filename,
				ENTITY_SELF *self )
{
	LATEX_INVOICE *latex_invoice;
	char *todays_date;
	REGISTRATION *registration;
	char title[ 128 ];

	sprintf(	title,
			"%s Invoice",
			self->entity->full_name );

	if ( ! ( registration =
			registration_fetch(
				full_name,
				street_address,
				season_name,
				year,
				1 /* fetch_enrollment_list */,
				1 /* fetch_offering */,
				1 /* fetch_course */,
				0 /* not fetch_program */,
				0 /* not fetch_tuition_payment_list */,
				0 /* not fetch_tuition_refund_list */ ) ) )
	{
		return 0;
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

	if ( ! ( latex_invoice->invoice_customer =
			generate_invoice_customer(
				registration ) ) )
	{
		return 0;
	}

	latex_invoice_output_header( output_stream );

	if ( !generate_invoice_line_item_list(
			latex_invoice->invoice_line_item_list,
			registration->registration_enrollment_list ) )
	{
		printf( "<H3>Error: Registration has no enrollments.</h3>\n" );
		document_close();
		exit( 0 );
	}

	latex_invoice->extended_price_total = registration->tuition;
	latex_invoice->total_payment = registration->tuition_payment_total;

	latex_invoice->exists_discount_amount =
		latex_invoice_exists_discount_amount(
			latex_invoice->invoice_line_item_list );

	latex_invoice_education_invoice_header(
		output_stream,
		latex_invoice->invoice_date,
		latex_invoice->invoice_self,
		latex_invoice->invoice_customer,
		title,
		predictive_logo_filename );

	latex_invoice_education_line_item_list(
		output_stream,
		latex_invoice->
			invoice_line_item_list );

	latex_invoice_education_invoice_footer(
		output_stream,
		latex_invoice->extended_price_total,
		latex_invoice->total_payment );

	latex_invoice_output_footer(
		output_stream,
		0 /* not with_customer_signature */ );

	return 1;
}

