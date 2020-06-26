/* $APPASERVER_HOME/src_hydrology/station.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "station.h"
#include "station_datatype.h"
#include "shef_datatype_code.h"
#include "timlib.h"
#include "piece.h"

STATION *station_new( char *station_name )
{
	STATION *s = (STATION *)calloc( 1, sizeof( STATION ) );

	if ( !s )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	s->station_name = station_name;
	return s;
} /* station_new() */

static LIST *global_station_list = {0};

double station_get_ground_surface_elevation_ft(
				char **station_elevation_null,
				char *application_name,
				char *station_name )
{
	STATION *station;

	if ( !global_station_list )
	{
		global_station_list =
			station_get_global_station_list(
				application_name );
	}

	if ( ! ( station =
			station_seek(
				station_name,
				global_station_list ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: station_seek(%s) failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 station_name );
		exit( 1 );
	}

	if ( station->ground_surface_elevation_null )
		*station_elevation_null = station->station_name;
	else
		*station_elevation_null = (char *)0;

	return station->ground_surface_elevation_ft;
} /* station_get_ground_surface_elevation_ft() */

LIST *station_get_agency_station_list(
				char *application_name,
				char *agency )
{
	STATION *station;
	LIST *station_list;

	if ( !global_station_list )
	{
		global_station_list =
			station_get_global_station_list(
				application_name );
	}

	station_list = list_new();

	if ( list_rewind( global_station_list ) )
	{
		do {
			station = list_get_pointer( global_station_list );
			if ( station->agency
			&&   strcmp(	agency,
					station->agency ) == 0 )
			{
				list_append_pointer( station_list, station );
			}
		} while( list_next( global_station_list ) );
	}

	return station_list;

} /* station_get_agency_station_list() */

LIST *station_get_enp_station_list(
				char *application_name )
{
	LIST *station_list = list_new();

	list_append_list(
		station_list,
		station_get_agency_station_list(
			application_name,
			"NPS_ENP" ) );

	return station_list;
} /* station_get_enp_station_list() */

LIST *station_get_waterquality_station_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	LIST *record_list;
	LIST *station_list;
	char *record;
	char piece_buffer[ 1024 ];
	STATION *station;
	char *select_list_string = STATION_WATERQUALITY_SELECT_LIST_STRING;

	station_list = list_new();

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=station			",
		application_name,
		select_list_string );

	record_list = pipe2list( sys_string );

	if ( !list_rewind( record_list ) )
	{
		return (LIST *)0;
	}

	station_list = list_new_list();
	do {
		record = list_get_pointer( record_list );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		station = station_new( strdup( piece_buffer ) );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		station->lat_nad83 = atof( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			record,
			2 );

		station->long_nad83 = atof( piece_buffer );

		list_append_pointer( station_list, station );

	} while( list_next( record_list ) );

	list_free_string_list( record_list );

	return station_list;

} /* station_get_waterquality_station_list() */

LIST *station_get_login_name_station_list(
				char *application_name,
				char *login_name )
{
	LIST *station_list;
	LIST *agency_name_list;
	char *agency_name;

	agency_name_list =
		station_get_agency_name_list(
			application_name, login_name );

	if ( !list_rewind( agency_name_list ) ) return (LIST *)0;

	station_list = list_new();
	do {
		agency_name = list_get_pointer( agency_name_list );
		list_append_list(
			station_list,
			station_get_agency_station_list(
				application_name,
				agency_name ) );
	} while( list_next( agency_name_list ) );

	return station_list;

} /* station_get_login_name_station_list() */

LIST *station_get_agency_name_list(
			char *application_name,
			char *login_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 128 ];

	sprintf( where_clause, "login_name = '%s'", login_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=agency			"
		 "			folder=appaserver_user_agency	"
		 "			where=\"%s\"			",
		 application_name,
		 where_clause );
	return pipe2list( sys_string );
} /* station_get_agency_name_list() */

LIST *station_get_global_station_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	LIST *record_list;
	LIST *station_list;
	char *record;
	char piece_buffer[ 1024 ];
	STATION *station;
	char *select_list_string = STATION_SELECT_LIST_STRING;

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=station			",
		application_name,
		select_list_string );

	record_list = pipe2list( sys_string );

	if ( !list_rewind( record_list ) )
	{
		return (LIST *)0;
	}

	station_list = list_new_list();
	do {
		record = list_get_pointer( record_list );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		station = station_new( strdup( piece_buffer ) );

		station_parse_station_buffer(
			&station->lat_nad83,
			&station->long_nad83,
			&station->agency,
			&station->ground_surface_elevation_ft,
			&station->ground_surface_elevation_null,
			&station->vertical_datum,
			&station->ngvd29_navd88_conversion,
			&station->station_type,
			&station->comments,
			record );

		list_append_pointer( station_list, station );

	} while( list_next( record_list ) );

	list_free_string_list( record_list );
	return station_list;

} /* station_get_global_station_list() */

STATION *station_seek(		char *station_name,
				LIST *station_list )
{
	STATION *station;

	if ( list_rewind( station_list ) )
	{
		do {
			station = list_get_pointer( station_list );

			if ( timlib_strcmp(
					station_name,
					station->station_name ) == 0 )
			{
				return station;
			}
		} while( list_next( station_list ) );
	}

	return (STATION *)0;

} /* station_seek() */

DICTIONARY *station_load_dictionary(
			char *application_name,
			char *station_name )
{
	STATION *station;
	DICTIONARY *dictionary;
	char buffer[ 32 ];

	station = station_new( station_name );

	if ( !station_load(	
				&station->lat_nad83,
				&station->long_nad83,
				&station->agency,
				&station->ground_surface_elevation_ft,
				&station->ground_surface_elevation_null,
				&station->vertical_datum,
				&station->ngvd29_navd88_conversion,
				&station->station_type,
				&station->comments,
				application_name,
				station->station_name ) )
	{
		return (DICTIONARY *)0;
	}

	dictionary = dictionary_small_new();

	dictionary_set_pointer(
		dictionary,
		"station",
		station->station_name );

	dictionary_set_pointer(
		dictionary,
		"agency",
		station->agency );

	dictionary_set_pointer(
		dictionary,
		"station_type",
		station->station_type );

	sprintf( buffer, "%.5lf", station->lat_nad83 );
	dictionary_set_pointer(
		dictionary,
		"lat_nad83",
		strdup( buffer ) );

	sprintf( buffer, "%.5lf", station->long_nad83 );
	dictionary_set_pointer(
		dictionary,
		"long_nad83",
		strdup( buffer ) );

	sprintf( buffer, "%.1lf", station->ground_surface_elevation_ft );
	dictionary_set_pointer(
		dictionary,
		"ground_surface_elevation_ft",
		strdup( buffer ) );

	dictionary_set_pointer(
		dictionary,
		"vertical_datum",
		station->vertical_datum );

	sprintf( buffer, "%.3lf", station->ngvd29_navd88_conversion );
	dictionary_set_pointer(
		dictionary,
		"ngvd29_navd88_conversion",
		strdup( buffer ) );

	dictionary_set_pointer(
		dictionary,
		"comments",
		station->comments );

	return dictionary;

} /* station_load_dictionary() */

boolean station_load(	double *lat_nad83,
			double *long_nad83,
			char **agency,
			double *ground_surface_elevation_ft,
			boolean *ground_surface_elevation_null,
			char **vertical_datum,
			double *ngvd29_navd88_conversion,
			char **station_type,
			char **comments,
			char *application_name,
			char *station_name )
{
	char sys_string[ 1024 ];
	char *record;
	char where_clause[ 128 ];
	char *select_list_string = STATION_SELECT_LIST_STRING;

	sprintf( where_clause,
		 "station = '%s'",
		 station_name );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=station			"
		"			where=\"%s\"			",
		application_name,
		select_list_string,
		where_clause );

	if ( ! ( record = pipe2string( sys_string ) ) )
		return 0;

	station_parse_station_buffer(
			lat_nad83,
			long_nad83,
			agency,
			ground_surface_elevation_ft,
			ground_surface_elevation_null,
			vertical_datum,
			ngvd29_navd88_conversion,
			station_type,
			comments,
			record );

	return 1;

} /* station_load() */

void station_parse_station_buffer(
			double *lat_nad83,
			double *long_nad83,
			char **agency,
			double *ground_surface_elevation_ft,
			boolean *ground_surface_elevation_null,
			char **vertical_datum,
			double *ngvd29_navd88_conversion,
			char **station_type,
			char **comments,
			char *input_buffer )
{
	char piece_buffer[ 256 ];

	if ( character_count( FOLDER_DATA_DELIMITER, input_buffer ) != 8 )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: not the correct number of delimiters in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_buffer );
		exit( 1 );
	}

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		1 );

	if ( *piece_buffer )
		*lat_nad83 = atof( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		2 );

	if ( *piece_buffer )
		*long_nad83 = atof( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		3 );

	if ( *piece_buffer )
		*agency = strdup( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		4 );

	*ground_surface_elevation_null = 0;

	if ( !*piece_buffer )
		*ground_surface_elevation_null = 1;
	else
		*ground_surface_elevation_ft =
			atof( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		5 );

	if ( *piece_buffer )
		*vertical_datum = strdup( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		6 );

	if ( *piece_buffer )
		*ngvd29_navd88_conversion = atof( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		7 );

	if ( *piece_buffer )
		*station_type = strdup( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		8 );

	if ( *piece_buffer )
		*comments = strdup( piece_buffer );

} /* station_parse_station_buffer() */

LIST *station_get_rectangle_station_name_list(
			char *application_name,
			double southwest_latitude,
			double southwest_longitude,
			double northeast_latitude,
			double northeast_longitude )
{
	LIST *station_name_list;
	double zone;

	if ( !global_station_list )
	{
		global_station_list =
			station_get_global_station_list(
				application_name );
	}

	for(	zone = 0.0;
		zone <= 0.5;
		zone += 0.005 )
	{

/*
{
char msg[ 1024 ];
sprintf( msg, "%s/%s()/%d: got zone = %.3lf\n",
__FILE__,
__FUNCTION__,
__LINE__,
zone );
m2( "hydrology", msg );
}
*/
		if ( ( station_name_list =
			station_get_zone_rectangle_station_name_list(
				global_station_list,
				southwest_latitude,
				southwest_longitude,
				northeast_latitude,
				northeast_longitude,
				zone ) ) )
		{
			return station_name_list;
		}
	}

	return (LIST *)0;

} /* station_get_rectangle_station_name_list() */

LIST *station_get_zone_rectangle_station_name_list(
			LIST *global_station_list,
			double southwest_latitude,
			double southwest_longitude,
			double northeast_latitude,
			double northeast_longitude,
			double zone )
{
	STATION *station;
	LIST *station_name_list = {0};

	southwest_latitude -= zone;
	southwest_longitude -= zone;
	northeast_latitude += zone;
	northeast_longitude += zone;

	if ( !list_rewind( global_station_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty global_station_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		station = list_get_pointer( global_station_list );

		if ( double_virtually_same( station->lat_nad83, 0.0 ) )
			continue;
		if ( double_virtually_same( station->long_nad83, 0.0 ) )
			continue;

		if ( station->lat_nad83 >= southwest_latitude
		&&   station->lat_nad83 <= northeast_latitude
		&&   station->long_nad83 >= southwest_longitude
		&&   station->long_nad83 <= northeast_longitude )

		{
			if ( !station_name_list )
				station_name_list = list_new();

			list_append_pointer(
				station_name_list,
				station->station_name );
		}

	} while( list_next( global_station_list ) );

	return station_name_list;

} /* station_get_zone_rectangle_station_name_list() */

LIST *station_get_station_type_station_list(
				LIST *station_list,
				char *station_type )
{
	LIST *station_type_list = {0};
	STATION *station;

	if ( !list_rewind( station_list ) ) return (LIST *)0;

	do {
		station = list_get( station_list );

		if ( strcmp( station_type, NULL_OPERATOR ) == 0 )
		{
			if ( !station_type_list )
				station_type_list = list_new();

			if ( !station->station_type )
			{
				list_append_pointer(
					station_type_list,
					station );
			}
		}
		else
		if ( timlib_strcmp( station->station_type, station_type ) == 0 )
		{
			if ( !station_type_list )
				station_type_list = list_new();

			list_append_pointer( station_type_list, station );
		}

	} while( list_next( station_list ) );

	return station_type_list;

} /* station_get_station_type_station_list() */

LIST *station_get_station_type_list(
				char *application_name,
				char *login_name )
{
	char sys_string[ 1024 ];
	char *station;
	char *appaserver_user_agency;
	char where[ 1024 ];

	station = get_table_name( application_name, "station" );

	appaserver_user_agency =
		get_table_name(
			application_name,
			"appaserver_user_agency" );

	sprintf( where,
		 "lat_nad83 is not null and			"
		 "station_type is not null and			"
		 "agency in (	select agency			"
		 "		from %s				"
		 "		where login_name = '%s' )	",
		 appaserver_user_agency,
		 login_name );

	sprintf( sys_string,
	"echo \"select distinct station_type from %s where %s;\"	|"
	 "sql.e								|"
	 "sort								 ",
		 station,
		 where );

	return pipe2list( sys_string );

} /* station_get_station_type_list() */

boolean station_exists(	char *application_name,
			char *station_name )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *return_value;

	sprintf( where, "station = '%s'", station_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=count	"
		 "			folder=station	"
		 "			where=\"%s\"	",
		 application_name,
		 where );

	if ( ! ( return_value = pipe2string( sys_string ) ) ) return 0;

	return (boolean) atoi( return_value );

} /* station_exists() */

STATION *station_fetch_new(	char *application_name,
				char *station_name )
{
	STATION *station;

	station = station_new( station_name );

	/* -------------------------------------------- */
	/* Only shef_upload_datatpe_list for a station. */
	/* -------------------------------------------- */
	station->shef_upload_datatype_list =
		shef_station_fetch_upload_datatype_list(
			application_name,
			station_name );

	station->station_datatype_list =
		station_datatype_fetch_list(
			application_name,
			station_name,
			station->shef_upload_datatype_list );

	return station;

} /* station_fetch_new() */

STATION *station_get_or_set_station(
			LIST *input_station_list,
			char *application_name,
			char *station_name )
{
	STATION *station;

	if ( ( station =
		station_seek(
			station_name,
			input_station_list ) ) )
	{
		return station;
	}

	station = station_fetch_new( application_name, station_name );
	list_append_pointer( input_station_list, station );
	return station;

} /* station_get_or_set_station() */

LIST *station_fetch_station_datatype_list(
			char *application_name,
			char *station_name )
{
	return station_datatype_fetch_list(
			application_name,
			station_name,
			(LIST *)0 /* shef_upload_datatype_list */ );
}

LIST *station_datatype_alias_list(
			char *application_name,
			char *station_name )
{
	char sys_string[ 1024 ];
	char *select;
	LIST *record_list;
	char *record;
	char piece_buffer[ 128 ];
	DATATYPE_ALIAS *datatype_alias;
	LIST *alias_list;
	char where[ 128 ];

	select = "datatype_alias,datatype";

	sprintf( where, "station = '%s'", station_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 "station_datatype_alias",
		 where );

	record_list = pipe2list( sys_string );

	if ( !list_rewind( record_list ) ) return (LIST *)0;

	alias_list = list_new();

	do {
		record = list_get( record_list );

		datatype_alias = datatype_alias_new();

		datatype_alias->datatype_alias =
			strdup( piece(	piece_buffer,
					FOLDER_DATA_DELIMITER,
					record,
					0 ) );

		datatype_alias->datatype_name =
			strdup( piece(	piece_buffer,
					FOLDER_DATA_DELIMITER,
					record,
					1 ) );

		list_append_pointer( alias_list, datatype_alias );

	} while ( list_next( record_list ) );

	return alias_list;
}

