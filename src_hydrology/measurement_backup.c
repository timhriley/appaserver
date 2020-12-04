/* measurement_backup.c 						*/
/* -------------------------------------------------------------------- */
/* This is the hydrology measurement_backup ADT.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include "measurement_backup.h"
#include "appaserver_library.h"
#include "date.h"
#include "piece.h"
#include "String.h"
#include "timlib.h"

MEASUREMENT_BACKUP *measurement_backup_calloc( void )
{
	MEASUREMENT_BACKUP *measurement_backup;

	if ( ! ( measurement_backup =
			(MEASUREMENT_BACKUP *)
				calloc( 1,
					sizeof( MEASUREMENT_BACKUP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return measurement_backup;
}

MEASUREMENT_BACKUP *measurement_backup_new(
				char *application_name,
				char *measurement_update_method,
				char *login_name,
				char *measurement_update_date,
				char *measurement_update_time )
{
	return new_measurement_backup(	
			application_name,
			measurement_update_method,
			login_name,
			measurement_update_date,
			measurement_update_time );
}

MEASUREMENT_BACKUP *new_measurement_backup(	
					char *application_name,
					char *measurement_update_method,
					char *login_name,
					char *measurement_update_date,
					char *measurement_update_time )
{
	MEASUREMENT_BACKUP *m;

	m = measurement_backup_calloc();
	m->application_name = application_name;
	m->measurement_update_method = measurement_update_method;
	m->login_name = login_name;
	m->measurement_update_date = measurement_update_date;
	m->measurement_update_time = measurement_update_time;
	return m;
}

FILE *measurement_backup_open_insert_pipe(
					char *application_name )
{
	char sys_string[ 1024 ];
	char *table_name;

	table_name = 
		get_table_name( application_name, "measurement_backup" );

	sprintf( sys_string,
		 "insert_statement.e %s %s | sql.e 2>&1 | grep -v Duplicate",
		 table_name,
		 MEASUREMENT_BACKUP_INSERT_COLUMN_COMMA_LIST );

	return popen( sys_string, "w" );

}


void measurement_backup_insert(	FILE *insert_pipe,
				char *measurement_update_date,
				char *measurement_update_time,
				char *measurement_update_method,
				char *login_name,
				char *comma_delimited_record,
				char really_yn,
				char delimiter )
{
	char station[ 128 ];
	char datatype[ 128 ];
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char original_value[ 128 ];

	if ( really_yn != 'y' ) return;

	if ( character_count( delimiter, comma_delimited_record ) <
	     MEASUREMENT_BACKUP_VALUE_PIECE )
	{
		fprintf( stderr,
	"WARNING in %s(): not enough fields in (%s)\n",
			 __FUNCTION__,
			 comma_delimited_record );
		return;
	}

	piece( 	station,
		delimiter,
		comma_delimited_record,
		MEASUREMENT_BACKUP_STATION_PIECE );

	piece( 	datatype,
		delimiter,
		comma_delimited_record,
		MEASUREMENT_BACKUP_DATATYPE_PIECE );

	piece(	measurement_date,
		delimiter,
		comma_delimited_record,
		MEASUREMENT_BACKUP_DATE_PIECE );

	piece(	measurement_time,
		delimiter,
		comma_delimited_record,
		MEASUREMENT_BACKUP_TIME_PIECE );

	piece( 	original_value,
		delimiter,
		comma_delimited_record,
		MEASUREMENT_BACKUP_VALUE_PIECE );

	fprintf( insert_pipe,
		 "%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
		 station,
		 datatype,
		 measurement_date,
		 measurement_time,
		 measurement_update_date,
		 measurement_update_time,
		 measurement_update_method,
		 login_name,
		 original_value );

	return;
}

void measurement_backup_close( FILE *insert_pipe )
{
	if ( insert_pipe ) pclose( insert_pipe );
}

char *measurement_backup_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" \"%s\"",
		 MEASUREMENT_BACKUP_TABLE,
		 where,
		 "measurement_update_date,measurement_update_time" );

	return strdup( sys_string );
}

LIST *measurement_backup_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *measurement_backup_list;

	input_pipe = popen( sys_string, "r" );
	measurement_backup_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			measurement_backup_list,
			measurement_backup_parse(
				input ) );
	}
	pclose( input_pipe );
	return measurement_backup_list;
}

FILE *measurement_backup_html_table_open( void )
{
	char sys_string[ 1024 ];
	char *heading_comma_string;
	char *justify_string;

	heading_comma_string =
		"update_date,"
		"update_time,"
		"station,"
		"datatype,"
		"measurement_date,"
		"measurement_time,"
		"method/login,"
		"original_value";

	justify_string = "left,left,left,left,left,left,left,right";

	sprintf( sys_string,
		 "html_table.e '' '%s' '%c' '%s'",
		 heading_comma_string,
		 '^',
		 justify_string );

	return popen( sys_string, "w" );
}

void measurement_backup_list_table_display(
			LIST *measurement_backup_list )
{
	MEASUREMENT_BACKUP *measurement_backup;
	FILE *output_pipe;

	output_pipe = measurement_backup_html_table_open();

	if ( !list_rewind( measurement_backup_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		measurement_backup = list_get( measurement_backup_list );

		measurement_backup_pipe_output(
			output_pipe,
			measurement_backup );

	} while ( list_next( measurement_backup_list ) );

	pclose( output_pipe );
}

boolean measurement_backup_pipe_output(
			FILE *output_pipe,
			MEASUREMENT_BACKUP *measurement_backup )
{
	char method_login[ 128 ];

	if ( !measurement_backup )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty measurement_backup.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	if ( !output_pipe ) output_pipe = stdout;

	if ( measurement_backup->measurement_update_method
	&&   measurement_backup->login_name )
	{
		sprintf(method_login,
			"%s/%s",
			measurement_backup->measurement_update_method,
			measurement_backup->login_name );
	}
	else
	{
		*method_login = '\0';
	}

	fprintf(output_pipe,
		"%s^%s^%s^%s^%s^%s^%s^%.4lf\n",
		(measurement_backup->measurement_update_date)
			? measurement_backup->measurement_update_date
			: "",
		(measurement_backup->measurement_update_time)
			? measurement_backup->measurement_update_time
			: "",
		measurement_backup->station,
		measurement_backup->datatype,
		measurement_backup->measurement_date,
		measurement_backup->measurement_time,
		method_login,
		measurement_backup->original_value );

	return 1;
}

void measurement_backup_list_pipe_output(
			FILE *output_pipe,
			LIST *measurement_backup_list )
{
	MEASUREMENT_BACKUP *measurement_backup;

	if ( !list_rewind( measurement_backup_list ) )
	{
		return;
	}

	do {
		measurement_backup = list_get( measurement_backup_list );

		measurement_backup_pipe_output(
			output_pipe,
			measurement_backup );

	} while ( list_next( measurement_backup_list ) );
}

MEASUREMENT_BACKUP *measurement_backup_parse( char *buffer )
{
	MEASUREMENT_BACKUP *measurement_backup;
	char station[ 128 ];
	char datatype[ 128 ];
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char measurement_update_date[ 128 ];
	char measurement_update_time[ 128 ];
	char measurement_update_method[ 128 ];
	char login_name[ 128 ];
	char original_value[ 128 ];


	measurement_backup = measurement_backup_calloc();

	piece( station, SQL_DELIMITER, buffer, 0 );
	measurement_backup->station = strdup( station );

	piece( datatype, SQL_DELIMITER, buffer, 1 );
	measurement_backup->datatype = strdup( datatype );

	piece( measurement_date, SQL_DELIMITER, buffer, 2 );
	measurement_backup->measurement_date = strdup( measurement_date );

	piece( measurement_time, SQL_DELIMITER, buffer, 3 );
	measurement_backup->measurement_time = strdup( measurement_time );

	piece( measurement_update_date, SQL_DELIMITER, buffer, 4 );
	measurement_backup->measurement_update_date =
		strdup( measurement_update_date );

	piece( measurement_update_time, SQL_DELIMITER, buffer, 5 );
	measurement_backup->measurement_update_time =
		strdup( measurement_update_time );

	piece( measurement_update_method, SQL_DELIMITER, buffer, 6 );
	measurement_backup->measurement_update_method =
		strdup( measurement_update_method );

	piece( login_name, SQL_DELIMITER, buffer, 7 );
	measurement_backup->login_name = strdup( login_name );

	piece( original_value, SQL_DELIMITER, buffer, 8 );
	measurement_backup->original_value = atof( original_value );

	return measurement_backup;
}

