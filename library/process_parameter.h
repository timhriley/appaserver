/* $APPASERVER_HOME/library/process_parameter.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_PARAMETER_H
#define PROCESS_PARAMETER_H

#include "list.h"
#include "boolean.h"
#include "folder.h"
#include "attribute.h"

#define PROCESS_PARAMETER_TABLE		"process_parameter"

#define PROCESS_PARAMETER_SELECT	"process,"			\
					"folder,"			\
					"attribute,"			\
					"drop_down_prompt,"		\
					"prompt,"			\
					"display_order,"		\
					"drop_down_multi_select_yn,"	\
					"drillthru_yn,"			\
					"populate_drop_down_process,"	\
					"populate_helper_process"

#define PROCESS_SET_PARAMETER_TABLE	"process_set_parameter"

#define PROCESS_SET_PARAMETER_SELECT	"process_set,"			\
					"folder,"			\
					"attribute,"			\
					"drop_down_prompt,"		\
					"prompt,"			\
					"display_order,"		\
					"drop_down_multi_select_yn,"	\
					"drillthru_yn,"			\
					"populate_drop_down_process,"	\
					"populate_helper_process"

#define PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE			\
					"drop_down_prompt_data"

#define PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_SELECT			\
					"drop_down_prompt_data,"	\
					"display_order"

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

LIST *process_parameter_drop_down_prompt_data_system_list(
		char *drop_down_prompt_name,
		char *process_parameter_drop_down_prompt_data_system_string );

PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
	process_parameter_drop_down_prompt_data_parse(
			char *drop_down_prompt_name,
			char *input );

PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
	process_parameter_drop_down_prompt_data_new(
			char *drop_down_prompt_name,
			char *drop_down_prompt_data );

PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
	process_parameter_drop_down_prompt_data_calloc(
			void );

#define PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE			\
					"drop_down_prompt"

#define PROCESS_PARAMETER_DROP_DOWN_PROMPT_SELECT			\
					"hint_message,"			\
					"optional_display"

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *drop_down_prompt_name;
	char *hint_message;
	char *optional_display;

	/* Process */
	/* ------- */
	LIST *process_parameter_drop_down_prompt_data_list;
} PROCESS_PARAMETER_DROP_DOWN_PROMPT;

/* Usage */
/* ----- */
PROCESS_PARAMETER_DROP_DOWN_PROMPT *
	process_parameter_drop_down_prompt_fetch(
			char *drop_down_prompt_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *process_parameter_drop_down_prompt_primary_where(
			char *drop_down_prompt_name );

/* Returns static memory */
/* --------------------- */
char *process_parameter_drop_down_prompt_system_string(
		char *process_parameter_drop_down_prompt_select,
		char *process_parameter_drop_down_prompt_table,
		char *process_parameter_drop_down_prompt_primary_where );

PROCESS_PARAMETER_DROP_DOWN_PROMPT *drop_down_prompt_parse(
			char *drop_down_prompt_name,
			char *input );

PROCESS_PARAMETER_DROP_DOWN_PROMPT *drop_down_prompt_calloc(
			void );

#define PROCESS_PARAMETER_PROMPT_TABLE	"prompt"

#define PROCESS_PARAMETER_PROMPT_SELECT	"prompt,"			\
					"input_width,"			\
					"hint_message,"			\
					"upload_filename_yn,"		\
					"date_yn"

typedef struct
{
	char *prompt_name;
	int input_width;
	char *hint_message;
	boolean upload_filename;
	boolean date;
} PROCESS_PARAMETER_PROMPT;

/* Usage */
/* ----- */
PROCESS_PARAMETER_PROMPT *
	process_parameter_prompt_fetch(
		char *prompt_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *process_parameter_prompt_primary_where(
			char *prompt_name );

/* Returns static memory */
/* --------------------- */
char *process_parameter_prompt_system_string(
			char *process_parameter_prompt_select,
			char *process_parameter_prompt_table,
			char *process_parameter_prompt_where );

PROCESS_PARAMETER_PROMPT *
	process_parameter_prompt_parse(
		char *input );

PROMPT *prompt_new(	char *prompt_name );

typedef struct
{
	/* Attributes */
	/* ---------- */
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

	/* Process */
	/* ------- */
	LIST *delimited_list;
	FOLDER *folder;
	ATTRIBUTE *attribute;
	DROP_DOWN_PROMPT *drop_down_prompt;
	PROMPT *prompt;
} PROCESS_PARAMETER;

/* Usage */
/* ----- */
LIST *process_parameter_system_list(
			char *system_string,
			char *role_name,
			char *login_name,
			DICTIONARY *drillthru_dictionary );

/* Usage */
/* ----- */
PROCESS_PARAMETER *process_parameter_parse(
			char *input,
			char *role_name,
			char *login_name,
			DICTIONARY *drillthru_dictionary );

/* Process */
/* ------- */
PROCESS_PARAMETER *process_parameter_new(
			char *process_or_set_name,
			char *folder_name,
			char *attribute_name,
			char *drop_down_prompt_name,
			char *prompt_name );

PROCESS_PARAMETER *process_parameter_calloc(
			void );

LIST *process_parameter_process_delimited_list(
			char *populate_drop_down_process_name,
			char *login_name,
			char *role_name,
			DICTIONARY *drillthru_dictionary );

LIST *process_parameter_folder_delimited_list(
			char *widget_folder_name,
			char *login_name,
			char *role_name,
			DICTIONARY *drillthru_dictionary );

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *process_parameter_where(
			char *process_name,
			boolean is_drillthru );

/* Returns heap memory */
/* ------------------- */
char *process_parameter_system_string(
			char *process_parameter_select,
			char *process_parameter_table,
			char *process_parameter_where );

/* Returns static memory */
/* --------------------- */
char *process_parameter_set_where(
			char *process_set_name,
			boolean is_drillthru );

/* Returns heap memory */
/* ------------------- */
char *process_parameter_set_system_string(
			char *process_set_parameter_select,
			char *process_set_parameter_table,
			char *process_parameter_set_where );

#endif

