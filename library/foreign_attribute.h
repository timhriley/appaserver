/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/foreign_attribute.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FOREIGN_ATTRIBUTE_H
#define FOREIGN_ATTRIBUTE_H

#include "boolean.h"
#include "list.h"

#define FOREIGN_ATTRIBUTE_TABLE		"foreign_column"

#define FOREIGN_ATTRIBUTE_SELECT	"table_name,"		\
					"related_table,"	\
					"related_column,"	\
					"foreign_column,"	\
					"foreign_key_index"

#define FOREIGN_ATTRIBUTE_PRIMARY_KEY	"table_name,"		\
					"related_table,"	\
					"related_column,"	\
					"foreign_column"

typedef struct
{
	char *folder_name;
	char *related_folder_name;
	char *related_attribute_name;
	char *foreign_attribute_name;
	int foreign_key_index;
} FOREIGN_ATTRIBUTE;

/* Usage */
/* ----- */
LIST *foreign_attribute_list(
		char *many_folder_name,
		char *one_folder_name,
		char *related_attribute_name );

/* Usage */
/* ----- */
FOREIGN_ATTRIBUTE *foreign_attribute_parse(
		char *input );

/* Usage */
/* ----- */
FOREIGN_ATTRIBUTE *foreign_attribute_new(
		char *folder_name,
		char *related_folder_name,
		char *related_attribute_name,
		char *foreign_attribute_name );

/* Process */
/* ------- */
FOREIGN_ATTRIBUTE *foreign_attribute_calloc(
		void );

/* Usage */
/* ----- */
LIST *foreign_attribute_name_list(
		LIST *foreign_attribute_list );

/* Usage */
/* ----- */
LIST *foreign_attribute_fetch_list(
		void );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *foreign_attribute_system_string(
		char *foreign_attribute_select,
		char *foreign_attribute_table );

/* Usage */
/* ----- */
LIST *foreign_attribute_system_list(
		char *foreign_attribute_system_string );

/* Usage */
/* ----- */
LIST *foreign_attribute_seek_list(
		char *many_folder_name,
		char *one_folder_name,
		char *related_attribute_name,
		LIST *foreign_attribute_list );

#endif

