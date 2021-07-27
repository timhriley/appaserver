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

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define FOREIGN_ATTRIBUTE_PRIMARY_KEY	"folder,"		\
					"related_folder,"	\
					"related_attribute,"	\
					"foreign_attribute"

#define FOREIGN_ATTRIBUTE_TABLE		"foreign_attribute"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *folder;
	char *related_folder;
	char *related_attribute;
	char *foreign_attribute;
	int foreign_key_index;

	/* Process */
	/* ------- */
	LIST *foreign_attribute_name_list;

} FOREIGN_ATTRIBUTE;

/* Prototypes */
/* ---------- */
FOREIGN_ATTRIBUTE *foreign_attribute_new(
			char *folder,
			char *related_folder,
			char *related_attribute,
			char *foreign_attribute );

FOREIGN_ATTRIBUTE *foreign_attribute_parse(
			char *input );

LIST *foreign_attribute_system_list(
			char *system_string );

/* Safely returns heap memory */
/* --------------------------- */
char *foreign_attribute_system_string(
			char *where );

LIST *foreign_many_folder_attribute_list(
			char *many_folder_name,
			LIST *foreign_attribute_list );

LIST *foreign_attribute_list(
			char *many_folder_name,
			char *one_folder_name );

LIST *foreign_attribute_name_list(
			LIST *foreign_attribute_list );

#endif

