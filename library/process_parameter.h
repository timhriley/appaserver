/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_parameter.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_PARAMETER_H
#define PROCESS_PARAMETER_H

#include "list.h"
#include "boolean.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "dictionary.h"
#include "process.h"
#include "folder_row_level_restriction.h"
#include "query.h"
#include "attribute.h"

#define PROCESS_PARAMETER_TABLE		"process_parameter"

#define PROCESS_PARAMETER_SELECT	"process,"			\
					"table_name,"			\
					"column_name,"			\
					"drop_down_prompt,"		\
					"prompt,"			\
					"display_order,"		\
					"drop_down_multi_select_yn,"	\
					"drillthru_yn,"			\
					"populate_drop_down_process,"	\
					"populate_helper_process"

#define PROCESS_SET_PARAMETER_TABLE	"process_set_parameter"

#define PROCESS_SET_PARAMETER_SELECT	"process_set,"			\
					"table_name,"			\
					"column_name,"			\
					"drop_down_prompt,"		\
					"prompt,"			\
					"display_order,"		\
					"drop_down_multi_select_yn,"	\
					"drillthru_yn,"			\
					"populate_drop_down_process,"	\
					"populate_helper_process"

#define PROCESS_PARAMETER_PROCESS_GROUP_TABLE				\
		"process_group"

#define PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE			\
		"drop_down_prompt_data"

#define PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_SELECT			\
		"drop_down_prompt_data,"				\
		"display_order"

#define PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE			\
		"drop_down_prompt"

#define PROCESS_PARAMETER_DROP_DOWN_PROMPT_SELECT			\
		"hint_message,"						\
		"optional_display"

#define PROCESS_PARAMETER_PROMPT_TABLE	"prompt"

#define PROCESS_PARAMETER_PROMPT_SELECT	"input_width,"			\
					"hint_message,"			\
					"upload_filename_yn,"		\
					"date_yn"

typedef struct
{
	char *drop_down_prompt_name;
	char *drop_down_prompt_data;
	int display_order;
} PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA;

/* Usage */
/* ----- */
LIST *process_parameter_drop_down_prompt_data_list(
		char *drop_down_prompt_name,
		char *process_parameter_drop_down_prompt_primary_where );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *process_parameter_drop_down_prompt_data_system_string(
		char *process_parameter_drop_down_prompt_select,
		char *process_parameter_drop_down_prompt_table,
		char *process_parameter_drop_down_prompt_primary_where );

/* Usage */
/* ----- */
LIST *process_parameter_drop_down_prompt_data_system_list(
		char *drop_down_prompt_name,
		char *process_parameter_drop_down_prompt_data_system_string );

/* Process */
/* ------- */
FILE *process_parameter_drop_down_prompt_data_pipe(
		char *process_parameter_drop_down_prompt_data_system_string );

/* Usage */
/* ----- */
PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
	process_parameter_drop_down_prompt_data_parse(
		char *drop_down_prompt_name,
		char *string_input );

/* Usage */
/* ----- */
PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
	process_parameter_drop_down_prompt_data_new(
		char *drop_down_prompt_name,
		char *drop_down_prompt_data );

/* Process */
/* ------- */
PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
	process_parameter_drop_down_prompt_data_calloc(
		void );

/* Usage */
/* ----- */
LIST *process_parameter_drop_down_prompt_member_data_list(
		char *drop_down_prompt_name,
		LIST *member_name_list );

/* Usage */
/* ----- */
LIST *process_parameter_drop_down_prompt_data_in_clause_list(
		const char *process_parameter_drop_down_prompt_data_table,
		LIST *process_parameter_drop_down_prompt_list );

/* Usage */
/* ----- */
LIST *process_parameter_drop_down_prompt_data_option_list(
		LIST *process_parameter_drop_down_prompt_data_list );

typedef struct
{
	char *drop_down_prompt_name;
	boolean multi_select_boolean;
	char *hint_message;
	char *optional_display;
	LIST *data_list;
	LIST *data_option_list;
} PROCESS_PARAMETER_DROP_DOWN_PROMPT;

/* Usage */
/* ----- */
PROCESS_PARAMETER_DROP_DOWN_PROMPT *
	process_parameter_drop_down_prompt_fetch(
		char *drop_down_prompt_name,
		boolean multi_select_boolean );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *process_parameter_drop_down_prompt_primary_where(
		char *drop_down_prompt_name );

/* Returns heap memory */
/* ------------------- */
char *process_parameter_drop_down_prompt_system_string(
		char *process_parameter_drop_down_prompt_select,
		char *process_parameter_drop_down_prompt_table,
		char *process_parameter_drop_down_prompt_primary_where );

/* Usage */
/* ----- */
PROCESS_PARAMETER_DROP_DOWN_PROMPT *
	process_parameter_drop_down_prompt_parse(
		char *drop_down_prompt_name,
		boolean multi_select_boolean,
		char *process_parameter_drop_down_prompt_primary_where,
		char *string_pipe_input );

/* Process */
/* ------- */
PROCESS_PARAMETER_DROP_DOWN_PROMPT *drop_down_prompt_calloc(
		void );

/* Usage */
/* ----- */
PROCESS_PARAMETER_DROP_DOWN_PROMPT *
	process_parameter_drop_down_prompt_set_member(
		char *process_set_default_prompt,
		char *prompt_display_text,
		LIST *member_name_list );

/* Process */
/* ------- */

/* Returns either parameter */
/* ------------------------ */
char *process_parameter_drop_down_prompt_set_member_name(
		char *process_set_default_prompt,
		char *prompt_display_text );

typedef struct
{
	char *prompt_name;
	int input_width;
	char *hint_message;
	boolean upload_filename_boolean;
	boolean date_boolean;
} PROCESS_PARAMETER_PROMPT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_PARAMETER_PROMPT *process_parameter_prompt_fetch(
		char *prompt_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *process_parameter_prompt_primary_where(
		char *prompt_name );

/* Returns heap memory */
/* ------------------- */
char *process_parameter_prompt_system_string(
		char *process_parameter_prompt_select,
		char *process_parameter_prompt_table,
		char *process_parameter_prompt_where );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_PARAMETER_PROMPT *process_parameter_prompt_parse(
		char *prompt_name,
		char *input );

/* Usage */
/* ----- */
PROCESS_PARAMETER_PROMPT *process_parameter_prompt_new(
		char *prompt_name );

/* Process */
/* ------- */
PROCESS_PARAMETER_PROMPT *process_parameter_prompt_calloc(
		void );

typedef struct
{
	boolean multi_select_boolean;
	FOLDER *folder;
	boolean fetch_mto1;
	LIST *relation_mto1_list;
	char *name;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	ROLE *role;
	SECURITY_ENTITY *security_entity;
	QUERY_DROP_DOWN *query_drop_down;
	PROCESS *process;
	LIST *delimited_list;
	boolean folder_no_initial_capital;
	LIST *widget_drop_down_option_list;
} PROCESS_PARAMETER_DROP_DOWN;

/* Usage */
/* ----- */
PROCESS_PARAMETER_DROP_DOWN *
	process_parameter_drop_down_process_fetch(
		char *folder_name,
		char *prompt_name,
		boolean multi_select_boolean,
		char *populate_drop_down_process_name );

/* Process */
/* ------- */
PROCESS_PARAMETER_DROP_DOWN *
	process_parameter_drop_down_calloc(
		void );

/* Usage */
/* ----- */
PROCESS_PARAMETER_DROP_DOWN *
	process_parameter_drop_down_folder_fetch(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *drillthru_dictionary,
		boolean multi_select_boolean );

/* Process */
/* ------- */
boolean process_parameter_drop_down_fetch_mto1(
		boolean drillthru_dictionary_length );

/* Usage */
/* ----- */

/* Returns prompt_name, "" or heap memory */
/* -------------------------------------- */
char *process_parameter_drop_down_name(
		char *folder_name,
		char *prompt_name,
		LIST *folder_attribute_primary_key_list );

typedef struct
{
	ATTRIBUTE *attribute;
} PROCESS_PARAMETER_ATTRIBUTE;

/* Usage */
/* ----- */
PROCESS_PARAMETER_ATTRIBUTE *process_parameter_attribute_fetch(
		char *attribute_name );

/* Process */
/* ------- */
PROCESS_PARAMETER_ATTRIBUTE *process_parameter_attribute_calloc(
		void );

typedef struct
{
	char *prompt_name;
	PROCESS_PARAMETER_PROMPT *process_parameter_prompt;
} PROCESS_PARAMETER_YES_NO;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_PARAMETER_YES_NO *process_parameter_yes_no_fetch(
		char *prompt_name );

/* Process */
/* ------- */
PROCESS_PARAMETER_YES_NO *process_parameter_yes_no_calloc(
		void );

typedef struct
{
	char *process_or_set_name;
	char *folder_name;
	char *attribute_name;
	char *drop_down_prompt_name;
	char *prompt_name;
	int display_order;
	boolean drop_down_multi_select;
	boolean drillthru;
	char *populate_drop_down_process_name;
	char *populate_helper_process_name;
	PROCESS_PARAMETER_DROP_DOWN *process_parameter_drop_down;
	PROCESS_PARAMETER_ATTRIBUTE *process_parameter_attribute;
	PROCESS_PARAMETER_YES_NO *process_parameter_yes_no;
	PROCESS_PARAMETER_DROP_DOWN_PROMPT *process_parameter_drop_down_prompt;
	PROCESS_PARAMETER_PROMPT *process_parameter_prompt;
} PROCESS_PARAMETER;

/* Usage */
/* ----- */
LIST *process_parameter_system_list(
		char *application_name,
		char *login_name,
		char *role_name,
		DICTIONARY *drillthru_dictionary,
		char *system_string );

/* Process */
/* ------- */
FILE *process_parameter_input_pipe(
		char *system_string );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_PARAMETER *process_parameter_parse(
		char *application_name,
		char *login_name,
		char *role_name,
		DICTIONARY *drillthru_dictionary,
		char *input );

/* Process */
/* ------- */
PROCESS_PARAMETER *process_parameter_calloc(
		void );

/* Usage */
/* ----- */
LIST *process_parameter_folder_name_list(
		LIST *process_parameter_list );

/* Usage */
/* ----- */

/* Either prepends or appends to process_parameter_list */
/* ---------------------------------------------------- */
LIST *process_parameter_set_member_append(
		LIST *process_parameter_list,
		char *process_set_default_prompt,
		char *prompt_display_text,
		LIST *member_name_list );

/* Usage */
/* ----- */
LIST *process_parameter_drop_down_prompt_list(
		LIST *process_parameter_list );

/* Usage */
/* ----- */
LIST *process_parameter_prompt_list(
		LIST *process_parameter_list );

/* Usage */
/* ----- */
LIST *process_parameter_folder_name_list(
		LIST *process_parameter_list );

/* Usage */
/* ----- */
LIST *process_parameter_upload_filename_list(
		LIST *process_parameter_list );

/* Usage */
/* ----- */
LIST *process_parameter_date_name_list(
		LIST *process_parameter_list );

/* Usage */
/* ----- */
boolean process_parameter_date_boolean(
		LIST *process_parameter_list );

/* Usage */
/* ----- */
boolean process_parameter_drillthru_boolean(
		char *process_name,
		char *process_set_name );

/* Usage */
/* ----- */
LIST *process_parameter_list(
		char *process_name,
		char *process_set_name,
		boolean is_drillthru );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *process_parameter_system_string(
		const char *process_parameter_select,
		const char *process_parameter_table,
		char *process_parameter_where );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *process_parameter_set_system_string(
		const char *process_set_parameter_select,
		const char *process_set_parameter_table,
		char *process_parameter_set_where );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *process_parameter_where(
		char *process_name,
		boolean is_drillthru );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *process_parameter_set_where(
		char *process_set_name,
		boolean is_drillthru );

/* Usage */
/* ----- */
LIST *process_parameter_set_list(
		char *process_set_name );

#endif

