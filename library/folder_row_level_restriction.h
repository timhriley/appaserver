/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/libary/folder_row_level_restriction.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FOLDER_ROW_LEVEL_RESTRICTION_H
#define FOLDER_ROW_LEVEL_RESTRICTION_H

#include "list.h"
#include "boolean.h"

#define FOLDER_ROW_LEVEL_RESTRICTION_TABLE	"table_row_level_restriction"
#define FOLDER_ROW_LEVEL_RESTRICTION_SELECT	"row_level_restriction"

typedef struct
{
	boolean non_owner_viewonly;
	boolean non_owner_forbid;
} FOLDER_ROW_LEVEL_RESTRICTION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction_fetch(
		char *folder_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction_parse(
		char *input );

/* Process */
/* ------- */
FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction_calloc(
		void );

boolean folder_row_level_restriction_non_owner_viewonly(
		char *input );

boolean folder_row_level_restriction_non_owner_forbid(
		char *input );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *folder_row_level_restriction_string(
		char *folder_name );

#endif

