/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rectification.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RECTIFICATION_H
#define RECTIFICATION_H

#include <stdio.h>
#include "boolean.h"
#include "list.h"

typedef struct
{
	char *column_name;
	int primary_key_index;
} RECTIFICATION_INDEX;

/* Usage */
/* ----- */
LIST *rectification_index_list(
		char *table_name );

/* Usage */
/* ----- */
RECTIFICATION_INDEX *rectification_index_parse(
		char *table_name,
		char *input );

/* Usage */
/* ----- */
RECTIFICATION_INDEX *rectification_index_new(
		char *column_name );

/* Process */
/* ------- */
RECTIFICATION_INDEX *rectification_index_calloc(
		void );

int rectification_index_primary_key_index(
		char *primary_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rectification_index_system_string(
		char *table_name );

/* Usage */
/* ----- */
RECTIFICATION_INDEX *rectification_index_seek(
		LIST *rectification_index_list,
		char *column_name );

typedef struct
{
	char *column_name;
	char *datatype_text;
	int primary_key_index;
	int width;
	int float_decimal_places;
	char *datatype_name;
} RECTIFICATION_COLUMN;

/* Usage */
/* ----- */
LIST *rectification_column_list(
		LIST *rectification_index_list,
		char *table_name );

/* Usage */
/* ----- */
RECTIFICATION_COLUMN *rectification_column_parse(
		LIST *rectification_index_list,
		char *input );

/* Usage */
/* ------- */
RECTIFICATION_COLUMN *rectification_column_new(
		char *column_name );

/* Process */
/* ------- */
RECTIFICATION_COLUMN *rectification_column_calloc(
		void );

int rectification_column_width(
		char *datatype_text );

int rectification_column_float_decimal_places(
		char *datatype_text );

/* Returns heap memory */
/* ------------------- */
char *rectification_column_datatype_name(
		char *datatype_text );	

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rectification_column_system_string(
		char *table_name );

/* Usage */
/* ----- */
RECTIFICATION_COLUMN *rectification_column_seek(
		char *column_name,
		LIST *rectification_column_list );

typedef struct
{
	char *table_name;
	LIST *rectification_index_list;
	LIST *rectification_column_list;
} RECTIFICATION_TABLE;

/* Usage */
/* ----- */
LIST *rectification_table_list(
		void );

/* Usage */
/* ----- */
RECTIFICATION_TABLE *rectification_table_new(
		char *table_name );

/* Process */
/* ------- */
RECTIFICATION_TABLE *rectification_table_calloc(
		void );

/* Usage */
/* ----- */
RECTIFICATION_TABLE *rectification_table_seek(
		char *table_name,
		LIST *rectification_table_list );

typedef struct
{
	char *attribute_name;
	int primary_key_index;
	char *datatype_name;
	int width;
	int float_decimal_places;
} RECTIFICATION_ATTRIBUTE;

/* Usage */
/* ----- */
LIST *rectification_attribute_list(
		LIST *folder_attribute_list );

/* Usage */
/* ----- */
RECTIFICATION_ATTRIBUTE *rectification_attribute_new(
		char *attribute_name,
		int primary_key_index,
		char *datatype_name,
		int width,
		int float_decimal_places );

/* Process */
/* ------- */
RECTIFICATION_ATTRIBUTE *rectification_attribute_calloc(
		void );

/* Usage */
/* ----- */
RECTIFICATION_ATTRIBUTE *rectification_attribute_seek(
		char *attribute_name,
		LIST *rectification_attribute_list );

typedef struct
{
	char *folder_name;
	char *folder_table_name;
	LIST *rectification_attribute_list;
} RECTIFICATION_FOLDER;

/* Usage */
/* ----- */
LIST *rectification_folder_list(
		char *application_name,
		LIST *folder_list );

/* Usage */
/* ----- */
RECTIFICATION_FOLDER *rectification_folder_new(
		char *application_name,
		char *folder_name,
		LIST *folder_attribute_list );

/* Process */
/* ------- */
RECTIFICATION_FOLDER *rectification_folder_calloc(
		void );

/* Public */
/* ------ */
RECTIFICATION_FOLDER *rectification_folder_seek(
		char *folder_table_name,
		LIST *rectification_folder_list );

typedef struct
{
	char *folder_name;
	RECTIFICATION_ATTRIBUTE *rectification_attribute;
	RECTIFICATION_COLUMN *rectification_column;
	char *missing_column_name;
	boolean width_boolean;
	boolean float_boolean;
	boolean key_boolean;
	boolean datatype_boolean;
	char *missing_attribute_name;
	char *drop_column_href_string;
	char *drop_column_tag;
	char *missing_table;
	char *missing_folder;
	char *drop_table_href_string;
	char *drop_table_tag;
} RECTIFICATION_MISMATCH;

/* Usage */
/* ----- */
RECTIFICATION_MISMATCH *rectification_mismatch_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *rectification_attribute_list,
		LIST *rectification_column_list );

/* Process */
/* ------- */
RECTIFICATION_MISMATCH *rectification_mismatch_calloc(
		void );

boolean rectification_mismatch_width_boolean(
		int attribute_width,
		int column_width );

boolean rectification_mismatch_float_boolean(
		int attribute_float_decimal_places,
		int column_float_decimal_places );

boolean rectification_mismatch_key_boolean(
		int attribute_primary_key_index,
		int column_primary_key_index );

boolean rectification_mismatch_datatype_boolean(
		char *attribute_datatype_name,
		int attribute_width,
		char *column_datatype_name );

/* Returns static memory */
/* --------------------- */
char *rectification_mismatch_column_prompt(
		char *missing_attribute_name );

/* Returns heap memory */
/* ------------------- */
char *rectification_mismatch_drop_column_tag(
		char *drop_column_href_string,
		char *target_frame,
		char *rectification_mismatch_column_prompt );

/* Usage */
/* ----- */
RECTIFICATION_MISMATCH *rectification_mismatch_no_table_new(
		char *folder_name );

/* Usage */
/* ----- */
RECTIFICATION_MISMATCH *rectification_mismatch_no_folder_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *table_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *rectification_mismatch_table_prompt(
		char *missing_table );

/* Returns heap memory */
/* ------------------- */
char *rectification_mismatch_drop_table_tag(
		char *drop_table_href_string,
		char *target_frame,
		char *rectification_mismatch_table_prompt );

typedef struct
{
	LIST *folder_list;
	LIST *rectification_folder_list;
	LIST *rectification_table_list;
	LIST *rectification_mismatch_list;
	LIST *not_in_role_table_name_list;
} RECTIFICATION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RECTIFICATION *rectification_fetch(
		char *application_name,
		char *session_key,
		char *login_name );

/* Process */
/* ------- */
RECTIFICATION *rectification_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *rectification_role_list_string(
		void );

/* Usage */
/* ----- */
LIST *rectification_not_in_role_table_name_list(
		const char *folder_table,
		const char *role_folder_table,
		const char *folder_primary_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rectification_not_in_role_table_system_string(
		const char *folder_table,
		const char *role_folder_table,
		const char *folder_primary_key );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *rectification_not_in_role_table_where(
		const char *role_folder_table,
		const char *folder_primary_key );

#endif
