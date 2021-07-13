/* ---------------------------------------------------------------------*/
/* $APPASERVER_HOME/src_predictive/post_change_employee_work_day.c	*/
/* ---------------------------------------------------------------------*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "employee.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_employee_work_day_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *begin_work_date_time,
			char *preupdate_begin_work_date_time,
			char *preupdate_end_work_date_time );

void post_change_employee_work_day_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *begin_work_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *begin_work_date_time;
	char *state;
	char *preupdate_begin_work_date_time;
	char *preupdate_end_work_date_time;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address begin_work_date_time state preupdate_begin_work_date_time preupdate_end_work_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	begin_work_date_time = argv[ 4 ];
	state = argv[ 5 ];
	preupdate_begin_work_date_time = argv[ 6 ];
	preupdate_end_work_date_time = argv[ 7 ];

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_employee_work_day_insert(
			application_name,
			full_name,
			street_address,
			begin_work_date_time );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_employee_work_day_update(
			application_name,
			full_name,
			street_address,
			begin_work_date_time,
			preupdate_begin_work_date_time,
			preupdate_end_work_date_time );
	}

	return 0;

} /* main() */

void post_change_employee_work_day_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *begin_work_date_time,
			char *preupdate_begin_work_date_time,
			char *preupdate_end_work_date_time )
{
	EMPLOYEE *employee;
	EMPLOYEE_WORK_DAY *employee_work_day;
	char begin_work_date[ 32 ];
	enum preupdate_change_state begin_work_date_time_change_state;
	enum preupdate_change_state end_work_date_time_change_state;

	column( begin_work_date, 0, begin_work_date_time );

	if ( ! ( employee =
			employee_with_load_new(
				application_name,
				full_name,
				street_address,
				begin_work_date,
				(char *)0 /* end_work_date */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load %s/%s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address );
		exit( 1 );
	}

	if ( ! ( employee_work_day =
			employee_work_day_seek(
				employee->employee_work_day_list,
				begin_work_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek %s/%s/%s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 begin_work_date_time );
		exit( 1 );
	}

	begin_work_date_time_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_begin_work_date_time,
			employee_work_day->begin_work_date_time
				/* postupdate_data */,
			"preupdate_begin_work_date_time" );

	end_work_date_time_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_end_work_date_time,
			employee_work_day->end_work_date_time
				/* postupdate_data */,
			"preupdate_end_work_date_time" );

	if ( begin_work_date_time_change_state != no_change
	||   end_work_date_time_change_state != no_change )
	{
		if ( !timlib_double_virtually_same(
			employee_work_day->employee_work_hours,
			employee_work_day->database_employee_work_hours ) )
		{
			employee_work_day_update(
				application_name,
				employee->full_name,
				employee->street_address,
				begin_work_date_time,
				employee_work_day->employee_work_hours,
				employee_work_day->
					database_employee_work_hours );

			employee_work_day_update_timestamp(
				application_name,
				full_name,
				street_address,
				begin_work_date_time,
				"update_timestamp" );
		}
	}

} /* post_change_employee_work_day_update() */

void post_change_employee_work_day_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *begin_work_date_time )
{
	employee_work_day_update_timestamp(
		application_name,
		full_name,
		street_address,
		begin_work_date_time,
		"insert_timestamp" );

} /* post_change_employee_work_day_insert() */

