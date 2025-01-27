/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_drillthru.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_DRILLTHRU_H
#define POST_DRILLTHRU_H

#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "dictionary_separate.h"
#include "drillthru.h"

#define POST_DRILLTHRU_EXECUTABLE	"post_drillthru"

typedef struct
{
	char *base_folder_name;
	LIST *folder_attribute_list;
	LIST *folder_attribute_date_name_list;
	DICTIONARY_SEPARATE_DRILLTHRU *
		dictionary_separate_drillthru;
	DRILLTHRU_CONTINUE *drillthru_continue;
	DICTIONARY *dictionary_separate_send_dictionary;
	char *dictionary_separate_send_string;
	char *appaserver_error_filename;
	char *system_string;
} POST_DRILLTHRU;

/* Usage */
/* ----- */
POST_DRILLTHRU *post_drillthru_new(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *current_folder_name,
			char *state,
			char *appaserver_data_directory,
			DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
POST_DRILLTHRU *post_drillthru_calloc(
			void );

/* Returns component of original_post_dictionary */
/* --------------------------------------------- */
char *post_drillthru_base_folder_name(
			char *dictionary_separate_drillthru_prefix,
			char *drillthru_base_key,
			DICTIONARY *original_post_dictionary );

#endif
