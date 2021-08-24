/* measurement_update_parameter.c					*/
/* -------------------------------------------------------------------- */
/* This is the hydrology measurement_update_parameter ADT.		*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "measurement_update_parameter.h"
#include "appaserver_library.h"
#include "timlib.h"
#include "list.h"
#include "date.h"
#include "dictionary.h"

/* Constants */
/* --------- */
#define PARAMETER_INSERT_LIST	 "station,datatype,measurement_update_method,measurement_update_date,measurement_update_time,login_name,parameter,parameter_value"

#define EVENT_INSERT_LIST	 "station,datatype,measurement_update_method,measurement_update_date,measurement_update_time,login_name,notes"

MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter_new(
				char *application_name,
				char *station,
				char *datatype,
				char *measurement_update_method,
				char *login_name,
				DICTIONARY *parameter_dictionary,
				char *notes )
{
	MEASUREMENT_UPDATE_PARAMETER *p;

	p = (MEASUREMENT_UPDATE_PARAMETER *)
		calloc( 1, sizeof( MEASUREMENT_UPDATE_PARAMETER ) );
	p->application_name = application_name;
	p->station = station;
	p->datatype = datatype;
	p->measurement_update_method = measurement_update_method;
	p->login_name = login_name;
	p->parameter_dictionary = parameter_dictionary;

	p->now_date =
		date_get_now_date_yyyy_mm_dd(
			date_get_utc_offset() );

	p->now_time =
		date_get_now_time_hhmm_colon_ss(
			date_get_utc_offset() );

	p->notes = notes;

	return p;

} /* measurement_update_parameter_new() */

void measurement_update_parameter_save(	MEASUREMENT_UPDATE_PARAMETER *
					measurement_update_parameter )
{
	measurement_update_parameter_save_event(
			measurement_update_parameter->application_name,
			measurement_update_parameter->station,
			measurement_update_parameter->datatype,
			measurement_update_parameter->measurement_update_method,
			measurement_update_parameter->login_name,
			measurement_update_parameter->now_date,
			measurement_update_parameter->now_time,
			measurement_update_parameter->notes );

	measurement_update_parameter_save_parameter(
			measurement_update_parameter->application_name,
			measurement_update_parameter->station,
			measurement_update_parameter->datatype,
			measurement_update_parameter->measurement_update_method,
			measurement_update_parameter->login_name,
			measurement_update_parameter->parameter_dictionary,
			measurement_update_parameter->now_date,
			measurement_update_parameter->now_time );
}


void measurement_update_parameter_save_event(
				char *application_name,
				char *station,
				char *datatype,
				char *measurement_update_method,
				char *login_name,
				char *now_date,
				char *now_time,
				char *notes )
{
	char sys_string[ 4096 ];
	char *table_name;
	FILE *p;

	if ( !notes || strcmp( notes, "notes" ) == 0 )
		notes = "";
	else
		search_replace_string( notes, "\"", "'" );

	table_name = get_table_name( application_name,
				     "measurement_update_event" );

	sprintf( sys_string,
		 "insert_statement.e %s %s '^'	|"
		 "tee_appaserver_error.sh	|"
		 "sql.e				 ",
		 table_name,
		 EVENT_INSERT_LIST );
	p = popen( sys_string, "w" );

	fprintf( p,
		 "%s^%s^%s^%s^%s^%s^%s\n",
		 station,
		 datatype,
		 measurement_update_method,
		 now_date,
		 now_time,
		 login_name,
		 notes );
	pclose( p );
} /* measurement_update_parameter_save_event() */

void measurement_update_parameter_save_parameter(
				char *application_name,
				char *station,
				char *datatype,
				char *measurement_update_method,
				char *login_name,
				DICTIONARY *parameter_dictionary,
				char *now_date,
				char *now_time )
{
	char sys_string[ 4096 ];
	char key_bar_separated[ 1024 ];
	char data_bar_separated[ 1024 ];
	LIST *key_list = {0};
	char *key, *data;
	char *table_name;
	FILE *p;

	if ( parameter_dictionary )
	{
		key_list = dictionary_get_key_list( parameter_dictionary );
	}

	if ( !key_list || !list_rewind( key_list ) )
	{
		return;
	}

	table_name = get_table_name( application_name,
				     "measurement_update_parameter" );

	sprintf( sys_string,
		 "insert_statement.e %s %s '^'	|"
		 "tee_appaserver_error.sh	|"
		 "sql.e				 ",
		 table_name,
		 PARAMETER_INSERT_LIST );
	p = popen( sys_string, "w" );

	do {
		key = list_get_string( key_list );

		if ( strncmp( key, "relation_operator_", 18 ) == 0 ) continue;
		if ( strcmp( key, "from_really_yn" ) == 0 ) continue;
		if ( strcmp( key, "notes" ) == 0 ) continue;

		data =
			dictionary_get( 
				key,
				parameter_dictionary );

		if ( data && *data )
		{
			timlib_strcpy( key_bar_separated, key, 1024 );
			search_replace_string( key_bar_separated, "^", "|" );

			timlib_strcpy( data_bar_separated, data, 1024 );
			search_replace_string( data_bar_separated, "^", "|" );

			fprintf( p,
				 "%s^%s^%s^%s^%s^%s^%s^%s\n",
				 station,
				 datatype,
				 measurement_update_method,
				 now_date,
				 now_time,
				 login_name,
				 key_bar_separated,
				 data_bar_separated );
		}
	} while( list_next( key_list ) );
	pclose( p );
} /* measurement_update_parameter_save_parameter() */

