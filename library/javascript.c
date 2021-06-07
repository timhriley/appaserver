/* $APPASERVER_HOME/library/javascript.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"
#include "javascript.h"

void javascript_replace_state(
			char *post_change_javascript,
			char *state )
{
	search_replace_string(
		post_change_javascript,
		"$state",
		state );
}

void javascript_replace_row(
			char *post_change_javascript,
			char *row )
{
	search_replace_string(
		post_change_javascript,
		"$row",
		row );
}

