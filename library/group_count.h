/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/group_count.h		   		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef GROUP_COUNT_H
#define GROUP_COUNT_H

#define GROUP_COUNT_EXECUTABLE		"output_group"

#include <stdio.h>
#include "boolean.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "post_dictionary.h"
#include "folder_row_level_restriction.h"
#include "dictionary_separate.h"
#include "appaserver_link.h"
#include "relation_mto1.h"
#include "query_statistic.h"

typedef struct
{
	RELATION_MTO1 *relation_mto1;
	char *title;
	QUERY_GROUP *query_group;
	APPASERVER_LINK *appaserver_link;
	char *prompt_filename;
	char *output_filename;
	char *anchor_html;
	char *html_table_system_string;
	char *csv_heading_string;
	char *csv_heading_system_string;
	char *csv_output_system_string;
} GROUP_RELATION;

/* Usage */
/* ----- */
LIST *group_relation_list(
		char *application_name,
		char *folder_name,
		char *data_directory,
		char *query_table_edit_where_string,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_float_name_list,
		LIST *no_display_name_list,
		unsigned long group_count_row_count,
		pid_t process_id );

/* Process */
/* ------- */
pid_t group_relation_process_id_increment(
		pid_t process_id );


/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GROUP_RELATION *group_relation_new(
		char *application_name,
		char *folder_name,
		char *data_directory,
		char *query_table_edit_where_string,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_float_name_list,
		unsigned long group_count_row_count,
		RELATION_MTO1 *relation_mto1,
		pid_t group_relation_process_id_increment );

/* Process */
/* ------- */
GROUP_RELATION *group_relation_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *group_relation_title(
		char *folder_name,
		char *relation_prompt );

/* Returns static memory */
/* --------------------- */
char *group_relation_csv_heading_string(
		LIST *folder_attribute_float_name_list,
		LIST *foreign_key_list );

/* Returns heap memory */
/* ------------------- */
char *group_relation_csv_heading_system_string(
		char *output_filename,
		char *group_relation_csv_heading );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *group_relation_html_table_system_string(
		char sql_delimiter,
		int html_table_queue_top_bottom,
		LIST *folder_attribute_float_name_list,
		LIST *foreign_key_list,
		char *group_relation_title );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *group_relation_html_heading_string(
		LIST *folder_attribute_float_name_list,
		LIST *foreign_key_list );

/* Returns static memory */
/* --------------------- */
char *group_relation_html_justify_string(
		int foreign_key_list_length );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *group_relation_csv_output_system_string(
		char sql_delimiter,
		char *output_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *group_relation_anchor_html(
		char *one_folder_name,
		char *prompt_filename );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *group_relation_prompt_message(
		char *one_folder_name );

/* Usage */
/* ----- */
void group_relation_output(
		unsigned long group_count_row_count,
		GROUP_RELATION *group_relation );

/* Process */
/* ------- */
FILE *group_relation_csv_output_pipe(
		char *csv_output_system_string );

FILE *group_relation_html_output_pipe(
		char *html_table_system_string );

FILE *group_relation_input_pipe(
		char *query_system_string );

/* Returns static memory */
/* --------------------- */
char *group_relation_html_row_count_string(
		char sql_delimiter,
		unsigned long group_count_row_count,
		int foreign_key_list_length );

typedef struct
{
	ROLE *role;
	LIST *role_attribute_exclude_lookup_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_attribute_date_name_list;
	LIST *folder_attribute_float_name_list;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	POST_DICTIONARY *post_dictionary;
	DICTIONARY_SEPARATE_TABLE_EDIT *dictionary_separate;
	char *primary_key_first;
} GROUP_COUNT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GROUP_COUNT_INPUT *group_count_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string );

/* Process */
/* ------- */
GROUP_COUNT_INPUT *group_count_input_calloc(
		void );

/* Returns primary_key_first */
/* ------------------------- */
char *group_count_input_primary_key_first(
		char *primary_key_first );

typedef struct
{
	GROUP_COUNT_INPUT *group_count_input;
	QUERY_STATISTIC *query_statistic;
	unsigned long row_count;
	LIST *group_relation_list;
	char *title_string;
	char *sub_title_string;
} GROUP_COUNT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GROUP_COUNT *group_count_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *appaserver_parameter_data_directory );

/* Process */
/* ------- */
GROUP_COUNT *group_count_calloc(
		void );

/* Returns primary_key_first */
/* ------------------------- */
char *group_count_primary_key_first(
		char *primary_key_first );

/* Returns static memory */
/* --------------------- */
char *group_count_title_string(
		char *folder_name );

/* Returns heap memory */
/* ------------------- */
char *group_count_sub_title_string(
		char *query_table_edit_where_string );

/* Usage */
/* ----- */
unsigned long group_count_row_count(
		QUERY_STATISTIC_FETCH *query_statistic_fetch );

/* Driver */
/* ------ */
void group_count_output(
		char *application_name,
		GROUP_COUNT *group_count );

#endif
