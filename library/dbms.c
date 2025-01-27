/* library/dbms.c */
/* -------------- */

#include <stdio.h>
#include <string.h>

static char sql_stmt[ 2048 ];

dbms_init()
{
	*sql_stmt = '\0';
	return 0;
}

dbms_append( char *s )
{
	strcat( sql_stmt, s );
	return 0;
}

dbms_execute()
{
	dm_dbms( sql_stmt );
	return 0;
}

dbms_send_to_dbms_out()
{
	FILE *f = fopen( "dbms.out", "w" );

	if ( !f )
	{
		sm_err_reset( "Cannot open dbms.out" );
	}
	else
	{
		fprintf( f, "%s\n", sql_stmt );
		fclose( f );
	}

	return 0;
}
