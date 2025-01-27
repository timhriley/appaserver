/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/sql.h			   		*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#ifndef SQL_H
#define SQL_H

#define SQL_DELIMITER	'^'
/* #define SQL_DELIMITER	0x1e */

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *sql_delimiter_string(
			char sql_delimiter );

#endif
