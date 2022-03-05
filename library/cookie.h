/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/cookie.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef COOKIE_H
#define COOKIE_H

#include "list.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */

/* COOKIE operations */
/* ----------------- */

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *cookie_key(	char *form_name,
			char *folder_name );

/* Returns static memory */
/* --------------------- */
char *cookie_multi_key(	char *form_name,
			char *folder_name );

/* Returns heap memory or null */
/* --------------------------- */
char *cookie_keystrokes_save_string(
			char *form_name,
			char *cookie_key,
			LIST *element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *cookie_keystrokes_multi_save_string(
			char *form_name,
			char *cookie_key,
			LIST *element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *cookie_keystrokes_recall_string(
			char *form_name,
			char *cookie_key,
			LIST *element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *cookie_keystrokes_multi_recall_string(
			char *form_name,
			char *cookie_key,
			LIST *element_list );

#endif
