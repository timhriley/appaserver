/* -------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_activity_work.c 	  */
/* -------------------------------------------------------------- */
/* 								  */
/* Freely available software: see Appaserver.org		  */
/* -------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void activity_work_close_all(
				char *application_name,
				char *login_name,
				char *begin_work_date_time );

char *activity_work_fetch_end_work_date_time(
				char *application_name,
				char *login_name,
				char *begin_work_date_time );

void activity_work_update(
				char *application_name,
				char *login_name,
				char *begin_work_date_time,
				double work_hours );

void post_change_activity_work_insert(
				char *application_name,
				char *login_name,
				char *begin_work_date_time );

void post_change_activity_work_update(
				char *application_name,
				char *login_name,
				char *begin_work_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *begin_work_date_time;
	char *state;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
"Usage: %s ignored login_name begin_work_date_time state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 2 ];
	begin_work_date_time = argv[ 3 ];
	state = argv[ 4 ];

	/* If change login_name only. */
	/* -------------------------- */
	if ( strcmp( begin_work_date_time, "begin_work_date_time" ) == 0 )
		exit( 0 );

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );
	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_activity_work_insert(
			application_name,
			login_name,
			begin_work_date_time );
	}
	else
	{
		post_change_activity_work_update(
			application_name,
			login_name,
			begin_work_date_time );
	}

	return 0;

} /* main() */

void post_change_activity_work_update(
			char *application_name,
			char *login_name,
			char *begin_work_date_time )
{
	char *end_work_date_time;
	double work_hours = 0.0;

	end_work_date_time =
		activity_work_fetch_end_work_date_time(
			application_name,
			login_name,
			begin_work_date_time );

	if ( end_work_date_time && *end_work_date_time )
	{
		work_hours = 
			customer_get_work_hours(
				end_work_date_time,
				begin_work_date_time );
	}

	activity_work_update(
		application_name,
		login_name,
		begin_work_date_time,
		work_hours );

} /* post_change_activity_work_update() */

void post_change_activity_work_insert(
			char *application_name,
			char *login_name,
			char *begin_work_date_time )
{
	char *end_work_date_time;
	double work_hours = 0.0;

	activity_work_close_all(
			application_name,
			login_name,
			begin_work_date_time );

	end_work_date_time =
		activity_work_fetch_end_work_date_time(
			application_name,
			login_name,
			begin_work_date_time );

	if ( end_work_date_time && *end_work_date_time )
	{
		work_hours = 
			customer_get_work_hours(
				end_work_date_time,
				begin_work_date_time );
	}

	if ( work_hours )
	{
		activity_work_update(
			application_name,
			login_name,
			begin_work_date_time,
			work_hours );
	}

} /* post_change_activity_work_insert() */

char *activity_work_fetch_end_work_date_time(
			char *application_name,
			char *login_name,
			char *begin_work_date_time )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	char *results;

	sprintf( where,
		 "login_name = '%s' and		"
		 "begin_work_date_time = '%s'	",
		 login_name,
		 begin_work_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=end_work_date_time	"
		 "			folder=activity_work		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( !results )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot fetch activity_work = (%s/%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 login_name,
			 begin_work_date_time );
		exit( 1 );
	}

	return results;

} /* activity_work_fetch_end_work_date_time() */

void activity_work_update(
			char *application_name,
			char *login_name,
			char *begin_work_date_time,
			double work_hours )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *table_name;
	char *key;

	table_name = get_table_name( application_name, "activity_work" );
	key = "login_name,begin_work_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^work_hours^%.4lf\n",
		login_name,
		begin_work_date_time,
		work_hours );

	pclose( output_pipe );

} /* activity_work_update() */

void activity_work_close_all(
			char *application_name,
			char *login_name,
			char *begin_work_date_time )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "activity_work_close_all.sh %s %s \"%s\"",
		 application_name,
		 login_name,
		 begin_work_date_time );

	system( sys_string );

} /* activity_work_close_all() */

