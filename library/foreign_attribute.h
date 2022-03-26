/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/foreign_attribute.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef FOREIGN_ATTRIBUTE_H
#define FOREIGN_ATTRIBUTE_H

#include "boolean.h"
#include "list.h"

#define FOREIGN_ATTRIBUTE_SELECT	"folder,"		\
					"related_folder,"	\
					"related_attribute,"	\
					"foreign_attribute,"	\
					"foreign_key_index"

#define FOREIGN_ATTRIBUTE_PRIMARY_KEY	"folder,"		\
					"related_folder,"	\
					"related_attribute,"	\
					"foreign_attribute"

#define FOREIGN_ATTRIBUTE_TABLE		"foreign_attribute"

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *folder_name;
	char *related_folder;
	char *related_attribute;
	char *foreign_attribute;
	int foreign_key_index;
} FOREIGN_ATTRIBUTE;

/* FOREIGN_ATTRIBUTE operations */
/* ---------------------------- */

/* Usage */
/* ----- */
LIST *foreign_attribute_list(
			char *many_folder_name,
			char *one_folder_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *foreign_attribute_where(
			char *many_folder_name,
			char *one_folder_name );

/* Returns heap memory */
/* ------------------- */
char *foreign_attribute_system_string(
			char *foreign_attribute_select,
			char *foreign_attribute_table,
			char *foreign_attribute_where );

LIST *foreign_attribute_system_list(
			char *foreign_attribute_system_string );

FOREIGN_ATTRIBUTE *foreign_attribute_parse(
			char *input );

FOREIGN_ATTRIBUTE *foreign_attribute_new(
			char *folder_name,
			char *related_folder,
			char *related_attribute,
			char *foreign_attribute );

FOREIGN_ATTRIBUTE *foreign_attribute_calloc(
			void );

/* Public */
/* ------ */
LIST *foreign_related_attribute_name_list(
			LIST *foreign_attribute_list );

LIST *foreign_attribute_name_list(
			LIST *foreign_attribute_list );

#endif

