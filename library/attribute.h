/* $APPASERVER_HOME/library/attribute.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

/* Includes */
/* -------- */
#include "list.h"
#include "boolean.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define ATTRIBUTE_TABLE			"attribute"

#define ATTRIBUTE_SELECT		"attribute,"			\
					"attribute_datatype,"		\
					"width,"			\
					"float_decimal_places,"		\
					"hint_message,"			\
					"post_change_javascript,"	\
					"on_focus_javascript_function,"	\
					"lookup_histogram_output_yn,"	\
					"lookup_timechart_output_yn"

/* Structures */
/* ---------- */

typedef struct
{
	/* Input */
	/* ----- */
	char *attribute_name;
	char *datatype_name;
	int width;
	int float_decimal_places;
	char *hint_message;
	char *post_change_javascript;
	char *on_focus_javascript_function;
	boolean lookup_histogram_output;
	boolean lookup_time_chart_output;
} ATTRIBUTE;

/* ATTRIBUTE operations */
/* -------------------- */
ATTRIBUTE *attribute_fetch(
			char *attribute_name );

ATTRIBUTE *attribute_new(
			char *attribute_name );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *attribute_system_string(
			char *where );

LIST *attribute_system_list(
			char *system_string );

ATTRIBUTE *attribute_parse(
			char *input );

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
LIST *attribute_list(	void );

ATTRIBUTE *attribute_seek(
			char *attribute_name,
			LIST *attribute_list );

boolean attribute_exists(
			char *attribute_name,
			LIST *attribute_list );

LIST *attribute_extract_float_list(
			LIST *attribute_list );

/* Returns static memory */
/* --------------------- */
char *attribute_full_attribute_name(
			char *folder_table_name,
			char *attribute_name );

boolean attribute_is_date(
			char *datatype );

boolean attribute_is_time(
			char *datatype );

boolean attribute_is_number(
			char *datatype );

boolean attribute_is_date_time(
			char *datatype );

boolean attribute_is_text(
			char *datatype );

boolean attribute_is_notepad(
			char *datatype );

boolean attribute_is_password(
			char *datatype );

boolean attribute_is_upload(
			char *datatype );

boolean attribute_is_timestamp(
			char *datatype );

boolean attribute_is_yes_no(
			char *attribute_name );

boolean attribute_is_boolean(
			char *attribute_name );

#endif
