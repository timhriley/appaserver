/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/date_convert.h	 			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef DATE_CONVERT_H
#define DATE_CONVERT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boolean.h"

#define CURRENT_CENTURY		2000

enum date_convert_format_enum {	date_convert_international,
				date_convert_american,
				date_convert_military,
				date_convert_unknown,
				date_convert_blank };

typedef struct
{
	enum date_convert_format_enum source_enum;
	enum date_convert_format_enum destination_enum;
	char *source_date_string;
	char date_column_string[ 20 ];
	char time_column_string[ 20 ];
	char *return_date_string;
} DATE_CONVERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DATE_CONVERT *date_convert_new(
		enum date_convert_format_enum source_enum,
		enum date_convert_format_enum destination_enum,
		char *source_date_string );

/* Process */
/* ------- */
DATE_CONVERT *date_convert_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory, source_date_string or null */
/* ----------------------------------------------- */
char *date_convert_return_date_string(
		enum date_convert_format_enum source_enum,
		enum date_convert_format_enum destination_enum,
		char *source_date_string );

/* Usage */
/* ----- */

/* Returns heap memory, source_date_string or null */
/* ----------------------------------------------- */
char *date_convert_source_american(
		enum date_convert_format_enum destination_enum,
		char *source_date_string );

/* Usage */
/* ----- */

/* Returns heap memory, source_date_string or null */
/* ----------------------------------------------- */
char *date_convert_source_international(
		enum date_convert_format_enum destination_enum,
		char *source_date_string );

/* Usage */
/* ----- */

/* Returns heap memory, source_date_string or null */
/* ----------------------------------------------- */
char *date_convert_source_military(
		enum date_convert_format_enum destination_enum,
		char *source_date_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *date_convert_international_to_military(
		char *source_date_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *date_convert_american_to_international(
		char *source_date_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *date_convert_military_to_international(
		char *source_date_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *date_convert_international_to_american(
		char *source_date_string );

/* Usage */
/* ----- */
enum date_convert_format_enum date_convert_string_evaluate(
		char *source_date_string );

/* Process */
/* ------- */
boolean date_convert_is_valid_military(
		char *date_string );

boolean date_convert_is_valid_international(
		char *date_string );

boolean date_convert_is_valid_american(
		char *date_string );

/* Usage */
/* ----- */

/* Returns static enum date_convert_format_enum */
/* -------------------------------------------- */
enum date_convert_format_enum date_convert_login_name_enum(
		char *application_name,
		char *login_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DATE_CONVERT *date_convert_destination_international(
		char *application_name,
		char *login_name,
		char *date_string );

/* Public */
/* ------ */
enum date_convert_format_enum date_convert_format_string_evaluate(
		char *date_convert_format_string );

boolean date_convert_is_valid_integers(
		int year_integer,
		int month_integer,
		int day_integer );

/* Returns static memory */
/* --------------------- */
char *date_convert_american_sans_slashes(
		char *compressed_date_string );

/* Returns static memory */
/* --------------------- */
char *date_convert_select_format_string(
		enum date_convert_format_enum date_convert_format_enum );

/* Returns program memory */
/* ---------------------- */
char *date_convert_format_string(
		enum date_convert_format_enum date_convert_format_enum );

void date_convert_free(
		DATE_CONVERT *date_convert );

#endif
