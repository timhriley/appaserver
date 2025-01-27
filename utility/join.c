/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/join.c		 			*/
/* -------------------------------------------------------------	*/
/* This is a clone of /usr/bin/join		 			*/ 
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "dictionary.h"

void join(	char *argv_0,
		char *file1_name,
		char *file2_name,
		char delimiter,
		char duplicate_delimiter );

DICTIONARY *get_dictionary(
		char *argv_0,
		char *file2_name,
		char delimiter,
		char duplicate_delimiter );

DICTIONARY *get_dictionary(	char *argv_0,
				char *file2_name,
				char delimiter,
				char duplicate_delimiter )
{
	FILE *file2;
	char input_buffer[ 4096 ];
	char key[ 1024 ];
	char data[ 4096 ];
	char buffer[ 8192 ];
	DICTIONARY *dictionary;
	char *existing_data;

	dictionary = dictionary_medium();

	file2 = open_file( file2_name, "r" );

	while( get_line( input_buffer, file2 ) )
	{
		piece( key, delimiter, input_buffer, 0 );
		if ( !piece( data, delimiter, input_buffer, 1 ) )
		{
			fprintf( stderr,
				 "Warning in %s: no delimiter '%c' in (%s)\n",
				 argv_0,
				 delimiter,
				 input_buffer );
			continue;
		}

		if ( ( existing_data = dictionary_get_pointer(
						dictionary, key ) ) )
		{
			sprintf(	buffer,
					"%s%c%s",
					existing_data,
					duplicate_delimiter,
					data );
			free( existing_data );
			strcpy( data, buffer );
		}

		dictionary_set_pointer(	dictionary,
					strdup( key ),
					strdup( data ) );
	}

	fclose( file2 );
	return dictionary;
}

void join(	char *argv_0,
		char *file1_name,
		char *file2_name,
		char delimiter,
		char duplicate_delimiter )
{
	FILE *file1;
	char input_buffer[ 4096 ];
	char key[ 1024 ];
	char left_data[ 3072 ];
	char *right_data;
	DICTIONARY *right_side_dictionary;

	right_side_dictionary =
		get_dictionary(
			argv_0,
			file2_name,
			delimiter,
			duplicate_delimiter );

	file1 = open_file( file1_name, "r" );

	while( get_line( input_buffer, file1 ) )
	{
		if ( character_exists( input_buffer, delimiter ) )
		{
			piece( key, delimiter, input_buffer, 0 );
			piece( left_data, delimiter, input_buffer, 1 );
		}
		else
		{
			strcpy( key, input_buffer );
		}

		if ( ( right_data =
			dictionary_get_pointer(
				right_side_dictionary, key ) ) )
		{
			printf(	"%s%c%s\n",
				input_buffer,
				delimiter,
				right_data );
		}
		else
		{
			printf(	"%s%c\n",
				input_buffer,
				delimiter );
		}

	}
	fclose( file1 );
}

int main( int argc, char **argv )
{
	char duplicate_delimiter;

	if ( argc < 4 )
	{
		fprintf( stderr,
		"Usage: %s file1 file2 delimiter [duplicate_delimiter]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	if ( argc == 5 )
		duplicate_delimiter = *argv[ 4 ];
	else
		duplicate_delimiter = ',';

	join(	argv[ 0 ],
		argv[ 1 ],
		argv[ 2 ],
		*argv[ 3 ],
		duplicate_delimiter );

	return 0;
}

