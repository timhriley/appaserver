/* --------------------------------------------------- 	*/
/* post_sudoku.c				       	*/
/* --------------------------------------------------- 	*/
/* 							*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "appaserver.h"
#include "environ.h"
#include "dictionary.h"
#include "post2dictionary.h"

#define SUNSET_DOCUMENT_ROOT		"/var/www"
#define SUGAR_DOCUMENT_ROOT		"/home/timriley/public_html"

void output_solved_sudoku( DICTIONARY *post_dictionary );
void solve_sudoku( char *output_filename, DICTIONARY *post_dictionary );
void output_html_heading( void );

int main( void )
{
	DICTIONARY *post_dictionary;
	char sys_string[ 1024 ];
	char *uname_string;
	char *document_root;

	add_dot_to_path();
	add_utility_to_path();

	post_dictionary =
		post2dictionary(
				stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ );

	printf( "Content-type: text/html\n\n" );
	fflush( stdout );

	uname_string = pipe2string( "uname -n" );

	if ( strcmp( uname_string, "sugar.he.net" ) == 0 )
		document_root = SUGAR_DOCUMENT_ROOT;
	else
	if ( strcmp( uname_string, "sunset" ) == 0 )
		document_root = SUNSET_DOCUMENT_ROOT;
	else
		document_root = SUNSET_DOCUMENT_ROOT;

	sprintf(sys_string,
		"cat %s/appahost/menu.dat",
		document_root );
	if ( system( sys_string ) ){};
	fflush( stdout );

	sprintf(sys_string,
		"cat %s/appahost/sudoku.dat",
		document_root );
	if ( system( sys_string ) ){};
	fflush( stdout );

	output_solved_sudoku( post_dictionary );
	fflush( stdout );

	sprintf(sys_string,
		"cat %s/appahost/footer.dat",
		document_root );
	if ( system( sys_string ) ){};
	fflush( stdout );

	return 0;
} /* main() */

void output_solved_sudoku( DICTIONARY *post_dictionary )
{
	char filename[ 128 ];
	char input_line[ 128 ];
	FILE *input_file;
	int count = 0;
	char piece_buffer[ 16 ];
	char key[ 16 ];
	char *background_color;

	sprintf( filename, "/tmp/post_sudoku_%d", getpid() );
	solve_sudoku( filename, post_dictionary );

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s(): cannot open %s for read\n",
			 __FUNCTION__,
			 filename );
		exit( 1 );
	}

	output_html_heading();
	while( get_line( input_line, input_file ) )
	{
		if ( !(count++ % 9) ) printf( "<tr>\n" );

		/* --------------------------- */
		/* Sample key = "cell-3-0"     */
		/* Sample input_line = "3,0,5" */
		/* --------------------------- */
		sprintf( key, "cell-%s", input_line );
		*(key + 8) = '\0';
		search_replace_character( key, ',', '-' );
		
		piece( piece_buffer, ',', input_line, 2 );

		if ( dictionary_exists_key( post_dictionary, key ) )
			background_color = "bgcolor=cyan";
		else
			background_color = "";

		printf( "<td width=15 %s>%s\n",
			background_color,
			piece_buffer );
	}
	fclose( input_file );

	printf(
"<tr><td colspan=9>The source code is available <a href=%s>here.</a>\n",
"https://github.com/timhriley/appaserver/tree/master/src_sudoku" );

	printf( "</table>\n" );

	printf(
"<table align=center border=0 cellpadding=0 cellspacing=0><tr><td align=center><input type=\"button\" value=\"Back\" onclick=\"history.back()\">\n" );

	printf( "</table></div>\n" );

	printf( "</body></html>\n" );

} /* output_solved_sudoku() */

void output_html_heading( void )
{
	document_output_html_stream( stdout );

	printf(
"<head>\n"
"<title>Sudoku</title>\n"
"</head>\n"
"\n"
"<body>\n"
"<div class=body>\n"
"<table border=1\n"
"cellpadding=3\n"
"cellspacing=3\n"
"align=center\n"
"valign=top>\n" );
} /* output_html_heading() */

void solve_sudoku( char *output_filename, DICTIONARY *post_dictionary )
{
	LIST *key_list;
	char sys_string[ 1024 ];
	char *data;
	char *key;
	FILE *output_pipe;
	char output_string[ 16 ];

	sprintf( sys_string,
		 "%s/src_sudoku/sudoku_dvr y > %s",
		 appaserver_parameter_file_get_appaserver_mount_point(),
		 output_filename );
	output_pipe = popen( sys_string, "w" );

	key_list = dictionary_get_key_list( post_dictionary );
	
	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			data = dictionary_fetch( post_dictionary, key );

			/* Sample key = "cell-3-0" */
			/* ----------------------- */
			sprintf( output_string,
				 "%s,%s",
				 key + 5, data );
			search_replace_character( output_string, '-', ',' );

			fprintf( output_pipe, "%s\n", output_string );
		} while( list_next( key_list ) );
	}

	pclose( output_pipe );

} /* solve_sudoku() */

