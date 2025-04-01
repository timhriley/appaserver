/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/sql.h			   		*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#ifndef SQL_H
#define SQL_H

#include "boolean.h"
#include "list.h"

#define SQL_DELIMITER	'^'
#define SQL_EXECUTABLE	"sql.e"

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *sql_delimiter_string(
		char delimiter );

/* Returns error_string or null */
/* ---------------------------- */
char *sql_execute(
		char *appaserver_error_filename,
		LIST *sql_list /* mutually exclusive */,
		char *sql_statement /* mutually exclusive */ );

#endif
