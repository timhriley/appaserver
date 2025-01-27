#include <stdio.h>
#include <stdlib.h>

int main( void )
{
	char buffer[ 4096 ];

	sprintf( buffer, "grep \"select \"| sql.e" );
	if ( system( buffer ) ){}

	return 0;
}
