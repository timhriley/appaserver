/* $APPASERVER_HOME/utility/location_convert.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"

#define UTM_NAD83		"UTM_NAD83"
#define WGS84_D_DECIMAL		"WGS84_D.decimal"
#define WGS84_D_M_DECIMAL	"WGS84_D_M.decimal"

int get_word_count(		char *filename );

char *get_filename(		char *key );

void location_convert(		char *source_format,
				char *destination_format,
				int eastwest_piece,
				int northsouth_piece,
				char delimiter );

int main( int argc, char **argv )
{
	char *source_format;
	char *destination_format;
	int eastwest_piece;
	int northsouth_piece;
	char delimiter;

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s source_format destination_format eastwest_piece northsouth_piece delimiter\n",
			 argv[ 0 ] );

		fprintf( stderr,
			 "Expecting format: %s, %s, or %s\n",
			 UTM_NAD83,
			 WGS84_D_DECIMAL,
			 WGS84_D_M_DECIMAL );
		return 1;
	}

	source_format = argv[ 1 ];
	destination_format = argv[ 2 ];
	eastwest_piece = atoi( argv[ 3 ] );
	northsouth_piece = atoi( argv[ 4 ] );
	delimiter = *argv[ 5 ];

	location_convert(	source_format,
				destination_format,
				eastwest_piece,
				northsouth_piece,
				delimiter );

	return 0;

}

char *get_filename( char *key )
{
	static char filename[ 128 ];

	sprintf( filename, "/tmp/location_convert_%s_%d", key, getpid() );
	return filename;
}

int get_word_count( char *filename )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string, "wc -l %s", filename );
	results = pipe2string( sys_string );
	return atoi( results );
}

void location_convert(		char *source_format,
				char *destination_format,
				int eastwest_piece,
				int northsouth_piece,
				char delimiter )
{
	char *cs2cs_executable;
	char spool_input_buffer[ 1024 ];
	char convert_input_buffer[ 1024 ];
	char *spool_filename;
	char *convert_filename;
	char sys_string[ 1024 ];
	FILE *spool_input_file;
	FILE *convert_input_file;
	int word_count_spool_file;
	int word_count_convert_file;
	int space_piece;
	char piece_buffer[ 128 ];

	if ( strcasecmp( source_format, UTM_NAD83 ) == 0
	&&   strcasecmp( destination_format, WGS84_D_M_DECIMAL ) == 0 )
	{
		cs2cs_executable = "cs2cs_utm.sh minutes";
		space_piece = 2;
	}
	else
	if ( strcasecmp( source_format, UTM_NAD83 ) == 0
	&&   strcasecmp( destination_format, WGS84_D_DECIMAL ) == 0 )
	{
		cs2cs_executable = "cs2cs_utm.sh";
		space_piece = 1;
	}
	else
	if ( strcasecmp( source_format, WGS84_D_DECIMAL ) == 0
	&&   strcasecmp( destination_format, UTM_NAD83 ) == 0 )
	{
		cs2cs_executable = "cs2cs_lonlat.sh";
		space_piece = 1;
	}
	else
	if ( strcasecmp( source_format, WGS84_D_M_DECIMAL ) == 0
	&&   strcasecmp( destination_format, UTM_NAD83 ) == 0 )
	{
		cs2cs_executable = "cs2cs_lonlat.sh minutes";
		space_piece = 1;
	}
	else
	{
		fprintf(stderr,
	"Error in %s/%s()/%d: invalid source/destination format = (%s/%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			source_format,
			destination_format );
		exit( 1 );
	}

	spool_filename = strdup( get_filename( "spool" ) );
	convert_filename = strdup( get_filename( "convert" ) );

	sprintf( sys_string,
		 "cat -			|"
		 "tee %s		|"
		 "piece.e '%c' %d,%d	|"
		 "tr '%c' ' '		|"
		 "%s			|"
		 "sed 's/ /%c/%d'	|"
		 "cat > %s		 ",
		 spool_filename,
		 delimiter,
		 eastwest_piece,
		 northsouth_piece,
		 delimiter,
		 cs2cs_executable,
		 delimiter,
		 space_piece,
		 convert_filename );

	if ( system( sys_string ) ){}

	word_count_spool_file =
		get_word_count( spool_filename );

	word_count_convert_file =
		get_word_count( convert_filename );

	if ( word_count_spool_file != word_count_convert_file )
	{
		fprintf( stderr,
"Error %s/%s()/%d: did not convert all locations. Expecting %d records, but got %d instead.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 word_count_spool_file,
			 word_count_convert_file );
		exit( 1 );
	}

	if ( ! ( spool_input_file = fopen( spool_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 spool_filename );
		exit( 1 );
	}

	if ( ! ( convert_input_file = fopen( convert_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 convert_filename );
		exit( 1 );
	}

	while( get_line( spool_input_buffer, spool_input_file ) )
	{
		get_line( convert_input_buffer, convert_input_file );

		/* Replace eastwest */
		/* ---------------- */
		piece(	piece_buffer,
			delimiter,
			convert_input_buffer,
			0 );

		piece_replace(
			spool_input_buffer,
			delimiter, 
			piece_buffer,
			eastwest_piece );

		/* Replace northsouth */
		/* ------------------ */
		piece(	piece_buffer,
			delimiter,
			convert_input_buffer,
			1 );

		piece_replace(
			spool_input_buffer,
			delimiter, 
			piece_buffer,
			northsouth_piece );

		printf( "%s\n", spool_input_buffer );
	}

	fclose( spool_input_file );
	fclose( convert_input_file );

	sprintf( sys_string, "rm -f %s", spool_filename );
	if ( system( sys_string ) ){}

	sprintf( sys_string, "rm -f %s", convert_filename );
	if ( system( sys_string ) ){}
}

