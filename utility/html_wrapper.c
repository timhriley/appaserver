/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/html_wrapper.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "javascript.h"
#include "document.h"

int main( void )
{
	char buffer[ 4096 ];

	printf( "%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		document_html(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_type_html(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_standard_string() ) );

	printf( "%s\n%s\n%s\n%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_open_tag(),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_meta_string(),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_javascript_string(),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_stylesheet_string() );

	printf( "%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_close_tag() );

	printf( "%s\n",
		document_body_onload_open_tag(
			JAVASCRIPT_WAIT_OVER,
			(char *)0 /* onload_javascript_string */,
			(char *)0 /* menu_onload_string */ ) );

	while( string_input( buffer, stdin, sizeof ( buffer ) ) )
		puts( buffer );

	/* Outputs both </body> and </html> */
	/* -------------------------------- */
	document_close();

	return 0;
}
