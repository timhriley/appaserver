/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/units.h		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#ifndef UNITS_H
#define UNITS_H

/* Includes */
/* -------- */
#include "list.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	int special_code;
	char *replacement_string;
} UNITS_SPECIAL_CODE_STRUCTURE;

typedef struct
{
	char *units_converted;
	double multiply_by;
} UNITS_CONVERTED;

typedef struct
{
	char *units_alias_name;
	char *units_name;
} UNITS_ALIAS;

typedef struct
{
	char *units_name;
	LIST *units_alias_list;
	LIST *units_converted_list;
} UNITS;

/* Operations */
/* ---------- */
UNITS_SPECIAL_CODE_STRUCTURE *units_special_code_structure_new(
				int special_code,
				char *replacement_string );

UNITS_CONVERTED *units_converted_new(
				void );

UNITS_ALIAS *units_alias_new(
				void );

UNITS *units_new(		void );

LIST *units_fetch_units_alias_list(
				char *application_name,
				char *units_name );

LIST *units_fetch_local_units_alias_list(
				char *application_name );

LIST *units_fetch_units_list(
				char *application_name );

LIST *units_list(		char *application_name );

UNITS *units_seek_alias_new(	char *application_name,
				char *units_name );

UNITS_ALIAS *units_alias_seek(	LIST *units_alias_list,
				char *units_alias_name );

void units_free(		UNITS *units );

UNITS *units_seek(		LIST *units_list,
				char *units_name );

UNITS *units_fetch(		char *application_name,
				char *units_name );

char *units_translate_units_name(
				LIST *units_alias_list,
				char *units_name,
				char *units_seek_phrase );

char *units_search_replace_special_codes(
				char *source );

LIST *units_list(		char *application_name );

char *units_select(		void );

#endif
