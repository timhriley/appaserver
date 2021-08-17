/* $APPASERVER_HOME/library/frameset_link_file.c	*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "timlib.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "security.h"
#include "environ.h"
#include "frameset_link_file.h"

FRAMESET_LINK_FILE *frameset_link_file_new(
			char *document_root_directory )
{
	FRAMESET_LINK_FILE *frameset_link_file;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( ! ( frameset_link_file =
			calloc( 1, sizeof( FRAMESET_LINK_FILE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	frameset_link_file->document_root_directory =
		document_root_directory;

	if ( ! ( frameset =
			frameset_new(
				application_nam,
				login_name,
				session ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: frameset_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return frameset_link_file;
}

