/* $APPASERVER_HOME/utility/covariance.c		   */
/* ======================================================= */
/* Freely available software: see Appaserver.org	   */
/* ======================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"
#include "list.h"
#include "appaserver_error.h"
#include "covariance.h"

void covariance(	char *anchor_datatype_group,
			int date_time_group_piece,
			int datatype_group_piece,
			int data_piece,
			char piece_character );

int main( int argc, char **argv )
{
	char *anchor_datatype_group;
	int date_time_group_piece;
	int datatype_group_piece;
	int data_piece;
	char piece_character;

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s anchor_datatype_group date_time_group_piece datatype_group_piece data_piece piece_character\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	appaserver_error_stderr( argc, argv );

	anchor_datatype_group = argv[ 1 ];
	date_time_group_piece = atoi( argv[ 2 ] );
	datatype_group_piece = atoi( argv[ 3 ] );
	data_piece = atoi( argv[ 4 ] );
	piece_character = *argv[ 5 ];

	covariance(	anchor_datatype_group,
			date_time_group_piece,
			datatype_group_piece,
			data_piece,
			piece_character );

	return 0;
}

void covariance(	char *anchor_datatype_group,
			int date_time_group_piece,
			int datatype_group_piece,
			int data_piece,
			char piece_character )
{
	COVARIANCE *covariance;
	COVARIANCE_DATATYPE *covariance_datatype;
	char input_buffer[ 1024 ];
	char datatype_group_buffer[ 512 ];
	char value_buffer[ 128 ];
	char date_time_group_buffer[ 128 ] = {0};
	double covariance_value;
	int count;
	double average;
	double standard_deviation;
	double correlation;

	covariance = covariance_new_covariance( anchor_datatype_group );

	while( get_line( input_buffer, stdin ) )
	{
/*
fprintf( stderr, "%s/%s(): got input_buffer = (%s)\n",
__FILE__,
__FUNCTION__,
input_buffer );
*/

		if ( !piece(	datatype_group_buffer,
				piece_character,
				input_buffer,
				datatype_group_piece ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s(): cannot piece datatype (%c) %d in (%s)\n",
				__FILE__,
				__FUNCTION__,
				piece_character,
				datatype_group_piece,
				input_buffer );
			continue;
		}

		if ( !piece(	date_time_group_buffer,
				piece_character,
				input_buffer,
				date_time_group_piece ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s(): cannot piece key (%c) %d in (%s)\n",
				__FILE__,
				__FUNCTION__,
				piece_character,
				date_time_group_piece,
				input_buffer );
			continue;
		}

		if ( !piece(	value_buffer,
				piece_character,
				input_buffer,
				data_piece ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s(): cannot piece value (%c) %d in (%s)\n",
				__FILE__,
				__FUNCTION__,
				piece_character,
				data_piece,
				input_buffer );
			continue;
		}

		if ( *value_buffer 
		&&   strcmp( value_buffer, "null" ) != 0 )
		{
			covariance_set_double(
				covariance->anchor_datatype_group,
				covariance->compare_covariance_datatype_list,
				strdup( datatype_group_buffer ),
				strdup( date_time_group_buffer ),
				atof( value_buffer ) );
		}
	}

	if ( !list_rewind( covariance->compare_covariance_datatype_list ) )
		return;

	do {
		covariance_datatype =
			list_get_pointer(
				covariance->
					compare_covariance_datatype_list );

		if ( covariance_get_results(	
				&covariance_value,
				&count,
				&average,
				&standard_deviation,
				&correlation,
				covariance->anchor_datatype_group,
				covariance->compare_covariance_datatype_list,
				covariance_datatype->datatype_name ) )
		{
			printf( "%s:%.5lf:%d:%.5lf:%.5lf:%.5lf\n",
				covariance_datatype->datatype_name,
				covariance_value,
				count,
				average,
				standard_deviation,
				correlation );
		}
		else
		{
			printf(
		"%s:No matching date/time combinations:0:0.0:0.0:0.0\n",
				covariance_datatype->datatype_name );
		}
	} while( list_next( covariance->compare_covariance_datatype_list ) );
}

