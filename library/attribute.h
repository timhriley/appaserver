/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/attribute.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "list.h"
#include "boolean.h"
#include "dictionary.h"

#define ATTRIBUTE_TABLE			"appaserver_column"

#define ATTRIBUTE_SELECT		"column_name,"			\
					"column_datatype,"		\
					"width,"			\
					"float_decimal_places,"		\
					"hint_message"

#define ATTRIBUTE_MULTI_KEY_DELIMITER	'^'
/* #define ATTRIBUTE_MULTI_KEY_DELIMITER	0x1e */

#define ATTRIBUTE_NAME_SORT_ORDER	"sort_order"
#define ATTRIBUTE_NAME_DISPLAY_ORDER	"display_order"
#define ATTRIBUTE_NAME_SEQUENCE_NUMBER	"sequence_number"

#define ATTRIBUTE_PRIMARY_KEY		"column_name"

#define ATTRIBUTE_DATATYPE_CHARACTER	"character"
#define ATTRIBUTE_DATATYPE_NOTEPAD	"notepad"
#define ATTRIBUTE_DATATYPE_INTEGER	"integer"

typedef struct
{
	char *attribute_name;
	char *datatype_name;
	int width;
	int float_decimal_places;
	char *hint_message;
} ATTRIBUTE;

/* Usage */
/* ----- */
ATTRIBUTE *attribute_fetch(
		char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *attribute_system_string(
		char *attribute_select,
		char *attribute_table,
		char *where );

LIST *attribute_system_list(
		char *attribute_system_string );

ATTRIBUTE *attribute_parse(
		char *input );

/* Usage */
/* ----- */
ATTRIBUTE *attribute_new(
		char *attribute_name );

/* Process */
/* ------- */
ATTRIBUTE *attribute_calloc(
		void );

/* Usage */
/* ----- */
ATTRIBUTE *attribute_seek(
		char *attribute_name,
		LIST *attribute_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *attribute_insert_statement(
		char *attribute_select,
		char *attribute_table,
		ATTRIBUTE *attribute );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *attribute_insert_statement_string(
		char *attribute_select,
		char *attribute_table,
		char *attribute_name,
		char *datatype_name,
		char *width_string,
		char *float_string,
		char *hint_string );

/* Usage */
/* ----- */

/* Returns number or null */
/* ---------------------- */
char *attribute_trim_number_characters(
		char *number );

/* Usage */
/* ----- */

/* Returns dictionary_small() */
/* -------------------------- */
DICTIONARY *attribute_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		LIST *attribute_name_list,
		LIST *attribute_data_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *attribute_insert_value_string(
		int integer,
		char *string );

/* Usage */
/* ----- */
void attribute_folder_name_list_stdout(
		char *folder_name );

/* Usage */
/* ----- */
void attribute_name_list_stdout(
		const char *attribute_table,
		const char *attribute_primary_key );

/* Usage */
/* ----- */
void attribute_name_list_sort_stdout(
		LIST *attribute_name_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *attribute_name_trim_datatype(
		char *attribute_name,
		boolean attribute_is_date,
		boolean attribute_is_time );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
LIST *attribute_list( void );

/* Usage */
/* ----- */
boolean attribute_exists(
		char *attribute_name,
		LIST *attribute_list );

/* Public */
/* ------ */

LIST *attribute_extract_float_list(
		LIST *attribute_list );

/* Returns attribute_name or heap memory */
/* ------------------------------------- */
char *attribute_full_attribute_name(
		char *folder_table_name,
		char *attribute_name );

boolean attribute_is_date(
		char *datatype_name );

boolean attribute_is_time(
		char *datatype_name );

boolean attribute_is_date_time(
		char *datatype_name );

boolean attribute_is_current_date_time(
		char *datatype_name );

boolean attribute_is_current_date(
		char *datatype_name );

boolean attribute_is_float(
		char *datatype_name );

boolean attribute_is_integer(
		const char *attribute_datatype_integer,
		char *datatype_name );

boolean attribute_is_number(
		char *datatype_name );

boolean attribute_is_character(
		const char *attribute_datatype_character,
		char *datatype_name );

boolean attribute_is_notepad(
		const char *attribute_datatype_notepad,
		char *datatype_name );

boolean attribute_is_password(
		char *datatype_name );

boolean attribute_is_encrypt(
		char *datatype_name );

boolean attribute_is_upload(
		char *datatype_name );

boolean attribute_is_timestamp(
		char *datatype_name );

boolean attribute_is_yes_no(
		char *attribute_name );

int attribute_width(
		char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *attribute_database_datatype(
		char *datatype_name,
		int width,
		int float_decimal_places,
		int primary_key_index,
		char *default_value );

/* Returns static memory */
/* --------------------- */
char *attribute_delete_statement(
		const char *attribute_table,
		char *attribute_name );

LIST *attribute_data_list(
		char delimiter,
		char *data_list_string );

#endif
