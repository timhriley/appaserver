/* utility/piece_set_character.c */
/* ----------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"

void piece_set_character( int argc, char **argv );
void do_work_on_buffer( char *piece_char_position_where_from, char *buffer );
int exists_3_commas( char *buffer );
void do_work_on_buffer_left(	char *field_contents, 
				char character, 
				int position );
void do_work_on_buffer_right(	char *field_contents, 
				char character, 
				int position );

int main( int argc, char **argv )
{
	if ( argc == 1 )
	{
		fprintf( stderr, 
"Usage: piece_set_character.e piece_offset,char,position,[left][right] ...\n" );
		exit( 1 );
	}

	piece_set_character( argc, argv );
	return 0;

} /* main() */



void piece_set_character( int argc, char **argv )
{
	char buffer[ 1024 ];
	int local_argc;
	char **local_argv;

	while( get_line( buffer, stdin ) )
	{
		local_argc = argc;
		local_argv = argv;

		while( --local_argc )
			do_work_on_buffer( *++local_argv, buffer );

		printf( "%s\n", buffer );
	}

} /* piece_set_character() */



void do_work_on_buffer( char *piece_char_position_where_from, char *buffer )
{
	char tmp[ 128 ];
	char field_contents[ 128 ];
	int piece_offset;
	char character;
	int position;
	char left_or_right[ 50 ];

	if ( !exists_3_commas( piece_char_position_where_from ) )
	{
		fprintf( stderr, 
		"piece_set_character.e not enough commas in parameter.\n" );
		exit( 1 );
	}

	/* Parse fields from argument */
	/* -------------------------- */
	piece_offset = atoi( piece( tmp, ',', 
				    piece_char_position_where_from, 0 ) );
	character = *piece( tmp, ',', 
				    piece_char_position_where_from, 1 );

	/* The character to set might be NULL */
	/* ---------------------------------- */
	if ( strcmp( tmp, "null" ) == 0 ) character = '\0';

	position = atoi( piece( tmp, ',', 
				    piece_char_position_where_from, 2 ) );
	piece( left_or_right, ',', 
				    piece_char_position_where_from, 3 );

	/* Make the change */
	/* --------------- */
	if ( !piece( field_contents, '|', buffer, piece_offset ) )
	{
		fprintf( stderr, 
		"piece_set_character.e: not enought fields to get: %s\n(%s)\n",
			 piece_char_position_where_from,
			 buffer );
		exit( 1 );
	}

	if ( strcmp( left_or_right, "left" ) == 0 )
		do_work_on_buffer_left( field_contents, character, position );
	else
	if ( strcmp( left_or_right, "right" ) == 0 )
		do_work_on_buffer_right( field_contents, character, position );
	else
	{
		fprintf( stderr, 
			 "Did not get left or right in: (%s)\n",
			 piece_char_position_where_from );
		exit( 1 );
	}

	/* Put the change back into the record */
	/* ----------------------------------- */
	replace_piece( buffer, '|', field_contents, piece_offset );

} /* do_work_on_buffer() */


void do_work_on_buffer_left(	char *field_contents, 
				char character, 
				int position )
{
	char tmp[ 1024 ];
	char *tmp_ptr = tmp;
	char *field_contents_ptr = field_contents;

	/* If the cannot do then do nothing */
	/* -------------------------------- */
	if ( position > (int)strlen( field_contents ) )
	{
		return;
	}

	/* Copy first part */
	/* --------------- */
	while( position )
	{
		*tmp_ptr++ = *field_contents_ptr++;
		position--;
	}

	/* Place in character */
	/* ------------------ */
	*tmp_ptr++ = character;

	/* Copy the rest */
	/* ------------- */
	while( *field_contents_ptr )
		*tmp_ptr++ = *field_contents_ptr++;

	*tmp_ptr = '\0';

	/* Bring back to original buffer */
	/* ----------------------------- */
	strcpy( field_contents, tmp );

} /* do_work_on_buffer_left() */


void do_work_on_buffer_right(	char *field_contents, 
				char character, 
				int position )
{
	char tmp[ 1024 ];
	char *tmp_ptr;
	char *field_contents_ptr;
	int field_contents_len;

	field_contents_len = strlen( field_contents );

	/* If the cannot do then do nothing */
	/* -------------------------------- */
	if ( position > field_contents_len )
	{
		return;
	}

	tmp_ptr = tmp + field_contents_len + 1;
	field_contents_ptr = field_contents + field_contents_len;

	/* Copy over the null */
	/* ------------------ */
	*tmp_ptr-- = *field_contents_ptr--;

	/* Copy first part */
	/* --------------- */
	while( position )
	{
		*tmp_ptr-- = *field_contents_ptr--;
		position--;
	}

	/* Place in character */
	/* ------------------ */
	*tmp_ptr-- = character;

	/* Copy the rest */
	/* ------------- */
	while( field_contents_ptr != field_contents - 1 )
		*tmp_ptr-- = *field_contents_ptr--;

	/* Bring back to original buffer */
	/* ----------------------------- */
	strcpy( field_contents, tmp );

} /* do_work_on_buffer_right() */


int exists_3_commas( char *buffer )
{
	int count = 0;

	while( *buffer )
	{
		if ( *buffer == ',' ) count++;
		buffer++;
	}
	return ( count == 3 );
}

