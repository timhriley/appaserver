/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/sql.h			   		*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#pragma once

#include "boolean.h"
#include "list.h"

#define SQL_DELIMITER	'^'
#define SQL_EXECUTABLE	"sql.e"

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *sql_delimiter_string(
		char delimiter );

/* Usage */
/* ----- */

/* Returns error_string or null */
/* ---------------------------- */
char *sql_execute(
		const char *sql_executable,
		char *appaserver_error_filename,
		LIST *sql_list /* mutually exclusive */,
		char *sql_statement /* mutually exclusive */ );

