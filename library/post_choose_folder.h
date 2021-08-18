/* $APPASERVER_HOME/library/post_choose_folder.h	*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#ifndef POST_CHOOSE_FOLDER_H
#define POST_CHOOSE_FOLDER_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "role.h"
#include "folder.h"
#include "role_folder.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ------*/
	char *application_name;
	char *login_name;
	char *session_key;
	char *folder_name;
	char *role_name;
	char *state;

	/* Process */
	/* ------- */
	ROLE *role;
	FOLDER *folder;
	ROLE_FOLDER *role_folder;
} POST_CHOOSE_FOLDER;

/* Operations */
/* ---------- */
POST_CHOOSE_FOLDER *post_choose_folder_calloc(
			void );

POST_CHOOSE_FOLDER *post_choose_folder_fetch(
			/* ------------------------------------- */
			/* Each parameter was security inspected */
			/* ------------------------------------- */
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *state );

#endif
