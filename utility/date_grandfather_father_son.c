/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/date_grandfather_father_son.c		*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "date.h"

void date_grandfather_father_son( DATE *d );

int main( int argc, char **argv )
{
	DATE *d;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s date_yyyymmdd\n", argv[ 0 ] );
		exit( 1 );
	}

	d = date_yyyymmdd_new( argv[ 1 ] );

	date_grandfather_father_son( d );

	return 0;
}

void date_grandfather_father_son( DATE *d )
{
	printf( "son\n" );

	if ( date_is_father( d ) )
		printf( "father\n" );

	if ( date_is_grandfather( d ) )
		printf( "grandfather\n" );

	if ( date_is_greatgrandfather( d ) )
		printf( "greatgrandfather\n" );

	if ( date_is_greatgreatgrandfather( d ) )
		printf( "greatgreatgrandfather\n" );

} /* date_grandfather_father_son() */

