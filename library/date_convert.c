/* $APPASERVER_HOME/library/date_convert.c	 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "piece.h"
#include "column.h"
#include "timlib.h"
#include "julian.h"
#include "date.h"
#include "appaserver_user.h"
#include "application.h"
#include "date_convert.h"

enum date_convert_format date_convert_get_database_date_format(
			char *application_name )
{
	char sys_string[ 1024 ];
	char where_string[ 128 ];
	char *results;
	static enum date_convert_format date_format = date_convert_unknown;

	if ( date_format != date_convert_unknown ) return date_format;

	sprintf( where_string, "application = '%s'", application_name );
	
	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=database_date_format	"
		 "			folder=application		"
		 "			where=\"%s\"			",
		 application_name,
		 where_string );
	
	results = pipe2string( sys_string );
	if ( !results || !*results ) return date_convert_unknown;
	
	date_format = date_convert_format_evaluate( results );
	return date_format;
}

enum date_convert_format date_convert_user_date_format(
			char *application_name,
			char *login_name )
{
	char sys_string[ 1024 ];
	char where_string[ 128 ];
	enum date_convert_format user_date_format;

	/* Get from APPASERVER_USER */
	/* ------------------------ */
	sprintf( where_string, "login_name = '%s'", login_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=user_date_format		"
		 "			folder=appaserver_user		"
		 "			where=\"%s\"			",
		 application_name,
		 where_string );

	user_date_format =
		date_convert_format_evaluate(
			string_pipe_fetch( sys_string ) );

	if ( user_date_format == date_convert_unknown )
	{
		/* Get from APPLICATION */
		/* -------------------- */
		sprintf( where_string, "application = '%s'", application_name );
	
		sprintf( sys_string,
			 "get_folder_data	application=%s		"
			 "			select=user_date_format	"
		 	"			folder=application	"
			 "			where=\"%s\"		",
			 application_name,
			 where_string );

		user_date_format =
			date_convert_format_evaluate(
				string_pipe_fetch( sys_string ) );
	}

	return user_date_format;
}

enum date_convert_format date_convert_get_user_date_format(
			char *application_name,
			char *login_name )
{
	return date_convert_user_date_format(
			application_name,
			login_name );
}

DATE_CONVERT *date_convert_international(
			char *application_name,
			char *login_name,
			char *date_string )
{
	enum date_convert_format user_date_format;

	user_date_format =
		date_convert_user_date_format(
			application_name,
			login_name );

	return date_convert_string_international(
			user_date_format,
			date_string );
}

DATE_CONVERT *date_convert_new(
			enum date_convert_format source_format,
			enum date_convert_format destination_format )
{
	DATE_CONVERT *date_convert = date_convert_calloc();

	date_convert->source_format = source_format;
	date_convert->destination_format = destination_format;

	return date_convert;
}

DATE_CONVERT *date_convert_evaluate(
			DATE_CONVERT *date_convert,
			char *date_string )
{
	if ( !date_convert )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: date_convert is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	date_convert_populate_return_date(
		date_convert->return_date,
		date_convert->source_format,
		date_convert->destination_format,
		date_string );

	return date_convert;
}

DATE_CONVERT *date_convert_calloc( void )
{
	DATE_CONVERT *d;

	if ( ! ( d = (DATE_CONVERT *)calloc( 1, sizeof( DATE_CONVERT ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return d;
}

DATE_CONVERT *date_convert_string_international(
			enum date_convert_format source_format,
			char *date_string )
{
	DATE_CONVERT *d = date_convert_calloc();

	if ( !d )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__ );
		exit( 1 );
	}

	d->source_format = source_format;
	d->destination_format = international;

	date_convert_populate_return_date(
		d->return_date,
		d->source_format,
		d->destination_format,
		date_string );

	return d;
}

DATE_CONVERT *date_convert_string_date_convert(
			enum date_convert_format destination_format,
			char *date_string )
{
	DATE_CONVERT *d = date_convert_calloc();

	if ( !d )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__ );
		exit( 1 );
	}

	if ( strcasecmp( date_string, "today" ) == 0 )
		date_string =
			date_today_yyyy_mm_dd(
				date_utc_offset() );
	else
	if ( strcasecmp( date_string, "yesterday" ) == 0 )
		date_string =
			date_yesterday_yyyy_mm_dd(
				date_utc_offset() );
	else
	if ( strcasecmp( date_string, "monday" ) == 0 )
		date_string =
			date_day_of_week_yyyy_mm_dd(
				WDAY_MONDAY,
				date_utc_offset() );
	else
	if ( strcasecmp( date_string, "tuesday" ) == 0 )
		date_string =
			date_day_of_week_yyyy_mm_dd(
				WDAY_TUESDAY,
				date_utc_offset() );
	else
	if ( strcasecmp( date_string, "wednesday" ) == 0 )
		date_string =
			date_day_of_week_yyyy_mm_dd(
				WDAY_WEDNESDAY,
				date_utc_offset() );
	else
	if ( strcasecmp( date_string, "thursday" ) == 0 )
		date_string =
			date_day_of_week_yyyy_mm_dd(
				WDAY_THURSDAY,
				date_utc_offset() );
	else
	if ( strcasecmp( date_string, "friday" ) == 0 )
		date_string =
			date_day_of_week_yyyy_mm_dd(
				WDAY_FRIDAY,
				date_utc_offset() );
	else
	if ( strcasecmp( date_string, "saturday" ) == 0 )
		date_string =
			date_day_of_week_yyyy_mm_dd(
				WDAY_SATURDAY,
				date_utc_offset() );
	else
	if ( strcasecmp( date_string, "sunday" ) == 0 )
		date_string =
			date_day_of_week_yyyy_mm_dd(
				WDAY_SUNDAY,
				date_utc_offset() );

	d->source_format = date_convert_date_format( date_string );
	d->destination_format = destination_format;

	if ( d->source_format == date_convert_unknown
	||   d->destination_format == date_convert_unknown )
	{
		strcpy( d->return_date, date_string );
		return d;
	}

	d->source_format =
		date_convert_populate_return_date(
			d->return_date,
			d->source_format,
			d->destination_format,
			date_string );

	return d;
}

DATE_CONVERT *date_convert_new_database_format_date_convert(
			char *application_name,
			char *date_string )
{
	enum date_convert_format database_date_format;

	database_date_format =
		date_convert_get_database_date_format(
			application_name );

	return date_convert_string_date_convert(
			database_date_format,
			date_string );
}

enum date_convert_format date_convert_populate_return_date(
			char *return_date,
			enum date_convert_format source_format,
			enum date_convert_format destination_format,
			char *date_string )
{
	if ( source_format == date_convert_unknown )
	{
		source_format = date_convert_date_format( date_string );
	}

	if ( source_format == american )
	{
		if ( !date_convert_source_american(
			return_date,
			destination_format,
			date_string ) )
		{
			return 0;
		}
	}
	else
	if ( source_format == international )
	{
		date_convert_source_international(
			return_date,
			destination_format,
			date_string );
	}
	else
	if ( source_format == military )
	{
		date_convert_source_military(
			return_date,
			destination_format,
			date_string );
	}
	else
	{
		*return_date = '\0';
		source_format = date_convert_unknown;
	}

	return source_format;

}

enum date_convert_format date_convert_date_format(
			char *date_time_string )
{
	return date_convert_date_time_evaluate( date_time_string );
}

enum date_convert_format date_convert_date_time_evaluate(
			char *date_time_string )
{
	char delimiter;
	char year_string[ 128 ];
	char date_string[ 128 ];

	column( date_string, 0, date_time_string );

	if ( !timlib_exists_numeric( date_string ) )
		return date_convert_unknown;

	if ( timlib_exists_alpha( date_string ) )
	{
		if ( date_convert_is_valid_military( date_string ) )
			return military;
		else
			return date_convert_unknown;
	}

	delimiter = date_convert_get_delimiter( date_string );

	if ( !delimiter ) return date_convert_unknown;

	if ( character_count( delimiter, date_string ) != 2 )
		return date_convert_unknown;

	if ( delimiter == '/' )
	{
		piece( year_string, delimiter, date_string, 2 );

		if ( strlen( year_string ) == 2 )
		{
			strcpy( year_string,
				julian_make_y2k_year( year_string ) );
		}

		if ( strlen( year_string ) == 4
		&& ( atoi( year_string ) > 1700 ) )
		{
			return american;
		}
		else
		{
			return date_convert_unknown;
		}
	}
	else
	if ( delimiter == '-' )
	{
		piece( year_string, delimiter, date_string, 0 );

		if ( ( strcmp( year_string, "0000" ) == 0 )
		||   ( atoi( year_string ) > 1700 ) )
		{
			return international;
		}
		else
		{
			return date_convert_unknown;
		}
	}
	else
	{
		return date_convert_unknown;
	}
}

void date_convert_free(	DATE_CONVERT *date_convert )
{
	free( date_convert );
}

boolean date_convert_date_time_source_unknown(
		char *return_date_time,
		enum date_convert_format destination_format,
		char *date_time_string )
{
	enum date_convert_format source_format;
	char date_string[ 128 ];
	char time_string[ 128 ];
	char buffer[ 128 ];

	if ( !return_date_time )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: null return_date_time\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	*return_date_time = '\0';
	*buffer = '\0';

	if ( !character_exists( date_string, ' ' ) ) return 0;

	column( date_string, 0, date_time_string );
	column( time_string, 1, date_time_string );

	source_format = date_convert_date_format( date_string );

	if ( source_format == american )
	{
		if ( !date_convert_source_american(
					buffer,
					destination_format,
					date_string ) )
		{
			return 0;
		}

		sprintf( return_date_time,
			 "%s %s",
			 buffer,
			 time_string );
	}
	else
	if ( source_format == international )
	{
		if ( !date_convert_source_international(
					buffer,
					destination_format,
					date_string ) )
		{
			return 0;
		}

		sprintf( return_date_time,
			 "%s %s",
			 buffer,
			 time_string );
	}
	else
	if ( source_format == military )
	{
		if ( !date_convert_source_military(
					buffer,
					destination_format,
					date_string ) )
		{
			return 0;	
		}

		sprintf( return_date_time,
			 "%s %s",
			 buffer,
			 time_string );
	}
	else
	{
		return 0;
	}

	return 1;

}

boolean date_convert_source_unknown(
		char *return_date,
		enum date_convert_format destination_format,
		char *date_string )
{
	enum date_convert_format source_format;

	/* If date-space-time */
	/* ------------------ */
	if ( character_exists( date_string, ' ' ) )
	{
		char buffer[ 128 ];

		column( buffer, 0, date_string );
		date_string = strdup( buffer );
	}

	source_format = date_convert_date_format( date_string );

	if ( source_format == american )
	{
		if ( !date_convert_source_american(
			return_date,
			destination_format,
			date_string ) )
		{
			return 0;
		}
	}
	else
	if ( source_format == international )
	{
		return date_convert_source_international(
			return_date,
			destination_format,
			date_string );
	}
	else
	if ( source_format == military )
	{
		date_convert_source_military(
			return_date,
			destination_format,
			date_string );
	}
	else
	{
		*return_date = '\0';
		return 0;
	}

	return 1;

}

boolean date_convert_source_american(
			char *return_date,
			enum date_convert_format destination_format,
			char *date_string )
{
	if ( !date_string ) return 0;
	if ( !return_date ) return 0;

	if ( destination_format == military )
	{
		char international[ 16 ];

		strcpy( international,
			date_convert_american2international(
				date_string ) );

		strcpy(	return_date,
			timlib_yyyymmdd_to_oracle_format(
				international ) );
	}
	else
	if ( destination_format == international )
	{
		strcpy( return_date,
			date_convert_american2international(
				date_string ) );

		return date_convert_is_valid_international( return_date );
	}
	else
	{
		char day[ 16 ];
		char month[ 16 ];
		char year[ 16 ];

		piece( month, '/', date_string, 0 );
		piece( day, '/', date_string, 1 );
		piece( year, '/', date_string, 2 );

		if ( strlen( year ) == 2 )
			strcpy( year, julian_make_y2k_year( year ) );

		sprintf(	return_date,
				"%s/%s/%s",
				month,
				day,
				year );
	}

	return 1;

}

boolean date_convert_source_military(
			char *return_date,
			enum date_convert_format destination_format,
			char *date_string )
{
	if ( !date_string ) return 0;

	if ( destination_format == american )
	{
		char international[ 16 ];

		strcpy( international,
			timlib_oracle_date2mysql_date_string(
				date_string ) );
		strcpy(	return_date,
			date_convert_international2american(
				international ) );
	}
	else
	if ( destination_format == international )
	{
		strcpy( return_date,
			timlib_oracle_date2mysql_date_string(
				date_string ) );
	}
	else
	{
		strcpy( return_date, date_string );
	}

	return 1;

}

boolean date_convert_source_international(
			char *return_date,
			enum date_convert_format destination_format,
			char *date_string )
{
	if ( !date_string ) return 0;

	if ( destination_format == american )
	{
		strcpy(	return_date,
			date_convert_international2american(
				date_string ) );
	}
	else
	if ( destination_format == military )
	{
		strcpy( return_date,
			timlib_yyyymmdd_to_oracle_format(
				date_string ) );
	}
	else
	{
		char delimiter_string[ 2 ];

		*delimiter_string = date_convert_delimiter( date_string );

		*(delimiter_string + 1) = '\0';

		strcpy( return_date, date_string );

		if ( !timlib_exists_character( date_string, '-' ) )
		{
			search_replace_string(
				return_date, delimiter_string, "-" );
		}
	}

	return date_convert_international_correct_format( return_date );

}

char *date_convert_international2american( char *international )
{
	char day[ 16 ];
	char month[ 16 ];
	char year[ 16 ];
	static char american[ 16 ];

	if ( character_count( '-', international ) != 2 )
		return international;

	piece( year, '-', international, 0 );

	if ( strlen( year ) == 2 )
		strcpy( year, julian_make_y2k_year( year ) );

	piece( month, '-', international, 1 );
	piece( day, '-', international, 2 );
	sprintf(american,
		"%.2d/%.2d/%s",
		atoi( month ),
		atoi( day ),
		year );
	return american;
}


char *date_convert_american2international( char *american )
{
	return date_convert_international_string( american );
}

enum date_convert_format date_convert_format_evaluate(
			char *format_string )
{
	if ( strcmp( format_string, "american" ) == 0 )
		return american;
	else
	if ( strcmp( format_string, "international" ) == 0 )
		return international;
	else
	if ( strcmp( format_string, "military" ) == 0 )
		return military;
	else
		return date_convert_unknown;
}

char *date_convert_get_date_format_string(
			enum date_convert_format date_convert_format )
{
	return date_convert_format_string( date_convert_format );
}

char *date_convert_format_string(
			enum date_convert_format date_convert_format )
{
	if ( date_convert_format == american )
		return "american";
	else
	if ( date_convert_format == international )
		return "international";
	else
	if ( date_convert_format == military )
		return "military";
	else
	if ( date_convert_format == date_convert_unknown )
		return "date_convert_unknown";
	else
		return "";
}

char date_convert_get_delimiter( char *date_string )
{
	return date_convert_delimiter( date_string );
}

char date_convert_delimiter( char *date_string )
{
	if ( timlib_exists_character( date_string, '/' ) )
		return '/';
	else
	if ( timlib_exists_character( date_string, '.' ) )
		return '.';
	else
	if ( timlib_exists_character( date_string, '-' ) )
		return '-';
	else
		return 0;
}

boolean date_convert_international_correct_format( char *date_string )
{
	char day[ 16 ];
	char month[ 16 ];
	char year[ 16 ];

	if ( !date_convert_is_valid_international( date_string ) )
	{
		return 0;
	}

	if ( strlen( date_string ) == 10 ) return 1;
	if ( strlen( date_string ) == 0 ) return 0;

	piece( year, '-', date_string, 0 );
	piece( month, '-', date_string, 1 );
	piece( day, '-', date_string, 2 );

	if ( strlen( year ) == 2 )
		strcpy( year, julian_make_y2k_year( year ) );

	sprintf(date_string,
		"%s-%.2d-%.2d",
		year,
		atoi( month ),
		atoi( day ) );

	return 1;
}

boolean date_convert_is_valid_military(
				char *date_string )
{
	char piece_string[ 128 ];
	int str_len = strlen( date_string );
	int day;
	int month;

	if ( character_count( '-', date_string ) != 2 ) return 0;

	if ( str_len > 11 || str_len < 9 ) return 0;

	piece( piece_string, '-', date_string, 0 );
	day = atoi( piece_string );
	if ( day < 1 || day > 31 ) return 0;

	piece( piece_string, '-', date_string, 1 );
	month = timlib_ora_month2integer( piece_string );
	if ( month < 1 ) return 0;

	return 1;
}

boolean date_convert_is_valid_american(
			char *date_string )
{
	return date_convert_valid_american( date_string );
}

boolean date_convert_valid_american(
			char *date_string )
{
	char day[ 128 ];
	char month[ 128 ];
	char year[ 128 ];
	int day_integer;
	int month_integer;
	int year_integer;

	piece( month, '/', date_string, 0 );

	if ( !piece( day, '/', date_string, 1 ) ) return 0;

	if ( !piece( year, '/', date_string, 2 ) ) return 0;

	if ( strlen( year ) == 2 )
	{
		strcpy( year, julian_make_y2k_year( year ) );
	}

	month_integer = atoi( month );
	day_integer = atoi( day );
	year_integer = atoi( year );

	return date_convert_is_valid_integers(
			year_integer,
			month_integer,
			day_integer );
}

boolean date_convert_is_valid_international(
				char *date_string )
{
	char day[ 128 ];
	char month[ 128 ];
	char year[ 128 ];
	int day_integer;
	int month_integer;
	int year_integer;

	if ( strlen( date_string ) != 10 )
	{
		return 0;
	}

	piece( year, '-', date_string, 0 );
	if ( !piece( month, '-', date_string, 1 ) ) return 0;
	if ( !piece( day, '-', date_string, 2 ) ) return 0;

	year_integer = atoi( year );
	month_integer = atoi( month );
	day_integer = atoi( day );

	return date_convert_is_valid_integers(
			year_integer,
			month_integer,
			day_integer );
}

boolean date_convert_is_valid_integers(
			int year_integer,
			int month_integer,
			int day_integer )
{
	if ( day_integer < 1 || day_integer > 31 ) return 0;

	if ( month_integer < 1 || month_integer > 12 ) return 0;

	if ( month_integer == 2 && day_integer > 29 ) return 0;

	if ( ( month_integer == 4
	||     month_integer == 6
	||     month_integer == 9
	||     month_integer == 11 ) && day_integer > 30 ) return 0;

	if ( month_integer == 2
	&&   ( year_integer % 4 )
	&&   day_integer == 29 )
	{
		return 0;
	}

	if ( year_integer < 1492 || year_integer > 3000 ) return 0;

	return 1;
}

char *date_convert_american_sans_slashes(
				char *compressed_date_string )
{
	static char return_date[ 16 ];

	if ( strlen( compressed_date_string ) == 5 )
	{
		*(return_date + 0) = '0';
	}
	else
	if ( strlen( compressed_date_string ) == 6 )
	{
		*(return_date + 0) = *compressed_date_string++;
	}
	else
	{
		return (char *)0;
	}

	*(return_date + 1) = *compressed_date_string++;
	*(return_date + 2) = '/';
	*(return_date + 3) = *compressed_date_string++;
	*(return_date + 4) = *compressed_date_string++;
	*(return_date + 5) = '/';

	if ( *compressed_date_string <= '3' )
	{
		*(return_date + 6) = '2';
		*(return_date + 7) = '0';
	}
	else
	{
		*(return_date + 6) = '1';
		*(return_date + 7) = '9';
	}
	*(return_date + 8) = *compressed_date_string++;
	*(return_date + 9) = *compressed_date_string++;

	return return_date;
}

char *date_convert_display( enum date_convert_format date_convert_format )
{
	return date_convert_get_date_format_string( date_convert_format );
}

char *date_convert_international_string(
			char *american_string )
{
	static char international[ 256 ];
	char day[ 16 ];
	char month[ 16 ];
	char year[ 16 ];

	*international = '\0';

	if ( !date_convert_valid_american(
		american_string ) )
	{
		return international;
	}

	piece( month, '/', american_string, 0 );
	piece( day, '/', american_string, 1 );
	piece( year, '/', american_string, 2 );

	/* If like: 2019/08/25 */
	/* ------------------- */
	if ( strlen( month ) == 4 )
	{
		search_replace_character( american_string, '/', '-' );
		strcpy( international, american_string );
		return international;
	}

	if ( strlen( year ) == 2 )
	{
		strcpy( year, julian_make_y2k_year( year ) );
	}

	sprintf(international,
		"%s-%.2d-%.2d",
		year,
		atoi( month ),
		atoi( day ) );

	return international;
}

DATE_CONVERT *date_convert_login_name_fetch(
			char *login_name )
{
	APPLICATION *application;
	APPASERVER_USER *appaserver_user;

	if ( ! ( appaserver_user =
			appaserver_user_fetch(
				login_name,
				0 /* fetch_role_list */,
				0 /* fetch_session_list */ ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: appaserver_user_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( *appaserver_user->user_date_format )
	{
		return date_convert_new(
			date_convert_unknown,
			date_convert_format_evaluate(
				appaserver_user->user_date_format ) );
	}

	if ( ! ( application = application_fetch() ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: application_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( *application->user_date_format )
	{
		return date_convert_new(
			date_convert_unknown,
			date_convert_format_evaluate(
				application->user_date_format ) );
	}
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: user_date_format is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
}

