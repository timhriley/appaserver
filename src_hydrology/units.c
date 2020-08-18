/* $APPASERVER_HOME/src_hydrology/units.c */
/* -------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "units.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"

UNITS_CONVERTED *units_converted_to_seek(
				char *units_converted_to,
				LIST *units_converted_list )
{
	UNITS_CONVERTED *units_converted;

	if ( !list_rewind( units_converted_list ) )
		return (UNITS_CONVERTED *)0;

	do {
		units_converted =
			list_get_pointer( 
				units_converted_list );

		if ( timlib_strcmp(
			units_converted->units_converted,
			units_converted_to ) == 0 )
		{
			return units_converted;
		}

	} while ( list_next( units_converted_list ) );

	return (UNITS_CONVERTED *)0;
}

UNITS_CONVERTED *units_converted_from_seek(
				char *units_converted_from,
				LIST *units_converted_list )
{
	UNITS_CONVERTED *units_converted;

	if ( !list_rewind( units_converted_list ) )
		return (UNITS_CONVERTED *)0;

	do {
		units_converted =
			list_get_pointer( 
				units_converted_list );

		if ( timlib_strcmp(
			units_converted->units_name,
			units_converted_from ) == 0 )
		{
			return units_converted;
		}

	} while ( list_next( units_converted_list ) );

	return (UNITS_CONVERTED *)0;
}

UNITS_CONVERTED *units_converted_new(
				void )
{
	UNITS_CONVERTED *d;

	if ( ! ( d = calloc( 1, sizeof( UNITS_CONVERTED ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return d;
}

UNITS_ALIAS *units_alias_seek(	LIST *units_alias_list,
				char *units_alias_name )
{
	UNITS_ALIAS *a;

	if ( !list_rewind( units_alias_list ) )
		return (UNITS_ALIAS *)0;

	do {
		a = list_get( units_alias_list );

		if ( timlib_strcmp(	a->units_alias_name,
					units_alias_name ) == 0 ) 
		{
			return a;
		}

	} while( list_next( units_alias_list ) );

	return (UNITS_ALIAS *)0;

} /* units_alias_seek() */

UNITS_ALIAS *units_alias_new( void )
{
	UNITS_ALIAS *d;

	if ( ! ( d = calloc( 1, sizeof( UNITS_ALIAS ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return d;

} /* units_alias_new() */

UNITS *units_new( void )
{
	UNITS *d;

	if ( ! ( d = calloc( 1, sizeof( UNITS ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return d;

} /* units_new() */

UNITS *units_seek_alias_new(	char *application_name,
				char *units_name )
{
	UNITS *u;
	static LIST *local_units_alias_list = {0};
	UNITS_ALIAS *a;

	if ( !units_name ) return (UNITS *)0;

	if ( !local_units_alias_list )
	{
		local_units_alias_list =
			units_fetch_local_units_alias_list(
				application_name );
	}

	u = units_new();

	if ( ( a = units_alias_seek(
			local_units_alias_list,
			units_name /* units_alias_name */ ) ) )
	{
		u->units_name = a->units_name;
	}

	u->units_alias_list =
		units_fetch_units_alias_list(
			application_name,
			u->units_name );

	return u;

} /* units_seek_alias_new() */

LIST *units_fetch_units_converted_list(	char *application_name,
				 	char *units_name )
{
	static LIST *local_units_converted_list = {0};
	LIST *units_converted_list;
	UNITS_CONVERTED *units_converted;

	if ( !units_name ) return (LIST *)0;

	if ( !local_units_converted_list )
	{
		local_units_converted_list =
			units_fetch_local_units_converted_list(
				application_name );
	}

	if ( !list_rewind( local_units_converted_list ) ) return (LIST *)0;

	units_converted_list = list_new();

	do {
		units_converted =
			list_get_pointer(
				local_units_converted_list );

		if ( timlib_strcmp(	units_converted->units_name,
					units_name ) == 0
		||   timlib_strcmp(	units_converted->units_converted,
					units_name ) == 0 )
		{
			list_append_pointer(
				units_converted_list,
				units_converted );
		}

	} while ( list_next( local_units_converted_list ) );

	return units_converted_list;
}

LIST *units_fetch_units_alias_list(	char *application_name,
				 	char *units_name )
{
	static LIST *local_units_alias_list = {0};
	LIST *units_alias_list;
	UNITS_ALIAS *units_alias;

	if ( !units_name ) return (LIST *)0;

	if ( !local_units_alias_list )
	{
		local_units_alias_list =
			units_fetch_local_units_alias_list(
				application_name );
	}

	if ( !list_rewind( local_units_alias_list ) ) return (LIST *)0;

	units_alias_list = list_new();

	do {
		units_alias = list_get_pointer( local_units_alias_list );

		if ( timlib_strcmp(	units_alias->units_name,
					units_name ) == 0 )
		{
			list_append_pointer( units_alias_list, units_alias );
		}

	} while ( list_next( local_units_alias_list ) );

	return units_alias_list;
}

LIST *units_fetch_local_units_converted_list( char *application_name )
{
	char sys_string[ 1024 ];
	char *folder;
	char *select;
	LIST *record_list;
	char *record;
	char piece_buffer[ 128 ];
	LIST *units_converted_list;
	UNITS_CONVERTED *units_converted;

	select = "units,units_converted,multiply_by";
	folder = "units_converted";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	record_list = pipe2list( sys_string );

	if ( !list_rewind( record_list ) ) return (LIST *)0;

	units_converted_list = list_new();

	do {
		record = list_get_pointer( record_list );

		units_converted = units_converted_new();

		piece( piece_buffer, FOLDER_DATA_DELIMITER, record, 0 );
		units_converted->units_name = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, record, 1 );
		units_converted->units_converted = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, record, 2 );
		units_converted->multiply_by = atof( piece_buffer );

		list_append_pointer( units_converted_list, units_converted );

	} while ( list_next( record_list ) );

	return units_converted_list;
}

LIST *units_fetch_local_units_alias_list( char *application_name )
{
	char sys_string[ 1024 ];
	char *folder;
	char *select;
	LIST *record_list;
	char *record;
	char piece_buffer[ 128 ];
	LIST *units_alias_list;
	UNITS_ALIAS *units_alias;

	select = "units_alias,units";
	folder = "units_alias";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	record_list = pipe2list( sys_string );

	if ( !list_rewind( record_list ) ) return (LIST *)0;

	units_alias_list = list_new();

	do {
		record = list_get_pointer( record_list );

		units_alias = units_alias_new();

		piece( piece_buffer, FOLDER_DATA_DELIMITER, record, 0 );
		units_alias->units_alias_name = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, record, 1 );
		units_alias->units_name = strdup( piece_buffer );

		list_append_pointer( units_alias_list, units_alias );

	} while ( list_next( record_list ) );

	return units_alias_list;
}

char *units_select( void )
{
	return "units";
}

UNITS *units_fetch(		char *application_name,
				char *units_name )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	UNITS *units;

	sprintf( where, "units = '%s'", units_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 units_select(),
		 "units",
		 where );

	units_name = pipe2string( sys_string );

	if ( !units_name || !*units_name ) return (UNITS *)0;

	units = units_new();
	units->units_name = units_name;

	units->units_alias_list =
		units_fetch_units_alias_list(
			application_name,
			units->units_name );

	units->units_converted_list =
		units_fetch_units_converted_list(
			application_name,
			units->units_name );

	return units;

} /* units_fetch() */

UNITS *units_seek(	LIST *units_list,
			char *units_name )
{
	UNITS *units;

	if ( !list_rewind( units_list ) ) return (UNITS *)0;

	do {
		units = list_get( units_list );

		if ( timlib_strcmp( units->units_name, units_name ) == 0 )
			return units;

	} while ( list_next( units_list ) );

	return (UNITS *)0;

} /* units_seek() */

LIST *units_list( char *application_name )
{
	return units_fetch_units_list( application_name );
}

LIST *units_fetch_units_list( char *application_name )
{
	char sys_string[ 1024 ];
	LIST *units_name_list;
	LIST *units_list;
	UNITS *units;
	char *units_name;

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 units_select(),
		 "units" );

	units_name_list = pipe2list( sys_string );

	if ( !list_rewind( units_name_list ) ) return (LIST *)0;

	units_list = list_new();

	do {
		units_name = list_get_pointer( units_name_list );

		units = units_new();
		units->units_name = units_name;

		units->units_alias_list =
			units_fetch_units_alias_list(
				application_name,
				units_name );

		list_append_pointer( units_list, units );

	} while ( list_next( units_name_list ) );

	return units_list;

} /* units_fetch_units_list() */

void units_free( UNITS *units )
{
	if ( !units ) return;

	if ( units->units_alias_list )
	{
		list_free_container( units->units_alias_list );
	}

	if ( units->units_converted_list )
	{
		list_free_container( units->units_converted_list );
	}

	free( units );

} /* units_free() */

char *units_translate_units_name(
				LIST *units_alias_list,
				char *units_name,
				char *units_seek_phrase )
{
	UNITS *units;

	if ( !units_name || !units_seek_phrase ) return (char *)0;

	if ( timlib_strcmp( units_name, units_seek_phrase ) == 0 )
	{
		return units_name;
	}

	if ( ( units = units_seek(
				units_alias_list,
				units_seek_phrase ) ) )
	{
		return units->units_name;
	}

	return (char *)0;
}

UNITS_SPECIAL_CODE_STRUCTURE *units_special_code_structure_new(
					int special_code,
					char *replacement_string )
{
	UNITS_SPECIAL_CODE_STRUCTURE *s;

	if ( ! ( s = (UNITS_SPECIAL_CODE_STRUCTURE *)
			calloc( 1, sizeof( UNITS_SPECIAL_CODE_STRUCTURE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	s->special_code = special_code;
	s->replacement_string = replacement_string;

	return s;

} /* units_special_code_structure_new() */

/* Special codes include [mu] and [deg] */
/* ------------------------------------ */
char *units_translate_symbols( char *source )
{
	static LIST *special_code_list = {0};
	char destination[ 1024 ] = {0};
	char *ptr;
	boolean made_swap;
	UNITS_SPECIAL_CODE_STRUCTURE *s;

	ptr = destination;
	*ptr = '\0';

	if ( !special_code_list )
	{
		special_code_list = list_new();

		/* -75 unsigned (181 decimal/0xB5) */
		/* ------------------------------- */
		s = units_special_code_structure_new(
				-75,
				"[mu]" );
		list_append_pointer( special_code_list, s );

		/* -80 unsigned (176 decimal/0xB0) */
		/* ------------------------------- */
		s = units_special_code_structure_new(
				-80,
				"[deg]" );
		list_append_pointer( special_code_list, s );
	}

	while ( *source )
	{
		/* -62 unsigned (194 decimal/0xC2) preceeds the 8-bit symbols */
		/* ---------------------------------------------------------- */
		if ( *source == -62 )
		{
			source++;
			continue;
		}

		list_rewind( special_code_list );
		made_swap = 0;

		do {
			s = list_get_pointer( special_code_list );

			if ( *source == s->special_code )
			{
				char end[ 1024 ];

				strcpy( end, ptr + 1 );

				ptr += sprintf(ptr,
					 	"%s%s",
					 	s->replacement_string,
					 	end );

				made_swap = 1;
				break;
			}

		} while( list_next( special_code_list ) );

		if ( !made_swap )
		{
			*ptr++ = *source;
		}
		source++;
	}

	return strdup( destination );
}

double units_converted_to_multiply_by(
				char *units_converted_string,
				LIST *units_converted_list )
{
	UNITS_CONVERTED *units_converted;

	if ( ( units_converted =
			units_converted_to_seek(
				units_converted_string,
				units_converted_list ) ) )
	{
		if ( timlib_strcmp(	units_converted->units_converted,
					units_converted_string ) == 0 )
		{
			return units_converted->multiply_by;
		}
	}
	return 0.0;
}

double units_converted_from_multiply_by(
				char *units_converted_string,
				LIST *units_converted_list )
{
	UNITS_CONVERTED *units_converted;

	if ( ( units_converted =
			units_converted_from_seek(
				units_converted_string,
				units_converted_list ) ) )
	{
		if ( timlib_strcmp(	units_converted->units_name,
					units_converted_string ) == 0 )
		{
			return units_converted->multiply_by;
		}
	}
	return 0.0;
}

char *units_converted_list_display(
			LIST *units_converted_list )
{
	UNITS_CONVERTED *units_converted;
	char display[ 65536 ];
	char *ptr = display;

	if ( list_rewind( units_converted_list ) )
	{
		do {
			units_converted = list_get( units_converted_list );

			ptr += sprintf( ptr,
			"Units name: %s --> Units converted: %-30s %.5lf\n",
					units_converted->units_name,
					units_converted->units_converted,
					units_converted->multiply_by );

		} while ( list_next( units_converted_list ) );
	}

	*ptr = '\0';
	return strdup( display );
}

/* Returns heap memory */
/* ------------------- */
char *units_alias_list_display(
			LIST *units_alias_list )
{
	UNITS_ALIAS *units_alias;
	char display[ 65536 ];
	char *ptr = display;

	if ( list_rewind( units_alias_list ) )
	{
		do {
			units_alias = list_get_pointer( units_alias_list );

			ptr += sprintf( ptr,
				"Units alias: %s --> Units name: %s\n",
					units_alias->units_alias_name,
					units_alias->units_name );

		} while ( list_next( units_alias_list ) );
	}

	*ptr = '\0';
	return strdup( display );
}

