/* $APPASERVER_HOME/utility/stat.c		       			   */
/* ----------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	   */
/* ----------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "timlib.h"
#include "boolean.h"
 
struct stat s;
void stat_execute( char *path, boolean is_ymd );
char *stat_owner( int uid );
char *stat_group( int gid );
char *stat_modified( time_t mtime, boolean is_ymd );
 
int main( int argc, char **argv )
{
        char instring[ 200 ];
	boolean is_ymd;
 
	is_ymd = ( strcmp( argv[ 0 ], "statymd.e" ) == 0 );

        if ( argc == 1 )
        {
                while( get_line( instring, stdin ) )
                        stat_execute( instring, is_ymd );
        }
        else
        {
                while( --argc )
                        stat_execute( *++argv, is_ymd );
        }
 
	return 0;
}
 
void stat_execute( char *path, boolean is_ymd )
{
        if ( stat( path, &s ) == -1 )
        {
                perror( path );
        }
        else
        {
                printf( ".filespec %s\n", path );
                printf( ".owner %s\n", stat_owner( s.st_uid ) );
                printf( ".group %s\n", stat_group( s.st_gid ) );
                printf( ".modified %s\n", stat_modified( s.st_mtime, is_ymd ) );
                printf( ".size %lu\n", s.st_size );
        }
}
 
char *stat_owner( int uid )
{
        struct passwd *pw = getpwuid( uid );
        return ( pw ) ? pw->pw_name : "Unknown";
}
 
char *stat_group( int gid )
{
        struct group *g = getgrgid( gid );
        return ( g ) ? g->gr_name : "Unknown";
}
 
char *stat_modified( time_t mtime, boolean is_ymd )
{
        static char ret_time[ 31 ];
        struct tm *tm = localtime( &mtime );
 
	if ( is_ymd )
        	strftime( ret_time, 30, "%F %H:%M", tm );
	else
        	strftime( ret_time, 30, "%d-%b-%Y %H:%M", tm );

        return ret_time;
}
