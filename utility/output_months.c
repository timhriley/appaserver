/* output_months.sh */
/* ---------------- */

#include <stdio.h>

char *months[] = { 	"JAN",
			"FEB",
			"MAR",
			"APR",
			"MAY",
			"JUN",
			"JUL",
			"AUG",
			"SEP",
			"OCT",
			"NOV",
			"DEC",
			"" };

main( int argc, char **argv )
{
	char **ptr = months;

	while( **ptr ) printf( "%s\n", *ptr++ );
	return 0;
}
