/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/date_convert.c	 			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "String.h"
#include "piece.h"
#include "column.h"
#include "timlib.h"
#include "julian.h"
#include "date.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "application.h"
#include "environ.h"
#include "date_convert.h"

enum date_convert_format_enum date_convert_format_string_evaluate(
		char *date_convert_format_string )
{
	if ( string_strcmp(
		date_convert_format_string,
		"international" ) == 0 )
	{
		return date_convert_international;
	}
	else
	if ( string_strcmp(
		date_convert_format_string,
		"american" ) == 0 )
	{
		return date_convert_american;
	}
	else
	if ( string_strcmp(
		date_convert_format_string,
		"military" ) == 0 )
	{
		return date_convert_military;
	}
	else
	{
		return date_convert_unknown;
	}
}

DATE_CONVERT *date_convert_new(
		enum date_convert_format_enum source_enum,
		enum date_convert_format_enum destination_enum,
		char *source_date_string )
{
	DATE_CONVERT *date_convert;
	int character_count;

	date_convert = date_convert_calloc();

	if ( !source_date_string
	||   !*source_date_string )
	{
		date_convert->source_enum = date_convert_unknown;
		return date_convert;
	}

	date_convert->source_enum = source_enum;
	date_convert->destination_enum = destination_enum;
	date_convert->source_date_string = source_date_string;

	if ( strlen( source_date_string ) > 20 )
	{
		date_convert->source_enum = date_convert_unknown;
		return date_convert;
	}

	character_count =
		string_character_count(
			' ',
			source_date_string );

	if ( character_count > 1 )
	{
		date_convert->source_enum = date_convert_unknown;
		return date_convert;
	}

	column( date_convert->date_column_string, 0, source_date_string );

	if ( character_count == 1 )
	{
		column(	date_convert->time_column_string,
			1,
			source_date_string );
	}

	if ( source_enum == date_convert_unknown )
	{
		date_convert->source_enum =
			date_convert_string_evaluate(
				date_convert->date_column_string
					/* source_date_string */ );

		if ( date_convert->source_enum == date_convert_unknown )
		{
			return date_convert;
		}
	}

	date_convert->return_date_string =
		/* ----------------------------------------------- */
		/* Returns heap memory, source_date_string or null */
		/* ----------------------------------------------- */
		date_convert_return_date_string(
			date_convert->source_enum,
			date_convert->destination_enum,
			date_convert->date_column_string
				/* source_date_string */ );

	if ( !date_convert->return_date_string )
	{
		date_convert->source_enum =
			date_convert_string_evaluate(
				date_convert->date_column_string
					/* source_date_string */ );

		if ( date_convert->source_enum == date_convert_unknown )
		{
			return date_convert;
		}

		date_convert->return_date_string =
			date_convert_return_date_string(
				date_convert->source_enum,
				date_convert->destination_enum,
				date_convert->date_column_string
					/* source_date_string */ );
	}

	if ( *date_convert->time_column_string )
	{
		char tmp_string[ 128 ];

		sprintf(tmp_string,
			"%s %s",
			date_convert->return_date_string,
			date_convert->time_column_string );

		if (	date_convert->return_date_string !=
			date_convert->date_column_string )
		{
			free( date_convert->return_date_string );
		}

		date_convert->return_date_string = strdup( tmp_string );
	}

	return date_convert;
}

DATE_CONVERT *date_convert_calloc( void )
{
	DATE_CONVERT *date_convert;

	if ( ! ( date_convert = calloc( 1, sizeof ( DATE_CONVERT ) ) ) )
	{
		fprintf(stderr,
			"Error in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return date_convert;
}

char *date_convert_return_date_string(
		enum date_convert_format_enum source_enum,
		enum date_convert_format_enum destination_enum,
		char *source_date_string )
{
	char *return_date_string = {0};

	if ( source_enum == date_convert_american )
	{
		return_date_string =
			/* ----------------------------------------------- */
			/* Returns heap memory, source_date_string or null */
			/* ----------------------------------------------- */
			date_convert_source_american(
				destination_enum,
				source_date_string );
	}
	else
	if ( source_enum == date_convert_international )
	{
		return_date_string =
			/* ----------------------------------------------- */
			/* Returns heap memory, source_date_string or null */
			/* ----------------------------------------------- */
			date_convert_source_international(
				destination_enum,
				source_date_string );
	}
	else
	if ( source_enum == date_convert_military )
	{
		return_date_string =
			/* ----------------------------------------------- */
			/* Returns heap memory, source_date_string or null */
			/* ----------------------------------------------- */
			date_convert_source_military(
				destination_enum,
				source_date_string );
	}
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid source_enum=%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			source_enum );

		exit( 1 );
	}

	return return_date_string;
}

enum date_convert_format_enum date_convert_string_evaluate(
		char *source_date_string )
{
	if ( date_convert_is_valid_military( source_date_string ) )
		return date_convert_military;
	else
	if ( date_convert_is_valid_international( source_date_string ) )
		return date_convert_international;
	else
	if ( date_convert_is_valid_american( source_date_string ) )
		return date_convert_american;
	else
		return date_convert_unknown;
}

char *date_convert_source_american(
		enum date_convert_format_enum destination_enum,
		char *source_date_string )
{
	if ( !source_date_string || !*source_date_string ) return (char *)0;

	if ( !isdigit( *source_date_string ) ) return (char *)0;

	if ( destination_enum == date_convert_military )
	{
		char *international;
		char *military;

		international =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			date_convert_american_to_international(
				source_date_string );

		if ( !international ) return source_date_string;

		military =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			date_convert_international_to_military(
				international );

		free( international );
		return military;
	}
	else
	if ( destination_enum == date_convert_international )
	{
		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		date_convert_american_to_international(
			source_date_string );
	}
	else
	{
		if ( !date_convert_is_valid_american(
			source_date_string ) )
		{
			return (char *)0;
		}
		else
		{
			return source_date_string;
		}
	}
}

boolean date_convert_is_valid_international( char *date_string )
{
	char day[ 16 ];
	char month[ 16 ];
	char year[ 16 ];
	int day_integer;
	int month_integer;
	int year_integer;

	if ( string_strlen( date_string ) == 0 ) return 0;

	if ( !piece( year, '-', date_string, 0 ) ) return 0;
	if ( !piece( month, '-', date_string, 1 ) ) return 0;
	if ( !piece( day, '-', date_string, 2 ) ) return 0;

	if ( strlen( year ) == 2 )
	{
		strcpy( year, julian_make_y2k_year( year ) );
	}

	month_integer = atoi( month );
	day_integer = atoi( day );
	year_integer = atoi( year );

	return
	date_convert_is_valid_integers(
		year_integer,
		month_integer,
		day_integer );
}

boolean date_convert_is_valid_military( char *date_string )
{
	char year[ 128 ];
	char month[ 128 ];
	char day[ 128 ];
	int day_integer;
	int month_integer;
	int year_integer;

	if ( string_strlen( date_string ) == 0 ) return 0;

	if ( !piece( day, '-', date_string, 0 ) ) return 0;
	if ( !piece( month, '-', date_string, 1 ) ) return 0;
	if ( !piece( year, '-', date_string, 2 ) ) return 0;

	if ( strlen( year ) == 2 )
	{
		strcpy( year, julian_make_y2k_year( year ) );
	}

	day_integer = atoi( day );
	month_integer = date_military_month_integer( month );
	year_integer = atoi( year );

	return
	date_convert_is_valid_integers(
		year_integer,
		month_integer,
		day_integer );
}

boolean date_convert_is_valid_american( char *date_string )
{
	char day[ 128 ];
	char month[ 128 ];
	char year[ 128 ];
	int day_integer;
	int month_integer;
	int year_integer;

	if ( string_strlen( date_string ) == 0 ) return 0;

	if ( !piece( month, '/', date_string, 0 ) ) return 0;
	if ( !piece( day, '/', date_string, 1 ) ) return 0;
	if ( !piece( year, '/', date_string, 2 ) ) return 0;

	if ( strlen( year ) == 2 )
	{
		strcpy( year, julian_make_y2k_year( year ) );
	}

	month_integer = atoi( month );
	day_integer = atoi( day );
	year_integer = atoi( year );

	return
	date_convert_is_valid_integers(
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

char *date_convert_american_sans_slashes( char *compressed_date_string )
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

enum date_convert_format_enum date_convert_login_name_enum(
		char *application_name,
		char *login_name )
{
	APPLICATION *application;
	APPASERVER_USER *appaserver_user;

	appaserver_user =
		/* Safely returns */
		/* -------------- */
		appaserver_user_login_fetch(
			login_name,
			0 /* fetch_role_name_list */ );

	if ( appaserver_user->user_date_format
	&&   *appaserver_user->user_date_format )
	{
		return
		date_convert_format_string_evaluate(
			appaserver_user->user_date_format );
	}

	if ( ! ( application =
			application_fetch(
				application_name ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: application_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( application->user_date_format
	&&   *application->user_date_format )
	{
		return
		date_convert_format_string_evaluate(
			application->user_date_format );
	}
	else
	{
		return date_convert_unknown;
	}
}

char *date_convert_international_to_military( char *source_date_string )
{
	char military[ 128 ];
	int mm_int;
	char year[ 16 ], mm[ 16 ], dd[ 16 ];

	if ( !piece( year, '-', source_date_string, 0 ) ) return (char *)0;
	if ( !piece( mm, '-', source_date_string, 1 ) ) return (char *)0;
	if ( !piece( dd, '-', source_date_string, 2 ) ) return (char *)0;

	mm_int = atoi( mm );

	if ( mm_int < 1 || mm_int > 12 ) return (char *)0;

	if ( strlen( year ) == 2 )
	{
		strcpy( year, julian_make_y2k_year( year ) );
	}

	sprintf(military, 
		"%.2d-%s-%s",
		atoi( dd ),
		date_month_military( mm_int - 1 /* month_offset */ ),
		year );

	return strdup( military );
}

char *date_convert_international_to_american( char *source_date_string )
{
	char day[ 16 ];
	char month[ 16 ];
	char year[ 16 ];
	char american[ 16 ];

	if ( string_character_count( '-', source_date_string ) != 2 )
		return (char *)0;

	piece( year, '-', source_date_string, 0 );

	if ( strlen( year ) == 2 )
	{
		strcpy( year, julian_make_y2k_year( year ) );
	}

	piece( month, '-', source_date_string, 1 );
	piece( day, '-', source_date_string, 2 );

	sprintf(american,
		"%.2d/%.2d/%s",
		atoi( month ),
		atoi( day ),
		year );

	return strdup( american );
}

char *date_convert_military_to_international( char *source_date_string )
{
	char international[ 128 ];
	int mm_int;
	char yyyy[ 128 ], mon[ 128 ], dd[ 128 ];

	if ( string_character_count( '-', source_date_string ) != 2 )
		return (char *)0;

	if ( !piece( dd, '-', source_date_string, 0 ) ) return (char *)0;
	if ( !piece( mon, '-', source_date_string, 1 ) ) return (char *)0;
	if ( !piece( yyyy, '-', source_date_string, 2 ) ) return (char *)0;

	if ( strlen( yyyy ) == 2 )
	{
		strcpy( yyyy, julian_make_y2k_year( yyyy ) );
	}

	mm_int =
		date_military_month_integer(
			mon );

	if ( mm_int == 0 ) return (char *)0;

	sprintf(international, 
		"%s-%.2d-%.2d",
		yyyy,
		mm_int,
		atoi( dd ) );

	return strdup( international );
}

char *date_convert_american_to_international( char *source_date_string )
{
	char international[ 128 ];
	char day[ 128 ];
	char month[ 128 ];
	char year[ 128 ];

	if ( string_character_count( '/', source_date_string ) != 2 )
		return (char *)0;

	if ( !piece( month, '/', source_date_string, 0 ) ) return (char *)0;
	if ( !piece( day, '/', source_date_string, 1 ) ) return (char *)0;
	if ( !piece( year, '/', source_date_string, 2 ) ) return (char *)0;

	/* If like: 2019/08/25 */
	/* ------------------- */
	if ( strlen( month ) == 4 )
	{
		search_replace_character( source_date_string, '/', '-' );
		strcpy( international, source_date_string );
		return strdup( international );
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

	return strdup( international );
}

char *date_convert_source_military(
		enum date_convert_format_enum destination_enum,
		char *source_date_string )
{
	if ( !source_date_string || !*source_date_string )
		return source_date_string;

	if ( destination_enum == date_convert_international )
	{
		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		date_convert_military_to_international(
			source_date_string );
	}
	else
	if ( destination_enum == date_convert_american )
	{
		char *international;
		char *american;

		international =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			date_convert_military_to_international(
				source_date_string );

		if ( !international ) return source_date_string;

		american =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			date_convert_international_to_american(
				international /* source_date_string */ );

		free( international );
		return american;
	}
	else
	{
		if ( !date_convert_is_valid_military(
			source_date_string ) )
		{
			return (char *)0;
		}
		else
		{
			return source_date_string;
		}
	}
}

char *date_convert_source_international(
		enum date_convert_format_enum destination_enum,
		char *source_date_string )
{
	if ( !source_date_string || !*source_date_string ) return (char *)0;

	if ( !isdigit( *source_date_string ) ) return (char *)0;

	if ( destination_enum == date_convert_military )
	{
		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		date_convert_international_to_military(
			source_date_string );
	}
	else
	if ( destination_enum == date_convert_american )
	{
		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		date_convert_international_to_american(
			source_date_string );
	}
	else
	{
		if ( !date_convert_is_valid_international(
			source_date_string ) )
		{
			return (char *)0;
		}
		else
		{
			return source_date_string;
		}
	}
}

char *date_convert_format_string(
		enum date_convert_format_enum date_convert_format_enum )
{
	if ( date_convert_format_enum == date_convert_international )
		return "international";
	else
	if ( date_convert_format_enum == date_convert_american )
		return "american";
	else
	if ( date_convert_format_enum == date_convert_military )
		return "military";
	else
		return "unknown";
}

char *date_convert_select_format_string(
		enum date_convert_format_enum date_convert_format_enum )
{
	static char format_string[ 16 ];

	if ( date_convert_format_enum == date_convert_international )
	{
		sprintf( format_string,
			 "%cY-%cm-%cd",
		 	 '%','%','%' );
	}
	else
	if ( date_convert_format_enum == date_convert_american )
	{
		sprintf( format_string,
			 "%cm/%cd/%cY",
		 	 '%','%','%' );
	}
	else
	if ( date_convert_format_enum == date_convert_military )
	{
		sprintf( format_string,
			 "%cd-%cb-%cY",
		 	 '%','%','%' );
	}
	else
	{
		sprintf( format_string,
			 "%cY-%cm-%cd",
		 	 '%','%','%' );
	}

	return format_string;
}

DATE_CONVERT *date_convert_destination_international(
		char *application_name,
		char *login_name,
		char *date_string )
{
	static enum date_convert_format_enum
		source_enum = date_convert_blank;
	DATE_CONVERT *date_convert;

	if ( !application_name
	||   !login_name
	||   !date_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( source_enum == date_convert_blank )
	{
		source_enum =
			date_convert_login_name_enum(
				application_name,
				login_name );
	}

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		date_convert_new(
			source_enum,
			date_convert_international
				/* destination_enum */,
			date_string );

	return date_convert;
}

void date_convert_free( DATE_CONVERT *date_convert )
{
	if ( !date_convert ) return;

	if (	date_convert->return_date_string
	&&   	date_convert->return_date_string !=
		date_convert->date_column_string )
	{
		free( date_convert->return_date_string );
	}

	free( date_convert );
}
