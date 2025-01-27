#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "parse_insert_statement.h"

main( int argc, char **argv )
{
	PARSE_INSERT_STATEMENT *p;

	p = new_parse_insert_statement( 
"insert into measurement ( station, datatype, measurement_date ) values ( 'BA', 'stage', '2000-10-01' );" );
}
