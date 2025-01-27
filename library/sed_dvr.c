#include <string.h>
#include <stdio.h>
#include "timlib.h"
#include "sed.h"

void test1( void );
void test2( void );
void test3( void );

int main( void )
{
	/* test1(); */
	/* test2(); */
	test3();
	return 0;
}

void test1( void )
{
	char *search  = "\\$_SESSION\\['\\$dbuser'\\]";
	char *replace = "$_SESSION['dbuser']";
	char buffer[ 512 ];
	SED *sed;

	strcpy( buffer,
		"a this is a test $_SESSION['$dbuser'] for this string" );

	sed = new_sed( search, replace );
	if ( sed_will_replace( buffer, sed ) )
	{
		printf( "before: %s\n", buffer );
		sed_search_replace( buffer, sed );
		printf( "after:  %s\n", buffer );
	}
}

void test2( void )
{
	char *regular_expression  =
		"[12][0-9][0-9][0-9]-[01][0-9]-[0123][0-9]";

	char *replace = "01-JUN-1999";
	char buffer[ 512 ];
	SED *sed;

	strcpy( buffer,
		"this is a test (1999-06-01) for this string" );

	sed = new_sed( regular_expression, (char *)0 );
	if ( sed_will_replace( buffer, sed ) )
	{
		sed->replace = replace;
		printf( "before: %s\n", buffer );
		sed_search_replace( buffer, sed );
		printf( "after:  %s\n", buffer );
	}
}

void test3( void )
{
	char sans_bank_date_description[ 512 ];
	char *replace;
	char *regular_expression;
	SED *sed;

	regular_expression = "[ ][0-9][1-9]/[0-9][1-9]$";
	replace = "";

	strcpy( sans_bank_date_description,
"Amazon.com AMZN.COM/BILL WA                  01/14$" );

	sed = sed_new( regular_expression, (char *)0 /* replace */ );

fprintf( stderr, "%s/%s()/%d: before: [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
sans_bank_date_description );

	if ( sed_will_replace( sans_bank_date_description, sed ) )
	{
		sed->replace = replace;

fprintf( stderr, "%s/%s()/%d: will replace!\n",
__FILE__,
__FUNCTION__,
__LINE__ );
		sed_search_replace( sans_bank_date_description, sed );

fprintf( stderr, "%s/%s()/%d: after:  [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
sans_bank_date_description );

	}

	sed_free( sed );
	printf( "Will return = [%s]\n",
		timlib_rtrim( sans_bank_date_description ) );
}
