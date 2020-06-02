/* $APPASERVER_HOME/src_hydrology/interpolation_spike_calculate.c	*/
/* -------------------------------------------------------------- 	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "basename.h"
#include "list.h"
#include "document.h"
#include "timlib.h"
#include "name_arg.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application.h"
#include "measurement_spike.h"

/* Constants */
/* --------- */
#define VALUE_DELIMITER		','

/* Prototypes */
/* ---------- */
void interpolation_spike_calculate_input_pipe(
				char *application_name,
				char *station_name,
				char *datatype_name,
				char *begin_date,
				char *end_date,
				double threshold,
				boolean trim_negative_drop,
				FILE *input_pipe );

NAME_ARG *setup_named_command_line_arguments( int argc, char **argv );
void output_spike_records( LIST *list );
LIST *begin_list_buffer( char *old_buffer, char *input_buffer );

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *datatype;
	char *begin_date;
	char *begin_time;
	char *end_date;
	char *end_time;
	double minimum_spike;
	char *trim_negative_drop_yn;
	boolean trim_negative_drop;
	NAME_ARG *name_arg;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	output_starting_argv_stderr( argc, argv );

	name_arg = setup_named_command_line_arguments( argc, argv );

	begin_date = fetch_arg( name_arg, "begin_date" );
	begin_time = fetch_arg( name_arg, "begin_time" );
	end_date = fetch_arg( name_arg, "end_date" );
	end_time = fetch_arg( name_arg, "end_time" );
	station = fetch_arg( name_arg, "station" );
	datatype = fetch_arg( name_arg, "datatype" );
	minimum_spike = atof( fetch_arg( name_arg, "minimum_spike" ) );
	trim_negative_drop_yn = fetch_arg( name_arg, "trim_negative_drop_yn" );

	trim_negative_drop = (*trim_negative_drop_yn == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	sprintf( sys_string,
		 "extract_measurements.sh %s '%s' '%s' %s %s %s %s",
		 application_name,
		 station,
		 datatype,
		 begin_date,
		 begin_time,
		 end_date,
		 end_time );

	input_pipe = popen( sys_string, "r" );

	interpolation_spike_calculate_input_pipe(
		application_name,
		station,
		datatype,
		begin_date,
		end_date,
		minimum_spike /* threshold */,
		trim_negative_drop,
		input_pipe );

	pclose( input_pipe );

	return 0;

} /* main() */

NAME_ARG *setup_named_command_line_arguments( int argc, char **argv )
{
	int ticket;
        NAME_ARG *arg = init_arg( argv[ 0 ] );

        ticket = add_valid_option( arg, "application" );
        set_default_value( arg, ticket, "");

        ticket = add_valid_option( arg, "login_name" );
        set_default_value( arg, ticket, "");

        ticket = add_valid_option( arg, "trim_negative_drop_yn" );
        set_default_value( arg, ticket, "n");

        ticket = add_valid_option( arg, "minimum_spike" );
        ticket = add_valid_option( arg, "begin_date" );
        ticket = add_valid_option( arg, "begin_time" );
        ticket = add_valid_option( arg, "end_date" );
        ticket = add_valid_option( arg, "end_time" );
        ticket = add_valid_option( arg, "datatype" );
        ticket = add_valid_option( arg, "station" );

        ins_all( arg, argc, argv );

	return arg;
} /* setup_named_command_line_arguments() */

void interpolation_spike_calculate_input_pipe(
			char *application_name,
			char *station_name,
			char *datatype_name,
			char *begin_date,
			char *end_date,
			double threshold,
			boolean trim_negative_drop,
			/* ---------------------------------------------- */
			/* Sample input: "BA,rain,1999-10-27,2350,25.600" */
			/* ---------------------------------------------- */
			FILE *input_pipe )
{
	MEASUREMENT_SPIKE *measurement_spike;

	if ( ! ( measurement_spike =	
			measurement_spike_new(
				application_name,
				station_name,
				datatype_name,
				begin_date,
				end_date,
				threshold,
				trim_negative_drop ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: measurement_spike_new(%s/%s) returned null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 station_name,
			 datatype_name );
		return;
	}

	measurement_spike->input.station_datatype->measurement_list =
		/* ---------------------------------------------- */
		/* Sample input: "BA,rain,1999-10-27,2350,25.600" */
		/* ---------------------------------------------- */
		measurement_fetch_list(
			input_pipe,
			VALUE_DELIMITER );

	/* -------------------------------------- */
	/* Sets last_good_measurement_value	  */
	/* Sets next_first_good_measurement_value */
	/* Doesn't set measurement_update	  */
	/* -------------------------------------- */
	measurement_spike->spike_block_list =
		measurement_spike_get_block_list(
			measurement_spike->
				input.
				station_datatype->
				measurement_list,
			measurement_spike->
				input.
				threshold,
			measurement_spike->
				input.
				trim_negative_drop );

/*
fprintf( stderr, "%s/%s()/%d: blocks:\n%s\n",
__FILE__,
__FUNCTION__,
__LINE__,
measurement_spike_block_display( measurement_spike->spike_block_list ) );
*/

	measurement_spike_set_block_measurement_update(
		measurement_spike->spike_block_list );

	measurement_change_text_output(
		measurement_spike->
			input.
			station_datatype->
			measurement_list,
		VALUE_DELIMITER );

} /* interpolation_spike_calculate_input_pipe() */

