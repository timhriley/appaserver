/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_spreadsheet.h			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef LOOKUP_SPREADSHEET_H
#define LOOKUP_SPREADSHEET_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "post_dictionary.h"
#include "folder_row_level_restriction.h"
#include "dictionary_separate.h"
#include "query.h"
#include "appaserver_link.h"

#define LOOKUP_SPREADSHEET_EXECUTABLE		"output_spreadsheet"

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
	POST_DICTIONARY *post_dictionary;
	APPASERVER_LINK *appaserver_link;
	char *output_filename;
	char *prompt_filename;
	DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *dictionary_separate;
} LOOKUP_SPREADSHEET_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_SPREADSHEET_INPUT *lookup_spreadsheet_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory );

/* Process */
/* ------- */
LOOKUP_SPREADSHEET_INPUT *lookup_spreadsheet_input_calloc(
		void );

typedef struct
{
	LOOKUP_SPREADSHEET_INPUT *lookup_spreadsheet_input;
	QUERY_SPREADSHEET *query_spreadsheet;
	char *title_string;
	char *sub_title_string;
} LOOKUP_SPREADSHEET;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_SPREADSHEET *lookup_spreadsheet_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory );

/* Process */
/* ------- */
LOOKUP_SPREADSHEET *lookup_spreadsheet_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *lookup_spreadsheet_title_string(
		char *folder_name );


/* Returns heap memory */
/* ------------------- */
char *lookup_spreadsheet_sub_title_string(
		char *query_table_edit_where_string );


/* Driver */
/* ------ */
void lookup_spreadsheet_output(
		char *application_name,
		LOOKUP_SPREADSHEET *lookup_spreadsheet );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *lookup_spreadsheet_prompt_message(
		unsigned long row_count );

#endif
