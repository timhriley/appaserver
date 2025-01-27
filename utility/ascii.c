/* utility/ascii.c */
/* --------------- */
#include <stdio.h>

struct
{
	char *mnemonic;
	int ascii;
} mnemonic_table[] = {	"backspace",	8,
			"bell",		7,
			"formfeed",	12,
			"linefeed",	13,
			"cr",		10,
			(char *)0,	0 };

main( argc, argv )
int argc;
char **argv;
{
	int ascii;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s [ascii][mnemonic]\n", argv[ 0 ] );
		display_mnemonic_table();
		exit( 1 );
	}

	ascii = atoi( argv[ 1 ] );

	if ( ascii == 0 ) ascii = get_from_table( argv[ 1 ] );

	if ( !ascii )
	{
		display_mnemonic_table();
		exit( 1 );
	}

	printf( "%c", ascii );
}


get_from_table( mnemonic )
char *mnemonic;
{
	int i;

	for( i = 0; mnemonic_table[ i ].mnemonic; i++ )
		if ( strcmp( mnemonic_table[ i ].mnemonic, mnemonic ) == 0 )
			return mnemonic_table[ i ].ascii;

	return 0;

} /* get_from_table() */


display_mnemonic_table()
{
	int i;
	
	fprintf( stderr, "\n" );
	fprintf( stderr, "-------------------\n" );
	fprintf( stderr, "Valid mnemonics are\n" );
	fprintf( stderr, "-------------------\n" );

	for( i = 0; mnemonic_table[ i ].mnemonic; i++ )
		fprintf( stderr, "%s\n", mnemonic_table[ i ].mnemonic );

} /* display_mnemonic_table() */
