/* $APPASERVER_HOME/src_appaserver/output_close_html_document.c		*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	DOCUMENT *document;

	output_starting_argv_stderr( argc, argv );

	document = document_new( "", "" );

	document_close();
	return 0;
}

