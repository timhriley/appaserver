/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/row_security_role_update.h	   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ROW_SECURITY_ROLE_UPDATE_H
#define ROW_SECURITY_ROLE_UPDATE_H

#include "list.h"
#include "folder.h"
#include "boolean.h"
#include "relation_one2m.h"

#define ROW_SECURITY_ROLE_UPDATE_SELECT		"table_name,"		\
						"column_not_null,"	\
						"no_override_yn"

#define ROW_SECURITY_ROLE_UPDATE_TABLE		"row_security_role_update"

typedef struct
{
	char *role_name;
	char *folder_name;
	char *attribute_not_null;
	boolean no_override_boolean;
	LIST *relation_one2m_recursive_list;

	/* Set externally */
	/* -------------- */
	RELATION_ONE2M *relation_one2m;
} ROW_SECURITY_ROLE_UPDATE;

/* Usage */
/* ----- */
ROW_SECURITY_ROLE_UPDATE *row_security_role_update_parse(
		char *role_name,
		char *input );

/* Process */
/* ------- */
ROW_SECURITY_ROLE_UPDATE *row_security_role_update_calloc(
		void );

/* Usage */
/* ----- */
ROW_SECURITY_ROLE_UPDATE *row_security_role_update_seek(
		char *folder_name,
		LIST *list );

/* Usage */
/* ----- */
ROW_SECURITY_ROLE_UPDATE *row_security_role_update_relation_seek(
		char *folder_name,
		LIST *row_security_role_update_list );

/* Usage */
/* ----- */
RELATION_ONE2M *row_security_role_update_relation_one2m(
		LIST *relation_one2m_recursive_list,
		char *folder_name );

typedef struct
{
	LIST *cache_list;
	ROW_SECURITY_ROLE_UPDATE *
		row_security_role_update;
	char *attribute_not_null;
	ROW_SECURITY_ROLE_UPDATE *
		row_security_role_update_relation;
	char *from;
	char *join_where;
} ROW_SECURITY_ROLE_UPDATE_LIST;

/* Usage */
/* ----- */

/* Returns null if not participating */
/* --------------------------------- */
ROW_SECURITY_ROLE_UPDATE_LIST *row_security_role_update_list_fetch(
		char *role_name,
		char *folder_name,
		boolean role_override_row_restrictions );

/* Process */
/* ------- */
ROW_SECURITY_ROLE_UPDATE_LIST *
	row_security_role_update_list_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *row_security_role_update_list_join_where(
		char *folder_name,
		char *one_folder_name,
		LIST *foreign_key_list );

/* Usage */
/* ----- */

/* Returns static LIST * */
/* --------------------- */
LIST *row_security_role_update_list_cache_list(
		char *role_name );

#endif
