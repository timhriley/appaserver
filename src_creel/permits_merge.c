/* permits_merge.c */
/* --------------- */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "hash_table.h"
#include "String.h"
#include "sql.h"
#include "creel.h"

int main()
{
	char input[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char system_string[ 1024 ];
	char *select;
	char *primary_key;
	char fishing_purpose[ 128 ];
	char census_date[ 128 ];
	char interview_location[ 128 ];
	char interview_number[ 128 ];
	char permit_code[ 128 ];
	CREEL_PERMITS *permits;
	HASH_TABLE *hash_table;
	char *table;
	char *where;

	hash_table =
		creel_permits_hash_table_fetch(
			"guide_angler_name is not null" );

	select =	"fishing_purpose,"	\
			"census_date,"		\
			"interview_location,"	\
			"interview_number,"	\
			"permit_code";

	table = "fishing_trips";

	where = "permit_code is not null";

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" census_date",
		select,
		table,
		where );

	input_pipe = popen( system_string, "r" );

	primary_key =	"fishing_purpose,"	\
			"census_date,"		\
			"interview_location,"	\
			"interview_number";

	sprintf(system_string,
		"update_statement table=%s key=\"%s\" carrot=y",
		table,
		primary_key );

	output_pipe = popen( system_string, "w" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		piece( fishing_purpose, SQL_DELIMITER, input, 0 );
		piece( census_date, SQL_DELIMITER, input, 1 );
		piece( interview_location, SQL_DELIMITER, input, 2 );
		piece( interview_number, SQL_DELIMITER, input, 3 );
		piece( permit_code, SQL_DELIMITER, input, 4 );

		if ( ( permits =
			creel_permits_hash_table_seek(
				permit_code,
				hash_table ) ) )
		{
			fprintf(output_pipe,
				"%s^%s^%s^%s^permit_code^%s\n",
				fishing_purpose,
				census_date,
				interview_location,
				interview_number,
				permits->permit_code );
		}
	}

	pclose( input_pipe );
	pclose( output_pipe );

	return 0;
}
