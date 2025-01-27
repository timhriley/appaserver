/* /usr2/lib/function_dvr.c */
/* ------------------------ */

#include "function.h"

int function_age( char *birth_date, char *line );

main()
{
	char functions_replaced[ 128 ];

	FUNCTION_ADT *f = function_init();
	set_function( f, "age", function_age );
	replace_all_functions(	functions_replaced, 
				":age(10/06/60)",
				f );

	printf( "Got: %s\n", functions_replaced );
	function_free( f );
}

int function_age( char *age_str, char *line )
{
	char birth_date[ 128 ];

	get_parameter(	birth_date, line, 0 );
	sprintf( age_str, "%d", age( birth_date ) );
	return 1;
}
