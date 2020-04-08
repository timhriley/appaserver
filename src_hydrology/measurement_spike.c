/* $APPASERVER_HOME/src_hydrology/measurement_spike.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include "list.h"
#include "measurement_spike.h"

MEASUREMENT_SPIKE_BLOCK *measurement_spike_block_new( void )
{
	MEASUREMENT_SPIKE_BLOCK *m;

	if ( ! ( m = (MEASUREMENT_SPIKE_BLOCK *)
			calloc( 1, sizeof( MEASUREMENT_SPIKE_BLOCK ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return m;

} /* measurement_spike_block_new() */

MEASUREMENT_SPIKE *measurement_spike_new(
				char *application_name,
				char *station_name,
				char *datatype_name,
				char *begin_date,
				char *end_date,
				double threshold,
				boolean trim_negative_drop )
{
	MEASUREMENT_SPIKE *m;

	if ( ! ( m = (MEASUREMENT_SPIKE *)
			calloc( 1, sizeof( MEASUREMENT_SPIKE ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	m->input.begin_date = begin_date;
	m->input.end_date = end_date;
	m->input.threshold = threshold;
	m->input.trim_negative_drop = trim_negative_drop;

	m->input.station_datatype =
		station_datatype_fetch_new(
			application_name,
			station_name,
			datatype_name );

	if ( !m->input.station_datatype ) return (MEASUREMENT_SPIKE *)0;

	m->input.station_datatype->measurement_list =
		station_datatype_fetch_measurement_list(
			application_name,
			m->input.station_datatype->station_name,
			m->input.station_datatype->datatype->datatype_name,
			m->input.begin_date,
			m->input.end_date );

	return m;

} /* measurement_spike_new() */

/* -------------------------------------- */
/* Sets last_good_measurement_value	  */
/* Sets next_first_good_measurement_value */
/* Doesn't set measurement_update	  */
/* -------------------------------------- */
LIST *measurement_spike_get_block_list(
				LIST *measurement_list,
				double threshold,
				boolean trim_negative_drop )
{
	LIST *block_list = {0};
	MEASUREMENT_SPIKE_BLOCK *measurement_spike_block;
	MEASUREMENT *measurement;
	MEASUREMENT *prior_measurement = {0};

	if ( ! list_rewind( measurement_list ) ) return (LIST *)0;

	do {
		measurement = list_get( measurement_list );

		if ( !prior_measurement )
		{
			prior_measurement = measurement;
			continue;
		}

		if ( measurement_spike_exceed_threshold(
				measurement->measurement_value,
				prior_measurement->measurement_value,
				threshold,
				trim_negative_drop ) )
		{
			measurement_spike_block = measurement_spike_block_new();

			measurement_spike_block->last_good_measurement_value =
				prior_measurement->measurement_value;

			measurement_spike_block->spike_measurement_list =
				measurement_spike_block_measurement_list(
					/* ------------------------- */
					/* Starts with the first bad */
					/* ------------------------- */
					measurement_list /* in/out */,
					&measurement_spike_block->
					    next_first_good_measurement_value,
					measurement_spike_block->
						last_good_measurement_value,
					threshold,
					trim_negative_drop );

			if ( !block_list ) block_list = list_new();

			list_append_pointer(
				block_list,
				measurement_spike_block );
		}

		prior_measurement = measurement;

	} while ( list_next( measurement_list ) );

	return block_list;

} /* measurement_spike_get_block_list() */

char *measurement_spike_block_display(
				LIST *spike_block_list )
{
	MEASUREMENT_SPIKE_BLOCK *b;
	MEASUREMENT *first_measurement;
	MEASUREMENT *last_measurement;
	char buffer[ 65536 ];
	char *ptr;

	if ( !list_rewind( spike_block_list ) ) return strdup( "" );

	ptr = buffer;
	*ptr = '\0';

	do {
		b = list_get( spike_block_list );

		if ( !list_length( b->spike_measurement_list ) )
		{
			ptr += sprintf( ptr,
			"Warning: empty measurement_list for block.\n" );

			continue;
		}

		first_measurement =
			list_get_first_pointer(
				b->spike_measurement_list );

		last_measurement =
			list_get_last_pointer(
				b->spike_measurement_list );

		ptr += sprintf( ptr,
				"last_good_measurement_value: %.3lf; "
				"next_first_good_measurement_value: %.3lf; "
				"first_spike: %s:%s; "
				"last_spike: %s:%s\n",
				b->last_good_measurement_value,
				b->next_first_good_measurement_value,
				first_measurement->measurement_date,
				first_measurement->measurement_time,
				last_measurement->measurement_date,
				last_measurement->measurement_time );

	} while ( list_next( spike_block_list ) );

	return strdup( buffer );

} /* measurement_spike_block_display() */

void measurement_spike_block_update_output(
				char *application_name,
				LIST *spike_block_list )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *key;
	MEASUREMENT_SPIKE_BLOCK *b;

	if ( !list_rewind( spike_block_list ) ) return;

	table_name = get_table_name( application_name, "measurement" );

	key = "station,datatype,measurement_date,measurement_time";

	fprintf( output_pipe,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	do {
		b = list_get( spike_block_list );

		measurement_spike_update_output(
			output_pipe,
			b->spike_measurement_list );

	} while ( list_next( spike_block_list ) );

	pclose( output_pipe );

} /* measurement_spike_block_update_output() */

void measurement_spike_update_output(
				FILE *output_pipe,
				LIST *spike_measurement_list )
{
	MEASUREMENT *measurement;

	if ( !list_rewind( spike_measurement_list ) ) return;

	do {
		measurement = list_get( spike_measurement_list );

		if ( measurement->measurement_update )
		{
			fprintf( output_pipe,
				 "%s^%s^%s^%s^measurement_value^%.3lf\n",
				 measurement->station_name,
				 measurement->datatype,
				 measurement->measurement_date,
				 measurement->measurement_time,
				 measurement->
					measurement_update->
					measurement_update );
		}

	} while ( list_next( spike_measurement_list ) );

} /* measurement_spike_update_output() */

LIST *measurement_spike_block_measurement_list(
				/* ------------------------- */
				/* Starts with the first bad */
				/* ------------------------- */
				LIST *measurement_list /* in/out */,
				double *next_first_good_measurement_value,
				double last_good_measurement_value,
				double threshold,
				boolean trim_negative_drop )
{
	MEASUREMENT *measurement;
	LIST *return_measurement_list = list_new();

	/* ------------------------- */
	/* Note: don't list_rewind() */
	/* ------------------------- */

	do {
		measurement = list_get( measurement_list );

		/* If still bad */
		/* ------------ */
		if ( measurement_spike_exceed_threshold(
				measurement->measurement_value,
				last_good_measurement_value,
				threshold,
				trim_negative_drop ) )
		{
			list_append_pointer(
				return_measurement_list,
				measurement );
		}
		else
		{
			/* Now back to good */
			/* ---------------- */
			*next_first_good_measurement_value =
				measurement->measurement_value;

			list_prior( measurement_list );

			return return_measurement_list;
		}

	} while ( list_next( measurement_list ) );

	/* Ran off the end */
	/* --------------- */
	*next_first_good_measurement_value = -1.0;
	return return_measurement_list;

} /* measurement_spike_block_measurement_list() */

void measurement_spike_block_text_output(
				LIST *spike_block_list,
				char delimiter )
{
	MEASUREMENT_SPIKE_BLOCK *b;

	if ( !list_rewind( spike_block_list ) ) return;

	do {
		b = list_get( spike_block_list );

		measurement_spike_text_output(
			b->spike_measurement_list,
			delimiter );

	} while ( list_next( spike_block_list ) );

} /* measurement_spike_block_text_output() */

void measurement_spike_text_output(
				LIST *spike_measurement_list,
				char delimiter )
{
	MEASUREMENT *measurement;

	if ( !list_rewind( spike_measurement_list ) ) return;

	do {
		measurement = list_get( spike_measurement_list );

		printf( "%s%c%s%c%s%c%s%c%.3lf%c",
			 measurement->station_name,
			 delimiter,
			 measurement->datatype,
			 delimiter,
			 measurement->measurement_date,
			 delimiter,
			 measurement->measurement_time,
			 delimiter,
			 measurement->measurement_value,
			 delimiter );

		if ( measurement->measurement_update )
		{
			printf( "%.3lf\n",
				 measurement->
					measurement_update->
					measurement_update );
		}
		else
		{
			printf( "%.3lf\n",
				 measurement->measurement_value );
		}

	} while ( list_next( spike_measurement_list ) );

} /* measurement_spike_update_output() */

void measurement_spike_set_block_measurement_update(
				LIST *spike_block_list )
{
	MEASUREMENT_SPIKE_BLOCK *b;

	if ( !list_rewind( spike_block_list ) ) return;

	do {
		b = list_get( spike_block_list );

		measurement_spike_set_measurement_update(
			b->spike_measurement_list,
			b->last_good_measurement_value,
			b->next_first_good_measurement_value );

	} while ( list_next( spike_block_list ) );

} /* measurement_spike_set_block_measurement_update() */

void measurement_spike_set_measurement_update(
				LIST *spike_measurement_list,
				double last_good_measurement_value,
				double next_first_good_measurement_value )
{
	MEASUREMENT *measurement;
	double range;
	double delta;
	double new_value;

	if ( !list_rewind( spike_measurement_list ) ) return;

	range =	next_first_good_measurement_value -
		last_good_measurement_value;

	delta = range /
		(double)( list_length( spike_measurement_list ) + 1 );

	new_value = last_good_measurement_value + delta;

	do {
		measurement = list_get( spike_measurement_list );

		measurement->measurement_update = measurement_update_new();
		measurement->measurement_update->measurement_update = new_value;

		new_value += delta;

	} while ( list_next( spike_measurement_list ) );

	return;

} /* measurement_spike_set_measurement_update() */

boolean measurement_spike_exceed_threshold(
				double measurement_value,
				double prior_measurement_value,
				double threshold,
				boolean trim_negative_drop )
{

	if ( !trim_negative_drop )
	{
		return (boolean)
				( measurement_value >
					prior_measurement_value + threshold );
	}
	else
	{
		return (boolean)
				( measurement_value <
					prior_measurement_value - threshold );
	}

} /* measurement_spike_exceed_threshold() */
