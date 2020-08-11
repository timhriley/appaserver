/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/shortcut_service_work.c		*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "environ.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void service_work_close_all(	char *application_name,
				char *login_name );

void service_work_close_fixed(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *begin_work_date_time );

void service_work_open_fixed(
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *login_name );

void service_work_close_hourly(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *description,
				char *begin_work_date_time );

void service_work_open_hourly(
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *description,
				char *login_name );

void display_customer_sale(	char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *process_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *operation;
	char *hourly_fixed;
	char *service_name;
	char *description;
	char *begin_work_date_time;
	DOCUMENT *document;
	char title[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 12 )
	{
		fprintf( stderr, 
"Usage: %s ignored process login_name full_name street_address sale_date_time operation hourly_fixed service_name description begin_work_date_time\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	full_name = argv[ 4 ];
	street_address = argv[ 5 ];
	sale_date_time = argv[ 6 ];
	operation = argv[ 7 ];
	hourly_fixed = argv[ 8 ];
	service_name = argv[ 9 ];
	description = argv[ 10 ];
	begin_work_date_time = argv[ 11 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	format_initial_capital( title, process_name );
	document = document_new( title, application_name );
	document->output_content_type = 1;

	document_output_head_stream(
			stdout,
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */,
			1 /* with close_head */ );

	document_output_body(	document->application_name,
				document->onload_control_string );

	printf( "<h1>%s</h1>\n", title );

	if ( !*full_name || strcmp( full_name, "full_name" ) == 0 )
	{
		printf(
		"<h3>Error: please select a customer sale.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( operation, "open" ) == 0
	&&   strcmp( hourly_fixed, "hourly" ) == 0 )
	{
		service_work_close_all( application_name, login_name );

		service_work_open_hourly(
			full_name,
			street_address,
			sale_date_time,
			service_name,
			description,
			login_name );

		display_customer_sale(
				application_name,
				full_name,
				street_address,
				sale_date_time );

	}
	else
	if ( strcmp( operation, "close" ) == 0
	&&   strcmp( hourly_fixed, "hourly" ) == 0 )
	{
		service_work_close_hourly(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			service_name,
			description,
			begin_work_date_time );

		display_customer_sale(
				application_name,
				full_name,
				street_address,
				sale_date_time );

	}
	else
	if ( strcmp( operation, "open" ) == 0
	&&   strcmp( hourly_fixed, "fixed" ) == 0 )
	{
		service_work_close_all( application_name, login_name );

		service_work_open_fixed(
			full_name,
			street_address,
			sale_date_time,
			service_name,
			login_name );

		display_customer_sale(
				application_name,
				full_name,
				street_address,
				sale_date_time );

	}
	else
	if ( strcmp( operation, "close" ) == 0
	&&   strcmp( hourly_fixed, "fixed" ) == 0 )
	{
		service_work_close_fixed(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			service_name,
			begin_work_date_time );

		display_customer_sale(
				application_name,
				full_name,
				street_address,
				sale_date_time );

	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid combination.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return 0;

} /* main() */

void display_customer_sale(	char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *select;
	char *where;

	select =
	"full_name,street_address,completed_date_time,invoice_amount";

	where = ledger_get_transaction_where(
				full_name,
				street_address,
				sale_date_time,
				(char *)0 /* folder_name */,
				"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=customer_sale		 "
		 "			where=\"%s\"			|"
		 "html_table.e '' '%s' '%c' 'left,left,left,right'	 ",
		 application_name,
		 select,
		 where,
		 select,
		 FOLDER_DATA_DELIMITER );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* display_customer_sale() */

void service_work_open_hourly(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *description,
			char *login_name )
{
	char sys_string[ 1024 ];
	char *table;
	char *field;
	FILE *output_pipe;
	char *now;

	table = "hourly_service_work";

	field =
"full_name,street_address,sale_date_time,service_name,description,begin_work_date_time,login_name";

	now = date_get_now16( date_get_utc_offset() );

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'	|"
		 "sql.e 2>&1					|"
		 "html_paragraph_wrapper.e			 ",
		 table,
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s^%s^%s^%s^%s^%s^%s\n",
		 full_name,
		 street_address,
		 sale_date_time,
		 service_name,
		 description,
		 now,
		 login_name );

	pclose( output_pipe );

} /* service_work_open_hourly() */

void service_work_close_hourly(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *description,
			char *begin_work_date_time )
{
	char sys_string[ 1024 ];
	char *table;
	char *key;
	FILE *output_pipe;
	char *now;

	table = "hourly_service_work";

	key =
"full_name,street_address,sale_date_time,service_name,description,begin_work_date_time";

	now = date_get_now16( date_get_utc_offset() );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s^%s^%s^%s^%s^%s^end_work_date_time^%s\n",
		 full_name,
		 street_address,
		 sale_date_time,
		 service_name,
		 description,
		 begin_work_date_time,
		 now );

	pclose( output_pipe );

	sprintf( sys_string,
"post_change_hourly_service_work %s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" update preupdate_end_work_date_time",
		 application_name,
		 full_name,
		 street_address,
		 sale_date_time,
		 service_name,
		 description,
		 begin_work_date_time );

	system( sys_string );

} /* service_work_close_hourly() */

void service_work_open_fixed(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *login_name )
{
	char sys_string[ 1024 ];
	char *table;
	char *field;
	FILE *output_pipe;
	char *now;

	table = "fixed_service_work";

	field =
"full_name,street_address,sale_date_time,service_name,begin_work_date_time,login_name";

	now = date_get_now16( date_get_utc_offset() );

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'	|"
		 "sql.e 2>&1					|"
		 "html_paragraph_wrapper.e			 ",
		 table,
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s^%s^%s^%s^%s^%s\n",
		 full_name,
		 street_address,
		 sale_date_time,
		 service_name,
		 now,
		 login_name );

	pclose( output_pipe );

} /* service_work_open_fixed() */

void service_work_close_fixed(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *begin_work_date_time )
{
	char sys_string[ 1024 ];
	char *table;
	char *key;
	FILE *output_pipe;
	char *now;

	table = "fixed_service_work";

	key =
"full_name,street_address,sale_date_time,service_name,begin_work_date_time";

	now = date_get_now16( date_get_utc_offset() );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s^%s^%s^%s^%s^end_work_date_time^%s\n",
		 full_name,
		 street_address,
		 sale_date_time,
		 service_name,
		 begin_work_date_time,
		 now );

	pclose( output_pipe );

	sprintf( sys_string,
"post_change_fixed_service_work %s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" update preupdate_end_work_date_time",
		 application_name,
		 full_name,
		 street_address,
		 sale_date_time,
		 service_name,
		 begin_work_date_time );

	system( sys_string );

} /* service_work_close_fixed() */

void service_work_close_all(	char *application_name,
				char *login_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "service_work_close_all.sh %s %s",
		 application_name,
		 login_name );

	system( sys_string );

	sprintf( sys_string,
		 "activity_work_close_all.sh %s %s ''",
		 application_name,
		 login_name );

	system( sys_string );

} /* service_work_close_all() */

