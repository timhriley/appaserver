/* $APPASERVER_HOME/library/choose_isa_drop_down.h	*/
/* -----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* -----------------------------------------------	*/

#ifndef CHOOSE_ISA_DROP_DOWN_H
#define CHOOSE_ISA_DROP_DOWN_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "folder.h"

/* Constants */
/* --------- */
#define CHOOSE_ISA_DROP_DOWN_PROMPT_PREFIX	"choose_"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ------*/
	char *login_name;
	char *session_key;
	char *folder_name;
	char *one2m_isa_folder_name;
	char *role_name;

	/* Process */
	/* ------- */
	FOLDER *folder;
	LIST *delimited_list;
	LIST *element_list;
} CHOOSE_ISA_DROP_DOWN;

/* Operations */
/* ---------- */
CHOOSE_ISA_DROP_DOWN *choose_isa_drop_down_calloc(
			void );

CHOOSE_ISA_DROP_DOWN *choose_isa_drop_down_fetch(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *login_name,
			char *session_key,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name );

LIST *choose_isa_drop_down_element_list(
			char *one2m_isa_folder_name,
			LIST *primary_key_list,
			LIST *delimited_list );
#endif
