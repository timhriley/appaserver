/* $APPASERVER_HOME/src_appaserver/output_results.c			*/
/* --------------------------------------------------------------------	*/
/*									*/
/* This is the form that displays the number of rows inserted.		*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "application.h"
#include "appaserver_library.h"
#include "environ.h"
#include "appaserver_parameter_file.h"
#include "dictionary.h"
#include "document.h"
#include "appaserver_error.h"

int main( int argc, char **argv )
{
	char *application_name, *folder;
	char buffer[ 1024 ];
	char trim_buffer[ 128 ];
	int inserted;
	char message[ 1024 ];
	char *vertical_new_button_many_folder_name;
	char onload_function[ 65536 ];
	char *session;
	char *login_name;
	char *role_name;
	char *content_type_yn;
	char *dictionary_string;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 10 )
	{
		fprintf( stderr, 
"Usage: %s ignored folder session login_name role inserted message vertical_new_button_many_folder_name content_type_yn [dictionary]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	folder = argv[ 2 ];
	session = argv[ 3 ];
	login_name = argv[ 4 ];
	role_name = argv[ 5 ];
	inserted = atoi( argv[ 6 ] );

	strcpy( message, argv[ 7 ] );
	search_replace_string(	message,
				"\\",
				"" );

	vertical_new_button_many_folder_name = argv[ 8 ];
	content_type_yn = argv[ 9 ];

	if ( argc == 11 )
		dictionary_string = argv[ 10 ];
	else
		dictionary_string = "";

	vertical_new_button_many_folder_name =
		trim_quotes(	trim_buffer,
				vertical_new_button_many_folder_name );

	if ( *vertical_new_button_many_folder_name )
	{
		char replace_string[ 2 ];

		sprintf( replace_string,
			 "%c",
			 DICTIONARY_ALTERNATIVE_DELIMITER );

		search_replace_string(	dictionary_string,
					"=",
					replace_string );

		sprintf(onload_function,
"onload=\"window.open('%s/output_prompt_insert_form?%s+%s+%s+%s+%s+insert+n+%s', '%s', 'menubar=yes,resizable=yes,scrollbars=yes,status=yes,toolbar=yes,location=yes', 'false' );\"",
			appaserver_parameter_file_get_cgi_directory(),
			login_name,
			application_name,
			session,
			vertical_new_button_many_folder_name,
			role_name,
			dictionary_string,
			PROMPT_FRAME );
	}
	else
	{
		*onload_function = '\0';
	}

	if ( *content_type_yn == 'y' )
		printf( "Content-type: text/html\n\n" );

	document_output_html_stream( stdout );

	printf(
"	<head>							\n"
"		<link rel=stylesheet type=\"text/css\"		\n"
"			href=/appaserver/%s/style.css>		\n"
"		<TITLE>Insert %s results</TITLE>		\n"
"	</head>							\n",
		application_name, folder );

	printf(
"	<body bgcolor=%s %s>					\n"
"	<h1>Insert %s results</h1>				\n",
		application_background_color( application_name ),
		onload_function,
		format_initial_capital( buffer, folder ) );

	fflush( stdout );
	/* system( "echo '<h1>' && date.e 0 && echo '</h1>'" ); */
	if ( system( timlib_system_date_string() ) ){};
	fflush( stdout );

	if ( inserted > -1 )
	{
		printf( "<p>Rows inserted: %d<br>\n", inserted );
		if ( *message ) printf( "%s\n", message );
	}

	printf(
"	</body>							\n"
"	</html>							\n" );

	return 0;

} /* main() */

