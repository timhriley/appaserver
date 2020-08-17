/* $APPASERVER_HOME/src_hydrology/measurement_insert.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "measurement.h"
#include "station_datatype.h"
#include "appaserver_library.h"
#include "timlib.h"
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "datatype.h"
#include "name_arg.h"

/* Prototypes */
/* ---------- */
void setup_arg(		NAME_ARG *arg, int argc, char **argv );

void fetch_parameters(	char **bypass_reject_yn,
			char **delimiter,
			char **replace_yn,
			char **insert_null_value_yn,
			char **insert_statements_yn,
			char **execute_yn,
			NAME_ARG *arg );

int main( int argc, char **argv )
{
	char *application_name;
	char delimited_record[ 1024 ];
	char *delimiter;
	char *bypass_reject_yn;
	char *replace_yn;
	char *insert_null_value_yn;
	char *insert_statements_yn;
	char *execute_yn;
	boolean bypass_reject;
	boolean replace;
	boolean insert_null_value;
	boolean insert_statements;
	boolean execute;
	MEASUREMENT_STRUCTURE *m;
	int row_number = 0;
	NAME_ARG *arg;
	LIST *station_datatype_list;
	STATION_DATATYPE *station_datatype;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	arg = name_arg_new( argv[ 0 ] );

	setup_arg( arg, argc, argv );

	fetch_parameters(
		&bypass_reject_yn,
		&delimiter,
		&replace_yn,
		&insert_null_value_yn,
		&insert_statements_yn,
		&execute_yn,
		arg );

	bypass_reject = ( *bypass_reject_yn == 'y' );
	replace = ( *replace_yn == 'y' );
	insert_null_value = ( *insert_null_value_yn == 'y' );
	insert_statements = ( *insert_statements_yn == 'y' );
	execute = ( *execute_yn == 'y' );

	m = measurement_structure_new( application_name );

	if ( insert_statements )
	{
		m->insert_statement_pipe =
			measurement_open_insert_statement_pipe(
				replace );
		execute = 0;
	}

	if ( !execute )
	{
		if ( !insert_statements )
		{
			m->html_table_pipe =
				measurement_open_html_table_pipe();
		}
	}

	if ( execute )
	{
		m->insert_pipe = 
			measurement_open_insert_pipe( replace );
	}

	station_datatype_list = list_new();

	while( timlib_get_line( delimited_record, stdin, 1024 ) )
	{
		row_number++;

		if ( !measurement_set_delimited_record(
			m, 
			delimited_record,
			*delimiter ) )
		{
			fprintf( stderr,
				 "Invalid data in row %d: %s\n",
				 row_number,
				 delimited_record );
			continue;
		}

		if ( ! ( station_datatype =
				station_datatype_get_or_set(
					station_datatype_list,
					application_name,
					m->measurement->station_name,
					m->measurement->datatype ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: station_datatype_get_or_set() failed.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		if ( !bypass_reject )
		{
			if ( !station_datatype->data_collection_frequency_list )
			{
				station_datatype->
					data_collection_frequency_list =
					   station_datatype_frequency_list(
						application_name,
						station_datatype->
							station_name,
						station_datatype->
							datatype->
							datatype_name );
			}

/*
fprintf( stderr, "%s/%s()/%d: with delimited_record = %s, got data_collection_frequency_list = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
delimited_record,
station_datatype_frequency_display(
	station_datatype->data_collection_frequency_list ) );
fflush( stderr );
*/

			if ( station_datatype_frequency_reject(
				station_datatype->
					data_collection_frequency_list,
				m->measurement->measurement_date,
				m->measurement->measurement_time ) )
			{
				fprintf( stderr,
			"Violates DATA_COLLECTION_FREQUENCY in row %d: %s\n",
				 	row_number,
				 	delimited_record );
				continue;
			}
		}

		if ( execute )
		{
			measurement_insert(
				m,
				insert_null_value );
		}
		else
		if ( m->html_table_pipe )
		{
			measurement_pipe_output(
				m->html_table_pipe,
				m );
		}
		else
		if ( m->insert_statement_pipe )
		{
			measurement_pipe_output(
				m->insert_statement_pipe,
				m );
		}

		station_datatype->measurement_count++;
	}

	if ( m->insert_pipe ) pclose( m->insert_pipe );
	if ( m->html_table_pipe ) pclose( m->html_table_pipe );
	if ( m->insert_statement_pipe ) pclose( m->insert_statement_pipe );

	fflush( stdout );
	station_datatype_html_display( station_datatype_list );
	fflush( stdout );

	return 0;
}

void fetch_parameters(	char **bypass_reject_yn,
			char **delimiter,
			char **replace_yn,
			char **insert_null_value_yn,
			char **insert_statements_yn,
			char **execute_yn,
			NAME_ARG *arg )
{
	*bypass_reject_yn = fetch_arg( arg, "bypass_reject" );
	*replace_yn = fetch_arg( arg, "replace" );
	*insert_null_value_yn = fetch_arg( arg, "insert_null_value" );
	*insert_statements_yn = fetch_arg( arg, "insert_statements" );
	*delimiter = fetch_arg( arg, "delimiter" );
	*execute_yn = fetch_arg( arg, "execute" );
}

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "replace" );
        add_valid_value( arg, ticket, "y" );
        add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ticket = add_valid_option( arg, "insert_null_value" );
        add_valid_value( arg, ticket, "y" );
        add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ticket = add_valid_option( arg, "bypass_reject" );
        add_valid_value( arg, ticket, "y" );
        add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ticket = add_valid_option( arg, "insert_statements" );
        add_valid_value( arg, ticket, "y" );
        add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ticket = add_valid_option( arg, "delimiter" );
        set_default_value( arg, ticket, "^" );

        ticket = add_valid_option( arg, "execute" );
        add_valid_value( arg, ticket, "y" );
        add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ins_all( arg, argc, argv );
}

