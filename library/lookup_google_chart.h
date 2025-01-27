/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_google_chart.h			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef LOOKUP_GRACE_H
#define LOOKUP_GRACE_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "post_dictionary.h"
#include "folder_row_level_restriction.h"
#include "dictionary_separate.h"
#include "query.h"
#include "google_chart.h"

#define LOOKUP_GRACE_EXECUTABLE		"output_grace_chart"

typedef struct
{
	ROLE *role;
	LIST *role_attribute_exclude_lookup_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_attribute_date_name_list;
	LIST *folder_attribute_name_list;
	FOLDER_ROW_LEVEL_RESTRICTION *
		folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	char *security_entity_where;
	POST_DICTIONARY *post_dictionary;
	DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *
		dictionary_separate;
	QUERY_CHART *query_chart;
	int row_list_length;
} LOOKUP_GRACE_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_GRACE_INPUT *lookup_grace_input_new(
			char *application_name,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *dictionary_string );

/* Process */
/* ------- */
LOOKUP_GRACE_INPUT *lookup_grace_input_calloc(
			void );

typedef struct
{
	LOOKUP_GRACE_INPUT *lookup_grace_input;
	QUERY_CHART *query_chart;
	int query_row_list_days_range;
	char *title;
	GRACE_SINGLE *grace_single;
	GRACE_WINDOW *grace_window;
} LOOKUP_GRACE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_GRACE *lookup_grace_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *dictionary_string,
			char *document_root_directory );

/* Process */
/* ------- */
LOOKUP_GRACE *lookup_grace_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *lookup_grace_title(
			char *folder_name );

#endif
